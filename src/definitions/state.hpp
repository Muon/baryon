#ifndef PLANNER_STATE_HPP
#define PLANNER_STATE_HPP

#include <functional>
#include <iosfwd>
#include <algorithm>

#include "constants.hpp"

struct State
{
    Resource lc = INITIAL_LC, qp = INITIAL_QP;

    RPState lc_rp_state;
    RPState qp_rp_state;
};

inline bool operator==(const State& a, const State& b)
{
    return a.lc == b.lc && a.lc_rp_state == b.lc_rp_state && a.qp_rp_state == b.qp_rp_state;
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
            hash_combine(seed, state.lc_rp_state.size());
            hash_combine(seed, state.qp_rp_state.size());
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
