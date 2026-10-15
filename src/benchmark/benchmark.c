#include "benchmark.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Fonctions internes au module */
static int validate_benchmark_arguments(const table *t, SortAlgorithm algorithm, sort_stats *stats, SortOrder order)
{
    if (t == NULL || stats == NULL)
        return BENCHMARK_INVALID_ARGUMENT;
 
    if ((algorithm != BUBBLE ) && (algorithm != INSERTION) && (algorithm != SELECTION ) && (algorithm != MERGE) && (algorithm != QUICK) && (algorithm != HEAP)) 
        return BENCHMARK_INVALID_ARGUMENT; 

    if (t->elements == NULL && t->nombre_elements > 0)
        return BENCHMARK_INVALID_ARGUMENT;

    if (order != ASCENDING && order != DESCENDING)
        return BENCHMARK_INVALID_ARGUMENT;

    return BENCHMARK_SUCCESS;
       
}

static int validate_benchmark_all_arguments(const table *t, SortOrder order, DataDistribution distribution, benchmark_result results[], size_t results_capacity)
{
    if (t == NULL || results == NULL)
        return BENCHMARK_INVALID_ARGUMENT;

    if (t->elements == NULL && t->nombre_elements > 0)
        return BENCHMARK_INVALID_ARGUMENT;

    if (order != ASCENDING && order != DESCENDING)
        return BENCHMARK_INVALID_ARGUMENT;

    if (distribution != RANDOM && distribution != SORTED && distribution != REVERSE_SORTED && distribution != NEARLY_SORTED && distribution != DUPLICATES)
        return BENCHMARK_INVALID_ARGUMENT;

    if (results_capacity < 6)
        return BENCHMARK_INVALID_ARGUMENT;

    return BENCHMARK_SUCCESS;
}

