#ifndef PLANNER_CONSTANTS_HPP
#define PLANNER_CONSTANTS_HPP

#include "types.hpp"

constexpr Time TICKS_PER_SECOND = 18;

constexpr Resource INITIAL_LC = 60;
constexpr Resource INITIAL_QP = 40;

constexpr Time LC_CYCLE_LENGTH = 268;
constexpr Time QP_CYCLE_LENGTH = 274;

constexpr Time RP_SWITCH_TIME = 5 * TICKS_PER_SECOND;

constexpr Resource RP_YIELD_SIZE = 8;
constexpr Resource LC_YIELD_SIZE = RP_YIELD_SIZE;
constexpr Resource QP_YIELD_SIZE = RP_YIELD_SIZE;

// Structure specs
constexpr Time RP_BUILD_TIME = 20 * TICKS_PER_SECOND;
constexpr Resource RP_LC_COST = 80;

#endif
