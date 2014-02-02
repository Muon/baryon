#ifndef PLANNER_STATE_HPP
#define PLANNER_STATE_HPP

#include <functional>
#include <iosfwd>
#include <algorithm>

#include "constants.hpp"

struct State
{
    Resource lc = INITIAL_LC, qp = INITIAL_QP;

    unsigned annexes = 1;
    unsigned depots = 0;
    unsigned foundations = 0;
    unsigned zvs = 1;
    unsigned zps = 0;
    unsigned upgraded_zps = 0;

    RPState lc_rp_state = {-5 * TICKS_PER_SECOND, -5 * TICKS_PER_SECOND, -5 * TICKS_PER_SECOND};
    RPState qp_rp_state;

    ProductionState foundation_queue;
    ProductionState depot_queue;

    ProductionState zv_queue;
    ProductionState zp_queue;
    ProductionState zp_upgrade_queue;
};

inline bool operator==(const State& a, const State& b)
{
    return a.lc == b.lc && a.qp == b.qp &&

           a.annexes == b.annexes &&
           a.depots == b.depots &&
           a.foundations == b.foundations &&
           a.zvs == b.zvs &&
           a.zps == b.zps &&
           a.upgraded_zps == b.upgraded_zps &&

           a.lc_rp_state == b.lc_rp_state &&
           a.qp_rp_state == b.qp_rp_state &&

           a.foundation_queue == b.foundation_queue &&
           a.depot_queue == b.depot_queue &&
           a.zv_queue == b.zv_queue &&
           a.zp_queue == b.zp_queue &&
           a.zp_upgrade_queue == b.zp_upgrade_queue;
}

// From Boost.Functional/Hash
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

namespace std {
    template <> struct hash<State>
    {
        size_t operator()(const State& state) const
        {
            std::size_t seed = 0;
            hash_combine(seed, state.lc);
            hash_combine(seed, state.qp);
            hash_combine(seed, state.annexes);
            hash_combine(seed, state.depots);
            hash_combine(seed, state.zvs);
            hash_combine(seed, state.zps);
            hash_combine(seed, state.upgraded_zps);
            hash_combine(seed, state.lc_rp_state.size());
            hash_combine(seed, state.qp_rp_state.size());
            hash_combine(seed, state.foundation_queue.size());
            hash_combine(seed, state.depot_queue.size());
            hash_combine(seed, state.zv_queue.size());
            hash_combine(seed, state.zp_queue.size());
            hash_combine(seed, state.zp_upgrade_queue.size());

            return seed;
        }
    };
}

struct Node
{
    const Node* predecessor = nullptr;
    Action action = {"<no action>"};
    Time t;

    State state;
};

extern std::ostream& operator<<(std::ostream& os, const Node& n);
BuildOrder extract_solution(const Node& final_state);
void print_solution(const BuildOrder& solution);

#endif
