#ifndef SORTING_H
    #define SORTING_H

    #include <stddef.h>
    #include "data.h"

    typedef enum {
        ASCENDING,
        DESCENDING
    } SortOrder;

    typedef struct {
        int comparaisons;
        int swaps;
        double execution_time;
        size_t auxiliary_memory;
    } sort_stats;

    /* Les six algorithmes de tri */
    void bubble_sort(table *t, SortOrder order, sort_stats *stats);
    void insertion_sort(table *t, SortOrder order, sort_stats *stats);
    void selection_sort(table *t, SortOrder order, sort_stats *stats);
    void merge_sort(table *t, SortOrder order, sort_stats *stats);
    void quick_sort(table *t, SortOrder order, sort_stats *stats);
    void heap_sort(table *t, SortOrder order, sort_stats *stats);

#endif