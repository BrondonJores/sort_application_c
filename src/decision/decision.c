#include "decision.h"

#include <stddef.h>
#include <float.h>

/* Fonctions internes */

static int same_experiment(const analysis_result *a, const analysis_result *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }

    if (a->array_size != b->array_size ||
        a->distribution != b->distribution ||
        a->order != b->order) {
        return 0;
    }

    return 1;
}

static int validate_analysis_results(const analysis_result analyses[], size_t analysis_count)
{
    if (analyses == NULL || analysis_count == 0) {
        return DECISION_INVALID_ARGUMENT;
    }

    for (size_t i = 1; i < analysis_count; i++) {
        if (!same_experiment(&analyses[0], &analyses[i])) {
            return DECISION_INVALID_ARGUMENT;
        }
    }

    return DECISION_SUCCESS;
}

/* Fonctions publiques de décision */

int select_fastest_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected)
{   
    if (selected == NULL) {
        return DECISION_INVALID_ARGUMENT;
    }

    int status = validate_analysis_results(analyses, analysis_count);
    if (status != DECISION_SUCCESS) {
        return status;
    }

    size_t best_index = 0;
    double min_time = DBL_MAX;
    int found = 0;

    for (size_t i = 0; i < analysis_count; i++) {
        if (analyses[i].average_execution_time < min_time) {
            min_time = analyses[i].average_execution_time;
            best_index = i;
            found = 1;
        }
    }

    if (!found) {
        return DECISION_NO_VALID_RESULT;
    }

    *selected = analyses[best_index].algorithm;
    return DECISION_SUCCESS;
}

int select_memory_efficient_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected)
{
    if (selected == NULL) {
        return DECISION_INVALID_ARGUMENT;
    }

    int status = validate_analysis_results(analyses, analysis_count);
    if (status != DECISION_SUCCESS) {
        return status;
    }

    size_t best_index = 0;
    double min_memory = DBL_MAX;
    int found = 0;

    for (size_t i = 0; i < analysis_count; i++) {
        if (analyses[i].average_auxiliary_memory < min_memory) {
            min_memory = analyses[i].average_auxiliary_memory;
            best_index = i;
            found = 1;
        }
    }

    if (!found) {
        return DECISION_NO_VALID_RESULT;
    }

    *selected = analyses[best_index].algorithm;
    return DECISION_SUCCESS;
}

int select_best_algorithm(const analysis_result analyses[], size_t analysis_count, DecisionCriterion criterion, SortAlgorithm *selected)
{
    if (selected == NULL) {
        return DECISION_INVALID_ARGUMENT;
    }

    int status = validate_analysis_results(analyses, analysis_count);
    if (status != DECISION_SUCCESS) {
        return status;
    }

    switch (criterion) {
        case FASTEST:
            return select_fastest_algorithm(analyses, analysis_count, selected);

        case MEMORY_EFFICIENT:
            return select_memory_efficient_algorithm(analyses, analysis_count, selected);

        case BEST_OVERALL: {
            size_t best_index = 0;
            double best_score = DBL_MAX;
            int found = 0;

            for (size_t i = 0; i < analysis_count; i++) {
                double score = (analyses[i].average_execution_time * 0.7) + (analyses[i].average_auxiliary_memory * 0.3);
                if (score < best_score) {
                    best_score = score;
                    best_index = i;
                    found = 1;
                }
            }

            if (!found) {
                return DECISION_NO_VALID_RESULT;
            }

            *selected = analyses[best_index].algorithm;
            return DECISION_SUCCESS;
        }

        default:
            return DECISION_INVALID_ARGUMENT;
    }
}