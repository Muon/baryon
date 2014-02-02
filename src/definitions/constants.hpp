#ifndef PLANNER_CONSTANTS_HPP
#define PLANNER_CONSTANTS_HPP

#include "types.hpp"

constexpr Time TICKS_PER_SECOND = 18;

constexpr Resource INITIAL_LC = 60;
constexpr Resource INITIAL_QP = 40;

// define to use the buggy RP timings
#define USE_ACHRON_1611_RPS

#ifdef USE_ACHRON_1611_RPS
constexpr Time LC_CYCLE_LENGTH = 268;
constexpr Time QP_CYCLE_LENGTH = 274;
#else
constexpr Time LC_CYCLE_LENGTH = 216 + 1;
constexpr Time QP_CYCLE_LENGTH = 270 + 1;
#endif

constexpr Time RP_SWITCH_TIME = 5 * TICKS_PER_SECOND;

constexpr Resource RP_YIELD_SIZE = 8;
constexpr Resource LC_YIELD_SIZE = RP_YIELD_SIZE;
constexpr Resource QP_YIELD_SIZE = RP_YIELD_SIZE;

// Structure specs
constexpr Time RP_BUILD_TIME = 20 * TICKS_PER_SECOND;
constexpr Resource RP_LC_COST = 80;

constexpr Time FOUNDATION_BUILD_TIME = 8 * TICKS_PER_SECOND;
constexpr Time FOUNDATION_LC_COST = 65;

constexpr Resource DEPOT_LC_COST = 50;
constexpr Resource DEPOT_QP_COST = 40;
constexpr Time DEPOT_BUILD_TIME = 42 * TICKS_PER_SECOND;

constexpr unsigned PULSERS_PER_DEPOT = 7;

constexpr Resource ZV_LC_COST = 36;
constexpr Time ZV_BUILD_TIME = 20 * TICKS_PER_SECOND;

constexpr Resource ZP_LC_COST = 29;
constexpr Resource ZP_QP_COST = 15;
constexpr Time ZP_BUILD_TIME = 32 * TICKS_PER_SECOND;
constexpr Time ZP_PILOT_TIME = ZP_BUILD_TIME - ZV_BUILD_TIME;

constexpr Resource SKIP_UPGRADE_LC_COST = 25;
constexpr Resource SKIP_UPGRADE_QP_COST = 10;
constexpr Time SKIP_UPGRADE_TIME = 540;

#endif
