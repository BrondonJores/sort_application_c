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
#include "sample_manager.h"

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
            table t_empty = {NULL, 0};
            sort_stats stats;
            algos[a](&t_empty, order, &stats);

            table t_single;
            init_table(&t_single, 1);
            t_single.elements[0] = 42.0;
            algos[a](&t_single, order, &stats);
            if (!is_table_sorted(&t_single, order)) {
                free_table(&t_single);
                return 0;
            }
            free_table(&t_single);

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

static int test_sample_manager_unit(void)
{
    SampleManager sm;
    sample_manager_init(&sm);

    Sample *s1 = sample_manager_add(&sm, 100, RANDOM);
    Sample *s2 = sample_manager_add(&sm, 500, REVERSE_SORTED);
    Sample *s3 = sample_manager_add(&sm, 100, DUPLICATES);

    if (!s1 || !s2 || !s3) { sample_manager_free(&sm); return 0; }
    if (sm.count != 3) { sample_manager_free(&sm); return 0; }

    Sample *found = sample_manager_find_by_id(&sm, s1->id);
    if (!found || found->donnees.nombre_elements != 100) { sample_manager_free(&sm); return 0; }

    Sample *matches[10];
    size_t match_cnt = sample_manager_filter(&sm, 100, 1, RANDOM, 0, matches, 10);
    if (match_cnt != 2) { sample_manager_free(&sm); return 0; }

    if (!sample_manager_delete_by_id(&sm, s2->id)) { sample_manager_free(&sm); return 0; }
    if (sm.count != 2) { sample_manager_free(&sm); return 0; }

    sample_manager_free(&sm);
    return 1;
}

static int test_selective_benchmarking(void)
{
    SampleManager sm;
    sample_manager_init(&sm);

    Sample *s1 = sample_manager_add(&sm, 40, RANDOM);
    Sample *s2 = sample_manager_add(&sm, 80, SORTED);

    benchmark_result results[100];
    size_t written = 0;
    SortAlgorithm algos[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};

    int code1 = benchmark_sample_repetitions(s1->id, &s1->donnees, ASCENDING, s1->distribution, 3, algos, 6, results, 100, &written);
    if (code1 != BENCHMARK_SUCCESS || written != 18) { sample_manager_free(&sm); return 0; }

    size_t written2 = 0;
    int code2 = benchmark_sample_repetitions(s2->id, &s2->donnees, ASCENDING, s2->distribution, 2, algos, 6, &results[written], 100 - written, &written2);
    if (code2 != BENCHMARK_SUCCESS || written2 != 12) { sample_manager_free(&sm); return 0; }

    analysis_result analyses[12];
    analyze_algorithm(results, 30, BUBBLE, 40, RANDOM, ASCENDING, &analyses[0]);
    if (analyses[0].sample_count != 3) { sample_manager_free(&sm); return 0; }

    sample_manager_free(&sm);
    return 1;
}

static int test_gnuplot_multi_size_plots(void)
{
    SampleManager sm;
    sample_manager_init(&sm);

    Sample *s1 = sample_manager_add(&sm, 20, RANDOM);
    Sample *s2 = sample_manager_add(&sm, 50, RANDOM);

    benchmark_result results[100];
    size_t written1 = 0, written2 = 0;
    SortAlgorithm algos[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};

    benchmark_sample_repetitions(s1->id, &s1->donnees, ASCENDING, s1->distribution, 2, algos, 6, results, 100, &written1);
    benchmark_sample_repetitions(s2->id, &s2->donnees, ASCENDING, s2->distribution, 2, algos, 6, &results[written1], 100 - written1, &written2);

    analysis_result analyses[12];
    analyze_all_algorithms(results, written1 + written2, 20, RANDOM, ASCENDING, &analyses[0], 6);
    analyze_all_algorithms(results, written1 + written2, 50, RANDOM, ASCENDING, &analyses[6], 6);

    int status = generate_execution_time_plot(analyses, 12, "output/plots/multi_size_exec.png");

    sample_manager_free(&sm);
    return (status == REPORT_SUCCESS || status == REPORT_GNUPLOT_NOT_FOUND);
}

int main(void)
{
    printf("========================================\n");
    printf("     RUNNING C BENCHMARK UNIT TESTS     \n");
    printf("========================================\n");

    RUN_TEST(test_sorting_algorithms);
    RUN_TEST(test_sample_manager_unit);
    RUN_TEST(test_selective_benchmarking);
    RUN_TEST(test_gnuplot_multi_size_plots);

    printf("========================================\n");
    printf("RESULTS: %d Passed, %d Failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
