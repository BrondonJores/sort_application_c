/* include/sorting.h*/


#ifndef SORTING_H
    #define SORTING_H
    #include "data.h"

    typedef enum 
    {
        ASCENDING,
        DESCENDING
    }SortOrder;

    typedef struct 
    {
        uint64_t comparaisons;
        uint64_t swaps;
        double execution_time;
        size_t auxiliary_memory; 
    }sort_stats;

    /* Tri a bulles */
    void bubble_sort(table *t, SortOrder order, sort_stats *stats);

    /* Tri par insertion */
    void insertion_sort(table *t, SortOrder order, sort_stats *stats);

    /* Tri par selection */
    void selection_sort(table *t, SortOrder order, sort_stats *stats);

    /* Tri fusion */
    void merge_sort(table *t, SortOrder order, sort_stats *stats);

    /* Tri rapide */
    void quick_sort(table *t, SortOrder order, sort_stats *stats);

    /* Tri par tas */
    void heap_sort(table *t, SortOrder order, sort_stats *stats);

#endif
