#ifndef PLANNER_ACTIONS_HPP
#define PLANNER_ACTIONS_HPP

#include <cassert>
#include <functional>

#include "state.hpp"

Resource update_rps(const Time dt, RPState& rp_state, const Time cycle_length,
                    const Resource yield_size)
{
    Resource res = 0;
    for(Time& t : rp_state)
    {
        t += dt;
        if(t >= 0)
        {
            res += (t / cycle_length) * yield_size;
            t %= cycle_length;
        }
    }
    return res;
}

unsigned update_production(const Time dt, ProductionState& queue)
{
    for(Time& t : queue)
    {
        t += dt;
    }

    using namespace std::placeholders;
    auto iter = std::remove_if(queue.begin(), queue.end(),
                               std::bind(std::greater_equal<Time>(), _1, 0));
    unsigned n = queue.end() - iter;
    queue.erase(iter, queue.end());
    return n;
}

void update(State& state, const Time dt)
{
    assert(dt > 0);

    state.lc += update_rps(dt, state.lc_rp_state, LC_CYCLE_LENGTH, LC_YIELD_SIZE);
    state.qp += update_rps(dt, state.qp_rp_state, QP_CYCLE_LENGTH, QP_YIELD_SIZE);

    state.foundations += update_production(dt, state.foundation_queue);
    state.depots += update_production(dt, state.depot_queue);
    state.zvs += update_production(dt, state.zv_queue);
    state.zps += update_production(dt, state.zp_queue);
    state.upgraded_zps += update_production(dt, state.zp_upgrade_queue);
}

void update(Node& n, const Time dt)
{
    assert(dt > 0);

    update(n.state, dt);
    n.t += dt;
}

Time time_to_next_produced(const ProductionState& v)
{
    return -(*std::max_element(v.begin(), v.end()));
}

// Resources

void spend_resource(Node& state, Resource& res, const Resource target, const Resource yield_size, const Time cycle_length, const RPState& rps)
{
    if(res < target)
    {
        Time dt = 0;
        int full_cycles = ((target - res) / yield_size) / rps.size();
        dt += full_cycles * cycle_length;
        if(dt > 0)
        {
            update(state, dt);
        }

        while(res < target)
        {
            update(state, 1);
        }
    }
    res -= target;
}

bool can_spend_lc(const Node& n, Resource lc)
{
    return n.state.lc >= lc || !n.state.lc_rp_state.empty();
}

void spend_lc(Node& n, const Resource lc)
{
    spend_resource(n, n.state.lc, lc, LC_YIELD_SIZE, LC_CYCLE_LENGTH, n.state.lc_rp_state);
}

bool can_spend_qp(const Node& n, Resource qp)
{
    return n.state.qp >= qp || !n.state.qp_rp_state.empty();
}

void spend_qp(Node& n, const Resource qp)
{
    spend_resource(n, n.state.qp, qp, QP_YIELD_SIZE, QP_CYCLE_LENGTH, n.state.qp_rp_state);
}

// Buildings

#define TRY(what) template<typename T> void try_ ## what (const Node& n, T visitor) { if(can_ ## what (n)) { visitor((what)(n)); } }

bool has_zv(const Node& n)
{
    return n.state.zv_queue.size() + n.state.zvs >= 1;
}

void wait_for_zv(Node& n)
{
    if(n.state.zvs < 1)
    {
        assert(!n.state.zv_queue.empty());
        update(n, time_to_next_produced(n.state.zv_queue));
    }
}

bool can_build_lc_rp(const Node& n)
{
    return has_zv(n) &&
           can_spend_lc(n, RP_LC_COST);
}

Node build_lc_rp(const Node& n)
{
    Node result = n;
    result.action.description = "Build an LC RP.";
    result.predecessor = &n;

    wait_for_zv(result);
    spend_lc(result, RP_LC_COST);
    result.state.lc_rp_state.push_back(-RP_BUILD_TIME);

    return result;
}

TRY(build_lc_rp);

bool can_build_qp_rp(const Node& n)
{
    return has_zv(n) &&
           can_spend_lc(n, RP_LC_COST);
}

