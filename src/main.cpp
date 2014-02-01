#include <iostream>

#include "solvers/astar.hpp"
#include "problems/buildorder.hpp"
#include "definitions/types.hpp"

int main()
{
    AStarSolver<BuildOrderProblem> solver;
    BuildOrder solution;

    if(solver.solve(solution))
    {
        print_solution(solution);
        return 0;
    }
    else
    {
        std::cerr << "Failed to find solution." << std::endl;
        return 1;
    }
}
