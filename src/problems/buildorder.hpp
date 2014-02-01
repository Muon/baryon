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

class BuildOrderProblem
{
public:
    Node start_node()
    {
        Node start;
        start.t = 0;
        start.state.lc_rp_state = {-5 * TICKS_PER_SECOND,
                                   -5 * TICKS_PER_SECOND,
                                   -5 * TICKS_PER_SECOND};
        return start;
    }

    //! Return initial upper bound for plan length.
    Time upper_bound()
    {
        return 5 * 60 * TICKS_PER_SECOND;
    }

    bool is_goal(const Node& n)
    {
        return n.state.lc_rp_state.size() >= 3 && n.state.qp_rp_state.size() >= 5;
    }

    //! Return lower bound on time to goal from given node.
    Time heuristic(const Node& n)
    {
        std::size_t s = n.state.lc_rp_state.size() + n.state.qp_rp_state.size();
        if(s < 8)
        {
            return min_time_to_gather_lc(n, (8 - s) * RP_LC_COST);
        }
        else
        {
            return 0;
        }
    }

    template<typename T>
    void visit_neighbors(const Node& n, T visitor)
    {
        try_build_lc_rp(n, visitor);
        try_build_qp_rp(n, visitor);

        try_switch_lc_to_qp(n, visitor);
        try_switch_qp_to_lc(n, visitor);
    }
};

#endif
