/* include/analysis.h*/

#ifndef ANALYSIS_H
    #define ANALYSIS_H
    #include <stddef.h>
    #include "benchmark.h"

    typedef struct 
    {
        SortAlgorithm algorithm;
        size_t array_size;
        DataDistribution distribution;
        SortOrder order;
        size_t sample_count;
        double average_execution_time;
        double average_comparisons;
        double average_swaps;
        double average_auxiliary_memory;
        double min_execution_time;
        double max_execution_time;
        double stddev_execution_time;
        double stddev_comparisons;
        double stddev_swaps;
        double stddev_auxiliary_memory;
    } analysis_result;

    typedef enum
    {
        ANALYSIS_SUCCESS = 0,
        ANALYSIS_INVALID_ARGUMENT = -1,
        ANALYSIS_ALLOCATION_ERROR = -2,
        ANALYSIS_EXPERIMENT_ERROR = -3,
        ANALYSIS_INTEGRITY_ERROR = -4
    }AnalysisStatus;

    int analyze_algorithm(const benchmark_result results[], size_t result_count, SortAlgorithm algorithm, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result *analysis);
    int analyze_all_algorithms(const benchmark_result results[], size_t result_count, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result analyses[], size_t analyses_capacity);
    int compare_algorithms(const analysis_result analyses[], size_t analysis_count);
#endif