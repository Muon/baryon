#include <iostream>
#include <cassert>

#include "state.hpp"

std::ostream& operator<<(std::ostream& os, const Node& n)
{
    assert(n.action.description != nullptr);

    Time m = n.t / (TICKS_PER_SECOND * 60);
    Time s_ = (n.t % (TICKS_PER_SECOND * 60)) / TICKS_PER_SECOND;
    Time t = n.t % TICKS_PER_SECOND;
    return os << "[" << m << "m " << s_ << "s " << t << "t" << "] " << n.action.description << " LC: " << n.state.lc << " QP: " << n.state.qp;
}

BuildOrder extract_solution(const Node& final_state)
{
    BuildOrder result;
    for(const Node* n = &final_state; n != nullptr; n = n->predecessor)
    {
        result.push_back(*n);
    }
    std::reverse(result.begin(), result.end());
    for(auto iter = result.begin() + 1, end = result.end(); iter != end; ++iter)
    {
        iter->predecessor = &(*(iter - 1));
    }
    return result;
}

void print_solution(const BuildOrder& solution)
{
    for(const Node& n : solution)
    {
        // Don't print out the start node.
        if(&n == &solution.front())
            continue;

        std::cout << n << '\n';
    }
    std::cout.flush();
}
