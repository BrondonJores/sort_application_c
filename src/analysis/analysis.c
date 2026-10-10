#include "analysis.h"

/* Fonctions internes */
static int same_experiment(
    const benchmark_result *a,
    const benchmark_result *b
);

/* Fonctions publiques d'analyses */
int analyze_algorithm(const benchmark_result results[], size_t result_count, SortAlgorithm algorithm, size_t array_size, DataDistribution distribution, SortOrder order, analysis_result *analysis)
{
     /* Corps de la fonction. */
}

int analyze_all_algorithms(const benchmark_result results[], size_t result_count, analysis_result analyses[], size_t analyses_capacity)
{
     /* Corps de la fonction. */
}

int compare_algorithms(const analysis_result analyses[], size_t analysis_count)
{
     /* Corps de la fonction. */
}