static int compare_croissant(const void *a, const void *b) {
    double int_a = *(const double *)a;
    double int_b = *(const double *)b;

    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

static int compare_decroissant(const void *a, const void *b) {
    double int_a = *(const double *)a;
    double int_b = *(const double *)b;

    if (int_a < int_b) return 1;
    if (int_a > int_b) return -1;
    return 0;
}

static table* copy_table(const table *source)
{
    if (source == NULL)
        return NULL;

    if (source->nombre_elements>0 && source->elements==NULL)
        return NULL;

    table *temp = malloc(sizeof(table));

    if(temp == NULL)
        return NULL;

    temp->elements = NULL;
    temp->nombre_elements = source->nombre_elements;

    if (temp->nombre_elements > 0)
    {
        temp->elements = malloc(temp->nombre_elements*sizeof(double));

        if(temp->elements == NULL)
        {
            free(temp);
            return NULL;
        }
            
        memcpy(temp->elements, source->elements, temp->nombre_elements*sizeof(double));
    }

    return temp;
}


static int verify_integrity(const table *original, const table *sorted, SortOrder order)
{
    if (original->nombre_elements != sorted->nombre_elements)
        return BENCHMARK_INTEGRITY_ERROR;
    table * temp = copy_table(original);   

    if (temp == NULL)
        return  BENCHMARK_ALLOCATION_ERROR;

    if (order == ASCENDING)
        qsort(temp->elements, temp->nombre_elements, sizeof(double), compare_croissant);
    else 
        qsort(temp->elements, temp->nombre_elements, sizeof(double), compare_decroissant); 
    
        
    for(size_t j=0; j<temp->nombre_elements; j++)
    {
        if(temp->elements[j] != sorted->elements[j])
        {
            free(temp->elements);
            free(temp);
            return BENCHMARK_INTEGRITY_ERROR;
        }
    }
    free(temp->elements);
    free(temp);
    return BENCHMARK_SUCCESS;
}

static int is_sorted_correctly(const table *original, const table *sorted, SortOrder order)
{
    if (original->nombre_elements != sorted->nombre_elements)
        return BENCHMARK_SORT_ERROR;

    if (original->nombre_elements==0)
        return BENCHMARK_SUCCESS;

    for (int i=0; i<original->nombre_elements-1; i++ )
    {
        
        if (order == ASCENDING)
        {
            if (sorted->elements[i] > sorted->elements[i+1])
                return BENCHMARK_SORT_ERROR;
        }

        else 
        {
            if (sorted->elements[i] < sorted->elements[i+1])
                return BENCHMARK_SORT_ERROR;
        }
            
    }
    return BENCHMARK_SUCCESS;
}

static int dispatch_sort(table *t, SortAlgorithm algorithm, SortOrder order, sort_stats *stats)
{
    switch (algorithm) 
    {
        case BUBBLE : {
            bubble_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }
        case INSERTION : {
            insertion_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }
        case SELECTION : {
            selection_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }
        case MERGE : {
            merge_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }
        case QUICK : {
            quick_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }
        case HEAP : {
            heap_sort(t, order, stats);
            return BENCHMARK_SUCCESS;
        }

        default : {
            return BENCHMARK_INVALID_ARGUMENT;
        }
    }
}

/* Fonctions benchmark publiques */
int benchmark_one(const table *t, SortAlgorithm algorithm, SortOrder order, sort_stats *stats)
{
    if(validate_benchmark_arguments(t, algorithm, stats, order) != BENCHMARK_SUCCESS)
        return BENCHMARK_INVALID_ARGUMENT;

    stats->auxiliary_memory=0;
    stats->execution_time=0;
    stats->swaps=0;
    stats->comparaisons=0;

    table *sorted = copy_table(t);
    if (sorted==NULL)
        return BENCHMARK_ALLOCATION_ERROR;

    clock_t debut = clock();
    int code_sort = dispatch_sort(sorted, algorithm, order, stats);
    clock_t fin = clock();
    stats->execution_time = (double)(fin-debut)/CLOCKS_PER_SEC;
    if(code_sort == BENCHMARK_SUCCESS)
    {
        int correct_sort = is_sorted_correctly(t, sorted, order);
        int verified = verify_integrity(t, sorted, order);
        int code_verification = BENCHMARK_SUCCESS;

        if(correct_sort==BENCHMARK_SUCCESS && verified==BENCHMARK_SUCCESS)
        {
            free(sorted->elements);
            free(sorted);
            return BENCHMARK_SUCCESS;
        }

        if(correct_sort != BENCHMARK_SUCCESS)
            code_verification = correct_sort;
        if(verified !=BENCHMARK_SUCCESS)
            code_verification = verified;

        if(code_verification != BENCHMARK_SUCCESS)
        {
            free(sorted->elements);
            free(sorted);
            return code_verification;
        }
    }
    free(sorted->elements);
    free(sorted);
    return code_sort;  
    
}

int benchmark_all(const table *t, SortOrder order, DataDistribution distribution, benchmark_result results[], size_t results_capacity)
{
    if(validate_benchmark_all_arguments(t, order, distribution, results, results_capacity) != BENCHMARK_SUCCESS)
        return BENCHMARK_INVALID_ARGUMENT;

    SortAlgorithm algorithms[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};

    benchmark_result *copy_result = malloc(6 * sizeof(benchmark_result));
    if(copy_result == NULL)
        return  BENCHMARK_ALLOCATION_ERROR;

    for(size_t k=0; k<6; k++)
    {
        int benchmark_code = benchmark_one(t, algorithms[k], order, &copy_result[k].stats); 
        if( benchmark_code != BENCHMARK_SUCCESS){
            free(copy_result);
            return benchmark_code;
        }else{
            copy_result[k].algorithm = algorithms[k];
            copy_result[k].array_size = t->nombre_elements;
            copy_result[k].distribution = distribution;
            copy_result[k].order = order;
        }
    }
    memcpy(results, copy_result, 6*sizeof(benchmark_result));
    free(copy_result);
    return BENCHMARK_SUCCESS;

}

int benchmark_multi_samples(const table *initial_table, SortOrder order, DataDistribution distribution, size_t sample_count, benchmark_result results[], size_t results_capacity)
{
    if (initial_table == NULL || results == NULL || sample_count == 0)
        return BENCHMARK_INVALID_ARGUMENT;

    if (results_capacity < 6 * sample_count)
        return BENCHMARK_INVALID_ARGUMENT;

    if (order != ASCENDING && order != DESCENDING)
        return BENCHMARK_INVALID_ARGUMENT;

    if (distribution != RANDOM && distribution != SORTED && distribution != REVERSE_SORTED && distribution != NEARLY_SORTED && distribution != DUPLICATES)
        return BENCHMARK_INVALID_ARGUMENT;

    size_t n = initial_table->nombre_elements;
    size_t result_idx = 0;

    for (size_t s = 0; s < sample_count; s++)
    {
        table sample_table;
        if (s == 0)
        {
            table *copy = copy_table(initial_table);
            if (copy == NULL)
                return BENCHMARK_ALLOCATION_ERROR;
            sample_table = *copy;
            free(copy);
        }
        else
        {
            if (generate_table(&sample_table, n, distribution) != 0)
                return BENCHMARK_ALLOCATION_ERROR;
        }

        int status = benchmark_all(&sample_table, order, distribution, &results[result_idx], results_capacity - result_idx);
        free_table(&sample_table);

        if (status != BENCHMARK_SUCCESS)
            return status;

        result_idx += 6;
    }

    return BENCHMARK_SUCCESS;
}
