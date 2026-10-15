#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include "data.h"
#include "sorting.h"
#include "benchmark.h"
#include "analysis.h"
#include "decision.h"
#include "file_manager.h"
#include "report.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define RUN_TEST(test_func) \
    do { \
        printf("Running %s...", #test_func); \
        if (test_func()) { \
            printf(" [PASS]\n"); \
            tests_passed++; \
        } else { \
            printf(" [FAIL]\n"); \
            tests_failed++; \
        } \
    } while (0)

static int is_table_sorted(const table *t, SortOrder order)
{
    if (t->nombre_elements < 2) return 1;
    for (size_t i = 0; i < t->nombre_elements - 1; i++) {
        if (order == ASCENDING && t->elements[i] > t->elements[i+1]) return 0;
        if (order == DESCENDING && t->elements[i] < t->elements[i+1]) return 0;
    }
    return 1;
}

static int test_sorting_algorithms(void)
{
    SortOrder orders[] = {ASCENDING, DESCENDING};
    typedef void (*sort_fn)(table *, SortOrder, sort_stats *);
    sort_fn algos[] = {bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort, heap_sort};

    for (int o = 0; o < 2; o++) {
        SortOrder order = orders[o];
        for (int a = 0; a < 6; a++) {
            /* 1. Empty table */
            table t_empty = {NULL, 0};
            sort_stats stats;
            algos[a](&t_empty, order, &stats);

            /* 2. Single element */
            table t_single;
            init_table(&t_single, 1);
            t_single.elements[0] = 42.0;
            algos[a](&t_single, order, &stats);
            if (!is_table_sorted(&t_single, order)) {
                free_table(&t_single);
                return 0;
            }
            free_table(&t_single);

            /* 3. Duplicates and random data */
            table t_rand;
            generate_table(&t_rand, 50, DUPLICATES);
            algos[a](&t_rand, order, &stats);
            if (!is_table_sorted(&t_rand, order)) {
                free_table(&t_rand);
                return 0;
            }
            free_table(&t_rand);
        }
    }
    return 1;
}

static int test_distributions(void)
{
    DataDistribution dists[] = {RANDOM, SORTED, REVERSE_SORTED, NEARLY_SORTED, DUPLICATES};
    for (int d = 0; d < 5; d++) {
        table t;
        if (generate_table(&t, 100, dists[d]) != 0) return 0;
        if (t.nombre_elements != 100 || t.elements == NULL) {
            free_table(&t);
            return 0;
        }
        free_table(&t);
    }
    return 1;
}

static int test_multi_sampling_and_analysis(void)
{
    table t;
    generate_table(&t, 30, RANDOM);

    size_t sample_count = 5;
    benchmark_result results[6 * 5];
    int status = benchmark_multi_samples(&t, ASCENDING, RANDOM, sample_count, results, 6 * 5);
    free_table(&t);

    if (status != BENCHMARK_SUCCESS) return 0;

    analysis_result analyses[6];
    status = analyze_all_algorithms(results, 6 * 5, 30, RANDOM, ASCENDING, analyses, 6);
    if (status != ANALYSIS_SUCCESS) return 0;

    for (int i = 0; i < 6; i++) {
        if (analyses[i].sample_count != sample_count) return 0;
        if (analyses[i].average_execution_time < 0) return 0;
        if (analyses[i].stddev_execution_time < 0) return 0;
    }

    SortAlgorithm best_fastest;
    if (select_fastest_algorithm(analyses, 6, &best_fastest) != DECISION_SUCCESS) return 0;

    return 1;
}

static int test_file_exports(void)
{
    table t;
    generate_table(&t, 20, RANDOM);

    benchmark_result results[6];
    benchmark_all(&t, ASCENDING, RANDOM, results, 6);

    analysis_result analyses[6];
    analyze_all_algorithms(results, 6, 20, RANDOM, ASCENDING, analyses, 6);

    (void)mkdir("output", 0755);

    if (export_data("output/test_data.csv", &t) != FILE_MANAGER_SUCCESS) {
        free_table(&t);
        return 0;
    }
    if (export_benchmark_results("output/test_bench.csv", results, 6) != FILE_MANAGER_SUCCESS) {
        free_table(&t);
        return 0;
    }
    if (export_analysis_results("output/test_analysis.csv", analyses, 6) != FILE_MANAGER_SUCCESS) {
        free_table(&t);
        return 0;
    }

    free_table(&t);
    return 1;
}

static int test_gnuplot_generation(void)
{
    table t;
    generate_table(&t, 25, RANDOM);

    benchmark_result results[6];
    benchmark_all(&t, ASCENDING, RANDOM, results, 6);

    analysis_result analyses[6];
    analyze_all_algorithms(results, 6, 25, RANDOM, ASCENDING, analyses, 6);

    int s1 = generate_execution_time_plot(analyses, 6, "output/plots/test_exec.png");
    int s2 = generate_comparisons_plot(analyses, 6, "output/plots/test_comp.png");
    int s3 = generate_auxiliary_memory_plot(analyses, 6, "output/plots/test_mem.png");

    free_table(&t);

    if (s1 != REPORT_SUCCESS && s1 != REPORT_GNUPLOT_NOT_FOUND) return 0;
    if (s2 != REPORT_SUCCESS && s2 != REPORT_GNUPLOT_NOT_FOUND) return 0;
    if (s3 != REPORT_SUCCESS && s3 != REPORT_GNUPLOT_NOT_FOUND) return 0;

    return 1;
}

int main(void)
{
    printf("========================================\n");
    printf("     RUNNING C BENCHMARK UNIT TESTS     \n");
    printf("========================================\n");

    RUN_TEST(test_sorting_algorithms);
    RUN_TEST(test_distributions);
    RUN_TEST(test_multi_sampling_and_analysis);
    RUN_TEST(test_file_exports);
    RUN_TEST(test_gnuplot_generation);

    printf("========================================\n");
    printf("RESULTS: %d Passed, %d Failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
