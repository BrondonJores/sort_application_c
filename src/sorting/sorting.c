#include "sorting.h"

#include <stdint.h>
#include <stdlib.h>

/* ---------- Fonctions internes ---------- */

static void stats_begin(sort_stats *stats)
{
    stats->comparaisons = 0;
    stats->swaps = 0;
    stats->execution_time = 0.0;
    stats->auxiliary_memory = 0;
}

static int valid_order(SortOrder order)
{
    return order == ASCENDING || order == DESCENDING;
}

/* Retourne vrai si a doit apparaître avant b. */
static int before(double a, double b, SortOrder order)
{
    return order == ASCENDING ? a < b : a > b;
}

/* Compte uniquement les échanges entre deux cases distinctes. */
static void swap_values(double *a, double *b, sort_stats *stats)
{
    if (a == b) {
        return;
    }

    double tmp = *a;
    *a = *b;
    *b = tmp;

    stats->swaps++;
}

/*
 * Prépare les statistiques et vérifie les arguments.
 * Les fonctions publiques restent void pour respecter le header
 * actuel du projet.
 */
static int prepare_sort(table *t, SortOrder order,
                        sort_stats **stats, sort_stats *local_stats)
{
    if (*stats == NULL) {
        *stats = local_stats;
    }

    stats_begin(*stats);

    if (!valid_order(order) || t == NULL) {
        return 0;
    }

    if (t->nombre_elements > 0 && t->elements == NULL) {
        return 0;
    }

    return 1;
}

/* ======================== TRI À BULLES ======================== */

void bubble_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    double *a = t->elements;
    size_t n = t->nombre_elements;

    for (size_t i = 0; i < n && i < n - (n > 0); ++i) {
        int changed = 0;

        for (size_t j = 0; j < n - i - 1; ++j) {
            stats->comparaisons++;

            if (before(a[j + 1], a[j], order)) {
                swap_values(&a[j], &a[j + 1], stats);
                changed = 1;
            }
        }

        if (!changed) {
            break;
        }
    }
}

/* ====================== TRI PAR INSERTION ===================== */

void insertion_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    double *a = t->elements;
    size_t n = t->nombre_elements;

    for (size_t i = 1; i < n; ++i) {
        double key = a[i];
        size_t j = i;

        while (j > 0) {
            stats->comparaisons++;

            if (!before(key, a[j - 1], order)) {
                break;
            }

            /* Décalage : ce n'est pas un échange. */
            a[j] = a[j - 1];
            --j;
        }

        a[j] = key;
    }
}

/* ====================== TRI PAR SÉLECTION ===================== */

void selection_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    double *a = t->elements;
    size_t n = t->nombre_elements;

    for (size_t i = 0; i < n && i < n - (n > 0); ++i) {
        size_t best = i;

        for (size_t j = i + 1; j < n; ++j) {
            stats->comparaisons++;

            if (before(a[j], a[best], order)) {
                best = j;
            }
        }

        if (best != i) {
            swap_values(&a[i], &a[best], stats);
        }
    }
}

/* ========================== TRI FUSION ======================== */

static void merge_parts(double *a, double *tmp,
                        size_t lo, size_t mid, size_t hi,
                        SortOrder order, sort_stats *stats)
{
    size_t i = lo;
    size_t j = mid;
    size_t k = lo;

    while (i < mid && j < hi) {
        stats->comparaisons++;

        if (before(a[j], a[i], order)) {
            tmp[k++] = a[j++];
        } else {
            /* En cas d'égalité, on conserve l'ordre gauche-droite. */
            tmp[k++] = a[i++];
        }
    }

    while (i < mid) {
        tmp[k++] = a[i++];
    }

    while (j < hi) {
        tmp[k++] = a[j++];
    }

    for (k = lo; k < hi; ++k) {
        a[k] = tmp[k];
    }
}

static void merge_rec(double *a, double *tmp,
                      size_t lo, size_t hi,
                      SortOrder order, sort_stats *stats)
{
    if (hi - lo < 2) {
        return;
    }

    size_t mid = lo + (hi - lo) / 2;

    merge_rec(a, tmp, lo, mid, order, stats);
    merge_rec(a, tmp, mid, hi, order, stats);
    merge_parts(a, tmp, lo, mid, hi, order, stats);
}