Node build_qp_rp(const Node& n)
{
    Node result = n;
    result.action.description = "Build a QP RP.";
    result.predecessor = &n;

    wait_for_zv(result);
    spend_lc(result, RP_LC_COST);
    result.state.qp_rp_state.push_back(-RP_BUILD_TIME);

    return result;
}

TRY(build_qp_rp);

Time time_to_cycle_switch(const Time t, const Time cycle_length)
{
    if(t >= 0)
    {
        return cycle_length - t;
    }
    else
    {
        // Not done building, but also needs to complete at least one cycle
        // to justify.
        return -t + cycle_length;
    }
}

RPState::iterator wait_for_idle_rp(Node& n, RPState& rps, const Time cycle_length)
{
    auto iter = std::min_element(rps.begin(), rps.end(), [cycle_length](Time a, Time b) {
        return time_to_cycle_switch(a, cycle_length) < time_to_cycle_switch(b, cycle_length);
    });
    update(n, time_to_cycle_switch(*iter, cycle_length));
    return iter;
}

bool can_switch_lc_to_qp(const Node& n)
{
    return !n.state.lc_rp_state.empty();
}

Node switch_lc_to_qp(const Node& n)
{
    Node result = n;
    result.action.description = "Switch an LC RP to QP.";
    result.predecessor = &n;

    result.state.lc_rp_state.erase(wait_for_idle_rp(result, result.state.lc_rp_state, LC_CYCLE_LENGTH));
    result.state.qp_rp_state.push_back(-RP_SWITCH_TIME);

    return result;
}

TRY(switch_lc_to_qp);

bool can_switch_qp_to_lc(const Node& n)
{
    return !n.state.qp_rp_state.empty();
}

Node switch_qp_to_lc(const Node& n)
{
    Node result = n;
    result.action.description = "Switch a QP RP to LC.";
    result.predecessor = &n;

    result.state.qp_rp_state.erase(wait_for_idle_rp(result, result.state.qp_rp_state, QP_CYCLE_LENGTH));
    result.state.lc_rp_state.push_back(-RP_SWITCH_TIME);

    return result;
}

TRY(switch_qp_to_lc);

void wait_for_annex(Node& n)
{
    if(!n.state.zv_queue.empty())
    {
        update(n, time_to_next_produced(n.state.zv_queue));
    }
}

bool can_build_zv(const Node& n)
{
    return n.state.annexes >= 1 && can_spend_lc(n, ZV_LC_COST);
}

Node build_zv(const Node& n)
{
    Node result = n;
    result.action.description = "Build a ZV.";
    result.predecessor = &n;

    wait_for_annex(result);
    spend_lc(result, ZV_LC_COST);
    result.state.zv_queue.push_back(-ZV_BUILD_TIME);

    return result;
}

TRY(build_zv);

bool has_ready_depot(const Node& n)
{
    return n.state.depots >= 1;
}

bool has_queued_depot(const Node& n)
{
    return n.state.depot_queue.size() >= 1;
}

bool has_depot(const Node& n)
{
    return has_ready_depot(n) || has_queued_depot(n);
}

void wait_for_depot(Node& n)
{
    if(has_ready_depot(n) && n.state.zp_queue.size() < PULSERS_PER_DEPOT * n.state.depots)
        return;

    // Wait for a new depot or until the next vehicle is done, whichever is faster.
    Time dt = std::numeric_limits<Time>::max();

    if(!n.state.depot_queue.empty())
    {
        dt = std::min(dt, time_to_next_produced(n.state.depot_queue));
    }

    assert(n.state.zp_queue.size() <= PULSERS_PER_DEPOT * n.state.depots);
    if(!n.state.zp_queue.empty() && n.state.zp_queue.size() == PULSERS_PER_DEPOT * n.state.depots)
    {
        dt = std::min(dt, time_to_next_produced(n.state.zp_queue));
    }

    assert(dt != std::numeric_limits<Time>::max());

    update(n, dt);
}

