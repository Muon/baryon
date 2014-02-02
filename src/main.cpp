#include <iostream>

// define to use branch and bound instead of A*
// #define USE_DFBB

#ifdef USE_DFBB
#include "solvers/dfbb.hpp"
#else
#include "solvers/astar.hpp"
#endif

#include "problems/buildorder.hpp"
#include "definitions/types.hpp"

int main()
{
#ifdef USE_DFBB
    DFBBSolver<BuildOrderProblem> solver;
#else
    AStarSolver<BuildOrderProblem> solver;
#endif
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
