#ifndef PLANNER_TYPES_HPP
#define PLANNER_TYPES_HPP

#include <vector>

struct Node;

typedef int Time;
typedef unsigned int Resource;
typedef std::vector<Node> BuildOrder;

struct Action
{
    const char* description;
};

typedef std::vector<Time> RPState;
typedef std::vector<Time> ProductionState;

#endif
