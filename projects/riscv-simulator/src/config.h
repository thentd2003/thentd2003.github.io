#ifndef __CONFIG_H__
#define __CONFIG_H__

// Enable one test configuration at a time, then run:
// make clean && make

// MS1
// #define DEBUG_REG_TRACE
// #define DEBUG_CYCLE
// #define MEM_LATENCY 0

// MS2 Set 1
// #define DEBUG_REG_TRACE
// #define DEBUG_CYCLE
// #define PRINT_STATS
// #define MEM_LATENCY 0

// MS2 Set 2
// #define PRINT_STATS
// #define MEM_LATENCY 0

// MS3 detailed cache test
#define DEBUG_REG_TRACE
#define DEBUG_CYCLE
#define PRINT_STATS
#define MEM_LATENCY 100
#define CACHE_ENABLE
#define PRINT_CACHE_STATS
#define PRINT_CACHE_TRACES

// MS3 cache summary
// #define PRINT_STATS
// #define MEM_LATENCY 100
// #define CACHE_ENABLE
// #define PRINT_CACHE_STATS

// MS3 no-cache baseline
// #define PRINT_STATS
// #define MEM_LATENCY 100
// #define PRINT_CACHE_STATS

// MS4 two-level cache
// Enable DEBUG_REG_TRACE, DEBUG_CYCLE, and PRINT_CACHE_TRACES
// only for the functionality test.
//
// #define DEBUG_REG_TRACE
// #define DEBUG_CYCLE
// #define PRINT_CACHE_TRACES
// #define PRINT_STATS
// #define MEM_LATENCY 100
// #define TWO_LEVEL_CACHE_ENABLE

// MS4 branch predictor
// Enable one predictor together with the MS4 cache configuration.
//
// #define BRANCH_PREDICT_1BIT
// #define BRANCH_PREDICT_2BIT

#endif // __CONFIG_H__