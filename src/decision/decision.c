#include "decision.h"

#include <stddef.h>

/* Fonctions internes */

static int validate_analysis_results(const analysis_result analyses[], size_t analysis_count)
{
    /* Corps de la fonction. */
}

static int same_experiment(const analysis_result *a, const analysis_result *b)
{
    /* Corps de la fonction. */
}

/* Public decision functions */

int select_fastest_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected)
{   
    /* Corps de la fonction. */
}

int select_memory_efficient_algorithm(const analysis_result analyses[], size_t analysis_count, SortAlgorithm *selected)
{
    /* Corps de la fonction. */
}

int select_best_algorithm(const analysis_result analyses[], size_t analysis_count, DecisionCriterion criterion, SortAlgorithm *selected)
{
    /* Corps de la fonction. */
}