#include "analysis.h"
#include <math.h>

/* Fonctions internes */
static int same_experiment_context(const benchmark_result *a, const benchmark_result *b)
{
    if(a==NULL || b==NULL)
        return 0;

    if(a->array_size != b->array_size || a->distribution != b->distribution || a->order != b->order)
        return 0;

    return 1;
}

static int same_algorithm_experiment(const benchmark_result *a, const benchmark_result *b)
{
    if(a==NULL || b==NULL)
        return 0;

    if(a->array_size != b->array_size || a->distribution != b->distribution || a->order != b->order || a->algorithm != b->algorithm)
        return 0;

    return 1;
}

static int validate_analysis_arguments(const benchmark_result results[], size_t result_count, SortAlgorithm algorithm, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result *analysis)
{
    if (results == NULL || analysis == NULL)
        return ANALYSIS_INVALID_ARGUMENT;
    
    if (result_count == 0)
        return ANALYSIS_INVALID_ARGUMENT;
 
    if ((algorithm != BUBBLE ) && (algorithm != INSERTION) && (algorithm != SELECTION ) && (algorithm != MERGE) && (algorithm != QUICK) && (algorithm != HEAP)) 
        return ANALYSIS_INVALID_ARGUMENT; 

     if (distribution != RANDOM && distribution != SORTED && distribution != REVERSE_SORTED && distribution != NEARLY_SORTED && distribution != DUPLICATES)
        return ANALYSIS_INVALID_ARGUMENT;

    if (order != ASCENDING && order != DESCENDING)
        return ANALYSIS_INVALID_ARGUMENT;

    return ANALYSIS_SUCCESS;
       
}

/* Fonctions publiques d'analyses */
int analyze_algorithm(const benchmark_result results[], size_t result_count, SortAlgorithm algorithm, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result *analysis)
{
    int status = validate_analysis_arguments(
        results, result_count, algorithm,
        array_size, distribution, order, analysis
    );

    if (status != ANALYSIS_SUCCESS)
        return status;

    size_t sample_count = 0;

    double total_time = 0.0;
    double total_comparisons = 0.0;
    double total_swaps = 0.0;
    double total_memory = 0.0;

    double min_time = 0.0;
    double max_time = 0.0;

    for (size_t i = 0; i < result_count; i++)
    {
        if (results[i].algorithm != algorithm ||
            results[i].array_size != array_size ||
            results[i].distribution != distribution ||
            results[i].order != order)
        {
            continue;
        }

        double time = results[i].stats.execution_time;

        if (sample_count == 0)
        {
            min_time = time;
            max_time = time;
        }
        else
        {
            if (time < min_time)
                min_time = time;

            if (time > max_time)
                max_time = time;
        }

        total_time += time;
        total_comparisons += results[i].stats.comparaisons;
        total_swaps += results[i].stats.swaps;
        total_memory += results[i].stats.auxiliary_memory;

        sample_count++;
    }

    if (sample_count == 0)
        return ANALYSIS_EXPERIMENT_ERROR;

    analysis_result temp;

    temp.algorithm = algorithm;
    temp.array_size = array_size;
    temp.distribution = distribution;
    temp.order = order;
    temp.sample_count = sample_count;

    temp.average_execution_time = total_time / sample_count;
    temp.average_comparisons = total_comparisons / sample_count;
    temp.average_swaps = total_swaps / sample_count;
    temp.average_auxiliary_memory = total_memory / sample_count;

    temp.min_execution_time = min_time;
    temp.max_execution_time = max_time;

    double var_time = 0.0;
    double var_comparisons = 0.0;
    double var_swaps = 0.0;
    double var_memory = 0.0;

    for (size_t i = 0; i < result_count; i++)
    {
        if (results[i].algorithm != algorithm ||
            results[i].array_size != array_size ||
            results[i].distribution != distribution ||
            results[i].order != order)
        {
            continue;
        }

        double diff_time = results[i].stats.execution_time - temp.average_execution_time;
        double diff_comp = (double)results[i].stats.comparaisons - temp.average_comparisons;
        double diff_swap = (double)results[i].stats.swaps - temp.average_swaps;
        double diff_mem = (double)results[i].stats.auxiliary_memory - temp.average_auxiliary_memory;

        var_time += diff_time * diff_time;
        var_comparisons += diff_comp * diff_comp;
        var_swaps += diff_swap * diff_swap;
        var_memory += diff_mem * diff_mem;
    }

    temp.stddev_execution_time = sqrt(var_time / sample_count);
    temp.stddev_comparisons = sqrt(var_comparisons / sample_count);
    temp.stddev_swaps = sqrt(var_swaps / sample_count);
    temp.stddev_auxiliary_memory = sqrt(var_memory / sample_count);

    *analysis = temp;

    return ANALYSIS_SUCCESS;
}

int analyze_all_algorithms(const benchmark_result results[], size_t result_count, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result analyses[], size_t analyses_capacity)
{
    if (results == NULL || analyses == NULL || result_count == 0)
        return ANALYSIS_INVALID_ARGUMENT;

    if (analyses_capacity < 6)
        return ANALYSIS_INVALID_ARGUMENT;

    analysis_result temp[6];

    SortAlgorithm algorithms[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};

    for (size_t i = 0; i < 6; i++)
    {
        int status = analyze_algorithm(
            results,
            result_count,
            algorithms[i],
            array_size,
            distribution,
            order,
            &temp[i]
        );

        if (status != ANALYSIS_SUCCESS)
            return status;
    }

    for (size_t i = 0; i < 6; i++)
        analyses[i] = temp[i];

    return ANALYSIS_SUCCESS;
}

int compare_algorithms(const analysis_result analyses[], size_t analysis_count)
{
    if (analyses == NULL || analysis_count != 6)
        return ANALYSIS_INVALID_ARGUMENT;

    for (size_t i = 0; i < analysis_count; i++)
    {
        if (analyses[i].sample_count == 0)
            return ANALYSIS_EXPERIMENT_ERROR;

        for (size_t j = i + 1; j < analysis_count; j++)
        {
            if (analyses[i].array_size != analyses[j].array_size ||
                analyses[i].distribution != analyses[j].distribution ||
                analyses[i].order != analyses[j].order)
            {
                return ANALYSIS_EXPERIMENT_ERROR;
            }

            if (analyses[i].algorithm == analyses[j].algorithm)
                return ANALYSIS_EXPERIMENT_ERROR;
        }
    }

    return ANALYSIS_SUCCESS;
}