#include "file_manager.h"

#include <stdio.h>
#include <stdlib.h>

/* Fonctions internes */

static const char *algorithm_to_string(SortAlgorithm algorithm)
{
    /* Corps de la fonction. */
    switch (algorithm)
    {
        case BUBBLE:
            return "Bubble Sort";
        case INSERTION:
            return "Insertion Sort";
        case SELECTION:
            return "Selection Sort";
        case MERGE:
            return "Merge Sort";
        case QUICK:
            return "Quick Sort";
        case HEAP:
            return "Heap Sort";
        default:
            return "Unknown Algorithm";
    }
}

static const char *distribution_to_string(DataDistribution distribution)
{
    /* Corps de la fonction. */
    switch (distribution)
    {
        case RANDOM:
            return "Random";
        case SORTED:
            return "Sorted";
        case REVERSE_SORTED:
            return "Reverse Sorted";
        case NEARLY_SORTED:
            return "Nearly Sorted";
        case DUPLICATES:
            return "Duplicates";
        default:
            return "Unknown Distribution";
    }
}

static const char *order_to_string(SortOrder order)
{
    /* Corps de la fonction. */   
    switch (order)
    {
        case ASCENDING:
            return "Ascending";
        case DESCENDING:
            return "Descending";
        default:
            return "Unknown Order";
    }
}

/* Fonctions publiques */


int export_data(const char *filename, const table *t)
{
    /* Corps de la fonction. */
    FILE *file = fopen(filename, "w");
    if (!file) {
        return FILE_MANAGER_OPEN_ERROR;
    }

    for (size_t i = 0; i < t->nombre_elements; i++) {
        fprintf(file, "%f\n", t->elements[i]);
    }

    fclose(file);
    return FILE_MANAGER_SUCCESS;
}

int export_benchmark_results(const char *filename, const benchmark_result results[], size_t result_count)
{
   /* Corps de la fonction. */
   FILE *file = fopen(filename, "w");
    if (!file) {
          return FILE_MANAGER_OPEN_ERROR;
     }
    
     fprintf(file, "Algorithm,Array Size,Distribution,Order,Comparisons,Swaps,Execution Time (s),Auxiliary Memory (bytes)\n");
     for (size_t i = 0; i < result_count; i++) {
          const benchmark_result *result = &results[i];
          fprintf(file, "%s,%zu,%s,%s,%d,%d,%f,%zu\n",
                 algorithm_to_string(result->algorithm),
                 result->array_size,
                 distribution_to_string(result->distribution),
                 order_to_string(result->order),
                 result->stats.comparaisons,
                 result->stats.swaps,
                 result->stats.execution_time,
                 result->stats.auxiliary_memory);
     }
    
     fclose(file);
     return FILE_MANAGER_SUCCESS;
}

int export_analysis_results(const char *filename, const analysis_result analyses[], size_t analysis_count)
{
   /* Corps de la fonction. */
    FILE *file = fopen(filename, "w");
     if (!file) {
             return FILE_MANAGER_OPEN_ERROR;
      }
     
      fprintf(file, "Algorithm,Array Size,Distribution,Order,Sample Count,Average Execution Time (s),StdDev Execution Time (s),Average Comparisons,StdDev Comparisons,Average Swaps,StdDev Swaps,Average Auxiliary Memory (bytes),StdDev Auxiliary Memory (bytes),Min Execution Time (s),Max Execution Time (s)\n");
      for (size_t i = 0; i < analysis_count; i++) {
             const analysis_result *result = &analyses[i];
             fprintf(file, "%s,%zu,%s,%s,%zu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                  algorithm_to_string(result->algorithm),
                  result->array_size,
                  distribution_to_string(result->distribution),
                  order_to_string(result->order),
                  result->sample_count,
                  result->average_execution_time,
                  result->stddev_execution_time,
                  result->average_comparisons,
                  result->stddev_comparisons,
                  result->average_swaps,
                  result->stddev_swaps,
                  result->average_auxiliary_memory,
                  result->stddev_auxiliary_memory,
                  result->min_execution_time,
                  result->max_execution_time);
      }
     
      fclose(file);
      return FILE_MANAGER_SUCCESS;
}