void merge_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    size_t n = t->nombre_elements;

    if (n < 2) {
        return;
    }

    if (n > SIZE_MAX / sizeof(double)) {
        return;
    }

    double *tmp = malloc(n * sizeof(*tmp));

    if (tmp == NULL) {
        /*
         * L'interface actuelle est void : l'échec ne peut pas être
         * transmis explicitement. Le benchmark doit vérifier
         * le résultat du tri ou l'interface devra évoluer.
         */
        return;
    }

    stats->auxiliary_memory = n * sizeof(*tmp);

    merge_rec(t->elements, tmp, 0, n, order, stats);

    free(tmp);
}

/* ========================== TRI RAPIDE ======================== */

/*
 * Partition à trois zones :
 *   [lo, lt) : valeurs avant le pivot
 *   [lt, i)  : valeurs égales au pivot
 *   [i, gt)  : zone encore à examiner
 *   [gt, hi) : valeurs après le pivot
 *
 * Les bornes sont semi-ouvertes : hi est exclu.
 * Cela facilite la gestion des tableaux vides et des doublons.
 */
static void quick_rec(double *a, size_t lo, size_t hi,
                      SortOrder order, sort_stats *stats,
                      size_t depth, size_t *max_depth)
{
    while (hi - lo > 1) {
        if (depth > *max_depth) {
            *max_depth = depth;
        }

        double pivot = a[lo + (hi - lo) / 2];
        size_t lt = lo;
        size_t i = lo;
        size_t gt = hi;

        while (i < gt) {
            stats->comparaisons++;

            if (before(a[i], pivot, order)) {
                swap_values(&a[lt], &a[i], stats);
                ++lt;
                ++i;
            } else {
                stats->comparaisons++;

                if (before(pivot, a[i], order)) {
                    --gt;
                    swap_values(&a[i], &a[gt], stats);
                } else {
                    ++i;
                }
            }
        }

        /* Traiter récursivement la plus petite zone. */
        size_t left_size = lt - lo;
        size_t right_size = hi - gt;

        if (left_size < right_size) {
            quick_rec(a, lo, lt, order, stats,
                      depth + 1, max_depth);
            lo = gt;
        } else {
            quick_rec(a, gt, hi, order, stats,
                      depth + 1, max_depth);
            hi = lt;
        }

        ++depth;
    }

    if (depth > *max_depth) {
        *max_depth = depth;
    }
}

void quick_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    size_t n = t->nombre_elements;

    if (n < 2) {
        return;
    }

    size_t max_depth = 0;

    quick_rec(t->elements, 0, n, order, stats, 1, &max_depth);

    /*
     * Estimation indicative de la pile auxiliaire, pas mesure exacte.
     * Le compilateur peut modifier la taille réelle des cadres de pile.
     */
    stats->auxiliary_memory =
        max_depth * (2 * sizeof(size_t) + sizeof(double));
}

/* ========================== TRI PAR TAS ======================= */

static void sift_down(double *a, size_t root, size_t size,
                      SortOrder order, sort_stats *stats)
{
    /*
     * Un nœud possède un enfant gauche si root < size / 2.
     * Cette condition évite le débordement de 2 * root + 1.
     */
    while (root < size / 2) {
        size_t child = 2 * root + 1;
        size_t best = root;

        stats->comparaisons++;

        if (before(a[best], a[child], order)) {
            best = child;
        }

        if (child + 1 < size) {
            stats->comparaisons++;

            if (before(a[best], a[child + 1], order)) {
                best = child + 1;
            }
        }

        if (best == root) {
            return;
        }

        swap_values(&a[root], &a[best], stats);
        root = best;
    }
}

void heap_sort(table *t, SortOrder order, sort_stats *stats)
{
    sort_stats local_stats;

    if (!prepare_sort(t, order, &stats, &local_stats)) {
        return;
    }

    double *a = t->elements;
    size_t n = t->nombre_elements;

    if (n < 2) {
        return;
    }

    /* Construction du tas. */
    for (size_t i = n / 2; i > 0; --i) {
        sift_down(a, i - 1, n, order, stats);
    }

    /* Extraction successive de la racine. */
    for (size_t end = n - 1; end > 0; --end) {
        swap_values(&a[0], &a[end], stats);
        sift_down(a, 0, end, order, stats);
    }
}