void use_zv(Node& n)
{
    --n.state.zvs;
}

bool can_pilot_zp(const Node& n)
{
    return has_depot(n) &&
           has_zv(n) &&
           can_spend_lc(n, ZP_LC_COST) &&
           can_spend_qp(n, ZP_QP_COST);
}

Node pilot_zp(const Node& n)
{
    Node result = n;
    result.action.description = "Pilot a ZP.";
    result.predecessor = &n;

    wait_for_zv(result);
    wait_for_depot(result);
    use_zv(result);
    spend_lc(result, ZP_LC_COST);
    spend_qp(result, ZP_QP_COST);
    result.state.zp_queue.push_back(-ZP_PILOT_TIME);

    return result;
}

TRY(pilot_zp);

bool can_build_zp(const Node& n)
{
    return has_depot(n) &&
           n.state.annexes >= 1 &&
           can_spend_lc(n, ZV_LC_COST + ZP_LC_COST) &&
           can_spend_qp(n, ZP_QP_COST);
}

Node build_zp(const Node& n)
{
    Node result = n;
    result.action.description = "Build a ZP.";
    result.predecessor = &n;

    wait_for_depot(result);
    spend_lc(result, ZV_LC_COST + ZP_LC_COST);
    spend_qp(result, ZP_QP_COST);
    result.state.zp_queue.push_back(-ZP_BUILD_TIME);

    return result;
}

TRY(build_zp);

bool has_zp(const Node& n)
{
    return n.state.zp_queue.size() + n.state.zps >= 1;
}

void wait_for_zp(Node& n)
{
    if(n.state.zps < 1)
    {
        assert(!n.state.zp_queue.empty());
        update(n, time_to_next_produced(n.state.zp_queue));
    }
}

void use_zp(Node& n)
{
    --n.state.zps;
}

bool can_upgrade_zp(const Node& n)
{
    return has_zp(n) &&
           can_spend_lc(n, SKIP_UPGRADE_LC_COST) &&
           can_spend_qp(n, SKIP_UPGRADE_QP_COST);
}

Node upgrade_zp(const Node& n)
{
    Node result = n;
    result.action.description = "Upgrade a ZP.";
    result.predecessor = &n;

    wait_for_zp(result);
    use_zp(result);
    spend_lc(result, SKIP_UPGRADE_LC_COST);
    spend_qp(result, SKIP_UPGRADE_QP_COST);
    result.state.zp_upgrade_queue.push_back(-SKIP_UPGRADE_TIME);

    return result;
}

TRY(upgrade_zp);

bool has_foundation(const Node& n)
{
    return n.state.foundation_queue.size() + n.state.foundations >= 1;
}

void wait_for_foundation(Node& n)
{
    if(n.state.foundations < 1)
    {
        assert(!n.state.foundation_queue.empty());
        update(n, time_to_next_produced(n.state.foundation_queue));
    }
}

void use_foundation(Node& n)
{
    --n.state.foundations;
}

bool can_build_depot(const Node& n)
{
    return has_foundation(n) &&
           can_spend_lc(n, DEPOT_LC_COST) &&
           can_spend_qp(n, DEPOT_QP_COST);
}

Node build_depot(const Node& n)
{
    Node result = n;
    result.action.description = "Build a Depot.";
    result.predecessor = &n;

    wait_for_foundation(result);
    use_foundation(result);
    spend_lc(result, DEPOT_LC_COST);
    spend_qp(result, DEPOT_QP_COST);
    result.state.depot_queue.push_back(-DEPOT_BUILD_TIME);

    return result;
}

TRY(build_depot);

bool can_build_foundation(const Node& n)
{
    return can_spend_lc(n, FOUNDATION_LC_COST);
}

Node build_foundation(const Node& n)
{
    Node result = n;
    result.action.description = "Build a Foundation.";
    result.predecessor = &n;

    spend_lc(result, FOUNDATION_LC_COST);
    result.state.foundation_queue.push_back(-FOUNDATION_BUILD_TIME);

    return result;
}

TRY(build_foundation);

#endif
