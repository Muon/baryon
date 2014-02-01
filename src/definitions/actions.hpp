#ifndef PLANNER_ACTIONS_HPP
#define PLANNER_ACTIONS_HPP

#include <cassert>

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

void update(State& state, const Time dt)
{
    assert(dt > 0);

    state.lc += update_rps(dt, state.lc_rp_state, LC_CYCLE_LENGTH, LC_YIELD_SIZE);
    state.qp += update_rps(dt, state.qp_rp_state, QP_CYCLE_LENGTH, QP_YIELD_SIZE);
}

void update(Node& n, const Time dt)
{
    assert(dt > 0);

    update(n.state, dt);
    n.t += dt;
}


// Resources

void spend_resource(Node& state, Resource& res, const Resource target)
{
    while(res < target)
    {
        update(state, 1);
    }
    res -= target;
}

bool can_spend_lc(const Node& n, Resource lc)
{
    return n.state.lc >= lc || !n.state.lc_rp_state.empty();
}

void spend_lc(Node& n, const Resource lc)
{
    spend_resource(n, n.state.lc, lc);
}

bool can_spend_qp(const Node& n, Resource qp)
{
    return n.state.qp >= qp || !n.state.qp_rp_state.empty();
}

void spend_qp(Node& n, const Resource qp)
{
    spend_resource(n, n.state.qp, qp);
}

// Buildings

#define TRY(what) template<typename T> void try_ ## what (const Node& n, T visitor) { if(can_ ## what (n)) { visitor((what)(n)); } }

bool can_build_lc_rp(const Node& n)
{
    return can_spend_lc(n, RP_LC_COST);
}

Node build_lc_rp(const Node& n)
{
    Node result = n;
    spend_lc(result, RP_LC_COST);
    result.action.description = "Build an LC RP.";
    result.predecessor = &n;

    result.state.lc_rp_state.push_back(-RP_BUILD_TIME);

    return result;
}

TRY(build_lc_rp);

bool can_build_qp_rp(const Node& n)
{
    return can_spend_lc(n, RP_LC_COST);
}

Node build_qp_rp(const Node& n)
{
    Node result = n;
    spend_lc(result, RP_LC_COST);
    result.action.description = "Build a QP RP.";
    result.predecessor = &n;

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

#endif
