#ifndef PLANNER_ASTAR_HPP
#define PLANNER_ASTAR_HPP

#include <deque>
#include <queue>
#include <unordered_set>
#include <iostream>

#include "definitions/types.hpp"
#include "definitions/state.hpp"

struct AstarNode
{
    Time f, h, g;
    const Node* n;
    unsigned int depth;
};

bool operator>(const AstarNode& a, const AstarNode& b) {
    return a.f > b.f || (a.f == b.f && a.h > b.h) || (a.f == b.f && a.h == b.h && a.depth > b.depth);
    // return a.f > b.f || (a.f == b.f && a.depth > b.depth) || (a.f == b.f && a.depth == b.depth && a.h > b.h);
}

template<typename Problem>
class AStarSolver
{
public:
    AStarSolver(Problem&& problem_ = Problem())
        : problem(problem_)
    {
    }

    bool solve(BuildOrder& result)
    {
        Node start = problem.start_node();

        std::deque<Node> nodes;

        std::priority_queue<AstarNode, std::vector<AstarNode>, std::greater<AstarNode>> open;
        std::unordered_set<State> closed;

        open.push(AstarNode {problem.heuristic(start), problem.heuristic(start), 0, &start, 0});

        while(!open.empty())
        {
            AstarNode node = open.top();
            open.pop();
            auto r = closed.insert(node.n->state);
            if(!r.second) // State already in closed set.
            {
                // Duplicate states may arise from not having decrease-key.
                continue;
            }

            if(problem.is_goal(*node.n))
            {
                result = extract_solution(*node.n);
                std::cerr << "Enqueued " << nodes.size() << " nodes." << std::endl;
                std::cerr << "Expanded " << (nodes.size() - open.size()) << " nodes." << std::endl;
                std::cerr << "Closed set load factor: " << closed.load_factor() << std::endl;
                return true;
            }
            else
            {
                problem.visit_neighbors(*node.n, [this, &open, &closed, &nodes, &node](Node&& n) mutable {
                    if(closed.count(n.state) == 0)
                    {
                        Time h = problem.heuristic(n);
                        Time g = n.t;

                        nodes.push_back(std::move(n));

                        open.push(AstarNode { g + h, h, g, &nodes.back(), node.depth + 1 });
                    }
                });
            }
        }

        return false;
    }
private:
    Problem problem;
};

#endif
