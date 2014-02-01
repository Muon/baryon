#ifndef PLANNER_DFBB_HPP
#define PLANNER_DFBB_HPP

#include <iostream>
#include <functional>

#include "definitions/state.hpp"

void log_partial_solution(const Node& final_state)
{
    for(const Node* n = &final_state; n != nullptr; n = n->predecessor)
    {
        if(n->predecessor)
        {
            std::cerr << *n << '\n';
        }
    }
    std::cerr.flush();
}

//! Depth-first branch and bound solver
template<typename Problem>
class DFBBSolver
{
public:
    DFBBSolver(Problem&& problem_ = Problem())
        : problem(problem_)
        , found(false)
    {
    }

    bool solve(BuildOrder& result)
    {
        found = false;
        upper_bound = problem.upper_bound();

        Node start = problem.start_node();
        dfbb(start);

        if(found)
        {
            result = best;
            return true;
        }
        else
        {
            return false;
        }
    }
private:
    void dfbb(const Node& n)
    {
        if(problem.is_goal(n))
        {
            if(n.t < upper_bound)
            {
                log_partial_solution(n); std::cout << std::endl;
                found = true;
                best = extract_solution(n);
                upper_bound = n.t;
            }
        }
        else
        {
            using namespace std::placeholders;
            problem.visit_neighbors(n, std::bind(&DFBBSolver::neighbor_visitor, this, _1));
        }
    }

    void neighbor_visitor(const Node& n)
    {
        if(n.t + problem.heuristic(n) < upper_bound)
        {
            dfbb(n);
        }
    }

    Problem problem;

    bool found;
    BuildOrder best;
    Time upper_bound;
};

#endif
