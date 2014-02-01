#ifndef PLANNER_IDA_HPP
#define PLANNER_IDA_HPP

#include "definitions/types.hpp"
#include "definitions/state.hpp"

template<typename Problem>
class IDASolver
{
public:
    IDASolver(Problem&& problem_ = Problem())
        : problem(problem_)
        , found(false)
    {
    }

    bool solve(BuildOrder& result)
    {
        Node start = problem.start_node();
        Time upper_bound = problem.upper_bound();
        Time lower_bound = problem.heuristic(start);

        while(true)
        {
            lower_bound = ida_search(start, lower_bound);
            // std::cerr << lower_bound << std::endl;
            if(found)
            {
                result = best;
                return true;
            }
            else if(lower_bound >= upper_bound)
            {
                return false;
            }
        }
    }
private:
    Time ida_search(const Node& n, Time limit)
    {
        Time f = n.t + problem.heuristic(n);
        if(f > limit)
        {
            return f;
        }
        else if(problem.is_goal(n))
        {
            found = true;
            best = extract_solution(n);
            return n.t;
        }
        else
        {
            Time min_f = std::numeric_limits<Time>::max();
            problem.visit_neighbors(n, [this, limit, &min_f](const Node& n) mutable {
                if(found)
                    return;

                Time new_f = ida_search(n, limit);
                if(new_f < min_f)
                {
                    min_f = new_f;
                }
            });
            return min_f;
        }
    }

    Problem problem;

    bool found;
    BuildOrder best;
};

#endif
