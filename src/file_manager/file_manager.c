#include "file_manager.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Fonctions internes */

static const char *algorithm_to_string(SortAlgorithm algorithm)
{
    /* Corps de la fonction. */
}

static const char *distribution_to_string(DataDistribution distribution)
{
    /* Corps de la fonction. */
}

static const char *order_to_string(SortOrder order)
{
    /* Corps de la fonction. */   
}

/* Fonctions publiques */


int export_data(const char *filename, const table *t)
{
    /* Corps de la fonction. */
}

int export_benchmark_results(const char *filename, const benchmark_result results[], size_t result_count)
{
   /* Corps de la fonction. */
}

int export_analysis_results(const char *filename, const analysis_result analyses[], size_t analysis_count)
{
   /* Corps de la fonction. */
}