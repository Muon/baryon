#ifndef PLANNER_BUILDORDER_HPP
#define PLANNER_BUILDORDER_HPP

#include "definitions/types.hpp"
#include "definitions/state.hpp"
#include "definitions/actions.hpp"

Time time_to_lc(State base, const Resource lc) {
    if(lc <= base.lc)
        return 0;

    Time dt = 0;
    int full_cycles = ((lc - base.lc) / LC_YIELD_SIZE) / base.lc_rp_state.size();
    dt += full_cycles * LC_CYCLE_LENGTH;
    if(dt > 0)
    {
        update(base, dt);
    }

    while(base.lc < lc)
    {
        update(base, 1);
        ++dt;
    }
    return dt;
}

Time min_time_to_gather_lc(Node n, const Resource lc) {
    for(std::size_t i = 0; i < n.state.qp_rp_state.size(); ++i)
    {
        n.state.lc_rp_state.push_back(-RP_SWITCH_TIME);
    }
    n.state.qp_rp_state.clear();

    Time t = time_to_lc(n.state, lc);
    while(true)
    {
        if(!can_build_lc_rp(n))
        {
            if(can_build_zv(n))
            {
                n = build_zv(n);
            }

            if(!can_build_lc_rp(n))
                break;
        }

        n = build_lc_rp(n);
        Time new_t = time_to_lc(n.state, lc);
        if(new_t < t)
        {
            t = new_t;
        }
        else
        {
            break;
        }
    }
    // std::cerr << t << std::endl;
    return t;
}

constexpr unsigned NUM_ZPS = 2;

class BuildOrderProblem
{
public:
    Node start_node()
    {
        Node start;
        start.t = 0;
        return start;
    }

    //! Return initial upper bound for plan length.
    Time upper_bound()
    {
        Node n = start_node();
        n = build_qp_rp(n);
        n = build_foundation(n);
        n = build_depot(n);
        for(unsigned i = 0; i < NUM_ZPS; ++i)
        {
            n = build_zp(n);
        }
        for(unsigned i = 0; i < NUM_ZPS; ++i)
        {
            n = upgrade_zp(n);
        }
        assert(is_goal(n));
        return n.t + 1;
    }

    bool is_goal(const Node& n)
    {
        return n.state.zp_upgrade_queue.size() + n.state.upgraded_zps >= NUM_ZPS;
    }

    //! Return lower bound on time to goal from given node.
    Time heuristic(const Node& n)
    {
        Time build_wait = 0;
        Resource lc_cost = 0;

        if(!has_ready_depot(n))
        {
            if(has_queued_depot(n))
            {
                build_wait += time_to_next_produced(n.state.depot_queue);
            }
            else
            {
                if(!has_foundation(n))
                {
                    build_wait += FOUNDATION_BUILD_TIME;
                    lc_cost += FOUNDATION_LC_COST;
                }
                build_wait += DEPOT_BUILD_TIME;
                lc_cost += DEPOT_LC_COST;
                // lc_cost += (QP_CYCLE_LENGTH * DEPOT_QP_COST) / LC_CYCLE_LENGTH;
            }
        }

        unsigned zps_upgraded = n.state.zp_upgrade_queue.size() + n.state.upgraded_zps;
        if(zps_upgraded < NUM_ZPS)
        {
            lc_cost += SKIP_UPGRADE_LC_COST * (NUM_ZPS - zps_upgraded);
            // lc_cost += (QP_CYCLE_LENGTH * SKIP_UPGRADE_QP_COST) / LC_CYCLE_LENGTH * (NUM_ZPS - zps_upgraded);
        }

        unsigned zps_produced = zps_upgraded + n.state.zp_queue.size() + n.state.zps;
        if(zps_produced < NUM_ZPS)
        {
            lc_cost += ZP_LC_COST * (NUM_ZPS - zps_produced);
            // lc_cost += (QP_CYCLE_LENGTH * ZP_QP_COST) / LC_CYCLE_LENGTH * (NUM_ZPS - zps_produced);
            build_wait += ZP_PILOT_TIME;
        }

        unsigned zvs_produced = zps_produced + n.state.zv_queue.size() + n.state.zvs;
        if(zvs_produced < NUM_ZPS)
        {
            lc_cost += ZV_LC_COST * (NUM_ZPS - zvs_produced);
        }

        return std::max(build_wait, min_time_to_gather_lc(n, lc_cost));
    }

    template<typename T>
    void visit_neighbors(const Node& n, T visitor)
    {
        // For up to 7 ZPs, we only need 1 Depot.
        if(!has_depot(n))
        {
            if(!has_foundation(n))
            {
                try_build_foundation(n, visitor);
            }
            try_build_depot(n, visitor);
        }

        try_build_zv(n, visitor);

        if(n.state.zvs <= 1)
        {
            try_build_zp(n, visitor);
        }
        try_pilot_zp(n, visitor);
        try_upgrade_zp(n, visitor);

        try_build_lc_rp(n, visitor);
        try_build_qp_rp(n, visitor);

        try_switch_lc_to_qp(n, visitor);
        try_switch_qp_to_lc(n, visitor);
    }
};

#endif
