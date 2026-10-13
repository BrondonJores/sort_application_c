/* include/benchmark.h*/

#ifndef BENCHMARK_H
    #define BENCHMARK_H
    #include <stddef.h>
    #include "sorting.h"
    #include "data.h"

    typedef enum 
    {
        BUBBLE,
        INSERTION,
        SELECTION,
        MERGE,
        QUICK,
        HEAP,
        ALL_ALGORITHMS
    } SortAlgorithm;

    typedef enum
    {
        BENCHMARK_SUCCESS = 0,
        BENCHMARK_INVALID_ARGUMENT = -1,
        BENCHMARK_ALLOCATION_ERROR = -2,
        BENCHMARK_SORT_ERROR = -3,
        BENCHMARK_INTEGRITY_ERROR = -4
    }BenchmarkStatus;

    typedef struct 
    {
        SortAlgorithm algorithm;
        size_t array_size;
        DataDistribution distribution;
        SortOrder order;
        sort_stats stats;
    } benchmark_result;

    int benchmark_one(const table *t, SortAlgorithm algorithm, SortOrder order, sort_stats *stats);

    int benchmark_all(const table *t, SortOrder order, DataDistribution distribution, benchmark_result results[], size_t results_capacity);
#endif