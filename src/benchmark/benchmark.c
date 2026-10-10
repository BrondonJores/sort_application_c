#include "benchmark.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* Fonctions internes au module */
static int validate_benchmark_arguments(const table *t, SortAlgorithm algorithm, sort_stats *stats)
{
    /* Corps de la fonction. */
}

static int is_sorted_correctly(const table *original, const table *sorted, SortOrder order)
{
    /* Corps de la fonction. */
}

static int dispatch_sort(table *t, SortAlgorithm algorithm, SortOrder order, sort_stats *stats)
{
    /* Corps de la fonction. */
}

/* Fonctions benchmark publiques */
int benchmark_one(const table *t, SortAlgorithm algorithm, SortOrder order, sort_stats *stats);
{
    /* Corps de la fonction. */
}

int benchmark_all(const table *t, SortOrder order, benchmark_result results[], size_t results_capacity)
{
    /* Corps de la fonction. */
}

