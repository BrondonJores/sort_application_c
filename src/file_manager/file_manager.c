#include "file_manager.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Fonctions internes */

static const char *algorithm_to_string(SortAlgorithm algorithm)
{
    switch (algorithm) {
        case BUBBLE:    return "BUBBLE";
        case INSERTION: return "INSERTION";
        case SELECTION: return "SELECTION";
        case MERGE:     return "MERGE";
        case QUICK:     return "QUICK";
        case HEAP:      return "HEAP";
        default:        return NULL;
    }
}

static const char *distribution_to_string(DataDistribution distribution)
{
    switch (distribution) {
        case RANDOM:        return "RANDOM";
        case SORTED:        return "SORTED";
        case REVERSE_SORTED:return "REVERSE_SORTED";
        case NEARLY_SORTED: return "NEARLY_SORTED";
        case DUPLICATES:    return "DUPLICATES";
        default:            return NULL;
    }
}

static const char *order_to_string(SortOrder order)
{
    switch (order) {
        case ASCENDING:  return "ASCENDING";
        case DESCENDING: return "DESCENDING";
        default:         return NULL;
    }
}

/* Fonctions publiques */


int export_data(const char *filename, const table *t)
{
    if (filename == NULL || filename[0] == '\0' || t == NULL ||
        (t->nombre_elements > 0 && t->elements == NULL)) {
        return FILE_MANAGER_INVALID_ARGUMENT;
    }

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return FILE_MANAGER_OPEN_ERROR;
    }

    int status = FILE_MANAGER_SUCCESS;

    for (size_t i = 0; i < t->nombre_elements; i++) {
        if (!isfinite(t->elements[i])) {
            status = FILE_MANAGER_INVALID_ARGUMENT;
            break;
        }

        if (fprintf(file, "%.17g\n", t->elements[i]) < 0) {
            status = FILE_MANAGER_WRITE_ERROR;
            break;
        }
    }

    if (fclose(file) != 0 && status == FILE_MANAGER_SUCCESS) {
        status = FILE_MANAGER_WRITE_ERROR;
    }

    return status;
}

int export_benchmark_results(const char *filename, const benchmark_result results[], size_t result_count)
{
    if (filename == NULL || filename[0] == '\0' ||
        results == NULL || result_count == 0) {
        return FILE_MANAGER_INVALID_ARGUMENT;
    }

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return FILE_MANAGER_OPEN_ERROR;
    }

    int status = FILE_MANAGER_SUCCESS;

    if (fprintf(file,
        "algorithm,array_size,distribution,order,"
        "comparisons,swaps,execution_time,auxiliary_memory\n") < 0) {
        status = FILE_MANAGER_WRITE_ERROR;
    }

    for (size_t i = 0; i < result_count &&
         status == FILE_MANAGER_SUCCESS; i++) {
        const benchmark_result *r = &results[i];

        const char *algorithm = algorithm_to_string(r->algorithm);
        const char *distribution =
            distribution_to_string(r->distribution);
        const char *order = order_to_string(r->order);

        if (algorithm == NULL || distribution == NULL ||
            order == NULL || !isfinite(r->stats.execution_time) ||
            r->stats.execution_time < 0.0) {
            status = FILE_MANAGER_INVALID_ARGUMENT;
            break;
        }

        if (fprintf(file, "%s,%zu,%s,%s,%llu,%llu,%.17g,%zu\n",
            algorithm,
            r->array_size,
            distribution,
            order,
            (unsigned long long)r->stats.comparisons,
            (unsigned long long)r->stats.swaps,
            r->stats.execution_time,
            r->stats.auxiliary_memory) < 0) {
            status = FILE_MANAGER_WRITE_ERROR;
        }
    }

    if (fclose(file) != 0 && status == FILE_MANAGER_SUCCESS) {
        status = FILE_MANAGER_WRITE_ERROR;
    }

    return status;
}

int export_analysis_results(const char *filename, const analysis_result analyses[], size_t analysis_count)
{
    if (filename == NULL || filename[0] == '\0' ||
        analyses == NULL || analysis_count == 0) {
        return FILE_MANAGER_INVALID_ARGUMENT;
    }

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return FILE_MANAGER_OPEN_ERROR;
    }

    int status = FILE_MANAGER_SUCCESS;

    if (fprintf(file,
        "algorithm,array_size,distribution,order,sample_count,"
        "average_execution_time,average_comparisons,"
        "average_swaps,average_auxiliary_memory,"
        "min_execution_time,max_execution_time\n") < 0) {
        status = FILE_MANAGER_WRITE_ERROR;
    }

    for (size_t i = 0; i < analysis_count &&
         status == FILE_MANAGER_SUCCESS; i++) {
        const analysis_result *a = &analyses[i];

        const char *algorithm = algorithm_to_string(a->algorithm);
        const char *distribution =
            distribution_to_string(a->distribution);
        const char *order = order_to_string(a->order);

        if (algorithm == NULL || distribution == NULL ||
            order == NULL || a->sample_count == 0 ||
            !isfinite(a->average_execution_time) ||
            !isfinite(a->average_comparisons) ||
            !isfinite(a->average_swaps) ||
            !isfinite(a->average_auxiliary_memory) ||
            !isfinite(a->min_execution_time) ||
            !isfinite(a->max_execution_time) ||
            a->average_execution_time < 0.0 ||
            a->average_comparisons < 0.0 ||
            a->average_swaps < 0.0 ||
            a->average_auxiliary_memory < 0.0 ||
            a->min_execution_time < 0.0 ||
            a->max_execution_time < a->min_execution_time) {
            status = FILE_MANAGER_INVALID_ARGUMENT;
            break;
        }

        if (fprintf(file,
            "%s,%zu,%s,%s,%zu,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n",
            algorithm,
            a->array_size,
            distribution,
            order,
            a->sample_count,
            a->average_execution_time,
            a->average_comparisons,
            a->average_swaps,
            a->average_auxiliary_memory,
            a->min_execution_time,
            a->max_execution_time) < 0) {
            status = FILE_MANAGER_WRITE_ERROR;
        }
    }

    if (fclose(file) != 0 && status == FILE_MANAGER_SUCCESS) {
        status = FILE_MANAGER_WRITE_ERROR;
    }

    return status;
}