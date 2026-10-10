/* include/decision.h*/

#ifndef DECISION_H
    #define DECISION_H

    #include <stddef.h>
    #include "analysis.h"

    typedef enum 
    {
        FASTEST,
        MEMORY_EFFICIENT,
        BEST_OVERALL
    } DecisionCriterion;

    typedef enum {
        DECISION_SUCCESS = 0,
        DECISION_INVALID_ARGUMENT = -1,
        DECISION_NO_VALID_RESULT = -2
    } DecisionStatus;

    int select_fastest_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected);
    int select_memory_efficient_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected);
    int select_best_algorithm(const analysis_result analyses[], size_t analysis_count, DecisionCriterion criterion, SortAlgorithm *selected);

#endif