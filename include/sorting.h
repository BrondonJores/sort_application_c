/* include/sorting.h*/


#ifndef SORTING_H
    #define SORTING_H
    #include <stddef.h>
    #include "data.h"

    typedef enum 
    {
        ASCENDING,
        DESCENDING
    }SortOrder;

    typedef struct 
    {
        int comparaisons;
        int swaps;
        double execution_time;
        size_t auxiliary_memory; 
    }sort_stats;

/*
 * before : décide si la valeur a doit être placée AVANT la valeur b
 */
static int before(double a, double b, SortOrder order)
{
    return (order == ASCENDING) ? (a < b) : (a > b);
}

/*
 * swap_values : échange le contenu de deux cases du tableau
 */
static void swap_values(double *a, double *b)
{
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * stats_begin : remet tous les compteurs à zéro avant un tri.
 * Ainsi le benchmark n'a pas besoin d'initialiser la structure
 */
static void stats_begin(sort_stats *s)
{
    s->comparaisons = 0;
    s->swaps = 0;
    s->execution_time = 0.0;
    s->auxiliary_memory = 0;
}

/*
 * SORT_PROLOGUE : code commun placé au DÉBUT de chaque tri
 * une macro  du texte copié-collé automatiquement à chaque usage,
 * pour éviter la répitition .
 */
#define SORT_PROLOGUE                                   
    sort_stats local_stats;                             
    if (stats == NULL) stats = &local_stats;            
    stats_begin(stats);                                 
    if (t == NULL || t->elements == NULL) return;       
    clock_t start_clock = clock();

/*
 * SORT_EPILOGUE : code commun placé à la FIN de chaque tri.
 * Calcule le temps écoulé
 */
#define SORT_EPILOGUE 
    stats->execution_time = (double)(clock() - start_clock) / CLOCKS_PER_SEC;


    /* Tri a bulles */
 void bubble_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    double *a = t->elements;          /* raccourci vers les données */
    size_t n = t->nombre_elements;    /* nombre d'éléments */
 
    for (size_t i = 0; i + 1 < n; i++)
    {
        int swapped = 0;   /* indique si on a fait au moins un échange */

        for (size_t j = 0; j + 1 < n - i; j++)
        {
            stats->comparaisons++;                
            if (before(a[j + 1], a[j], order))     
            {
                swap_values(&a[j], &a[j + 1]);     
                stats->swaps++;
                swapped = 1;
            }
        }
        if (!swapped) break;   /* aucun échange : déjà trié, on arrête */
    }
    SORT_EPILOGUE
}

    /* Tri par insertion */
    void insertion_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    double *a = t->elements;
    size_t n = t->nombre_elements;
 
    for (size_t i = 1; i < n; i++)
    {
        double key = a[i];   /* l'élément à insérer */
        size_t j = i;        /* position où on va le placer */
 
        /* On recule tant que la clé doit passer avant l'élément à sa gauche. */
        while (j > 0)
        {
            stats->comparaisons++;
            if (before(key, a[j - 1], order))
            {
                a[j] = a[j - 1];   /* décale l'élément d'un cran à droite */
                stats->swaps++;    /* un décalage compte comme un swap */
                j--;
            }
            else
                break;   /* la clé est à sa bonne place */
        }
        a[j] = key;   /* on dépose la clé dans le trou créé */
    }
    SORT_EPILOGUE
}

    /* Tri par selection */
   void selection_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    double *a = t->elements;
    size_t n = t->nombre_elements;
 
    for (size_t i = 0; i + 1 < n; i++)
    {
        size_t best = i;   /* indice du "meilleur" candidat trouvé jusque-là */
 
        /* On examine tout le reste du tableau pour trouver le meilleur. */
        for (size_t j = i + 1; j < n; j++)
        {
            stats->comparaisons++;
            if (before(a[j], a[best], order))
                best = j;   /* nouveau meilleur candidat */
        }
 
        /* On échange seulement si le meilleur n'est pas déjà en place. */
        if (best != i)
        {
            swap_values(&a[i], &a[best]);
            stats->swaps++;
        }
    }
    SORT_EPILOGUE
}

    /* Tri fusion */

    static void merge_parts(double *a, double *tmp, size_t lo, size_t mid,
                        size_t hi, SortOrder order, sort_stats *stats)
{
    size_t i = lo;    /* doigt dans la moitié gauche */
    size_t j = mid;   /* doigt dans la moitié droite */
    size_t k = lo;    /* position d'écriture dans tmp */
 
    while (i < mid && j < hi)
    {
        stats->comparaisons++;
        if (before(a[j], a[i], order))
            tmp[k++] = a[j++];
        else
            tmp[k++] = a[i++];
    }

    while (i < mid) tmp[k++] = a[i++];
    while (j < hi)  tmp[k++] = a[j++];
 
    for (k = lo; k < hi; k++)
    {
        a[k] = tmp[k];
        stats->swaps++; 
    }
}
 
static void merge_rec(double *a, double *tmp, size_t lo, size_t hi,
                      SortOrder order, sort_stats *stats)
{
    if (hi - lo < 2) return;                  /* cas de base */
    size_t mid = lo + (hi - lo) / 2;          /* milieu (sans dépassement) */
    merge_rec(a, tmp, lo, mid, order, stats); /* trie la moitié gauche */
    merge_rec(a, tmp, mid, hi, order, stats); /* trie la moitié droite */
    merge_parts(a, tmp, lo, mid, hi, order, stats);
}
 

void merge_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    size_t n = t->nombre_elements;
 
    if (n >= 2)  
    {
        double *tmp = malloc(n * sizeof(double));
        if (tmp == NULL) return;  
        stats->auxiliary_memory = n * sizeof(double);   
        merge_rec(t->elements, tmp, 0, n, order, stats);
        free(tmp);           
    }
    SORT_EPILOGUE
}
 

    /* Tri rapide */
    static void quick_rec(double *a, long long lo, long long hi, SortOrder order,
                      sort_stats *stats, size_t depth, size_t *max_depth)
{
    if (depth > *max_depth) *max_depth = depth;
 
    while (lo < hi)
    {
        long long i = lo, j = hi;
        double pivot = a[lo + (hi - lo) / 2];   /* pivot = valeur du milieu */
 
        while (i <= j)
        {
            /* L'opérateur virgule "(A, B)" exécute A (compter la
               comparaison) puis évalue B (le test) : on compte donc
               chaque test, y compris celui qui fait sortir de la boucle. */
            while (stats->comparaisons++, before(a[i], pivot, order)) i++;
            while (stats->comparaisons++, before(pivot, a[j], order)) j--;
 
            if (i <= j)
            {
                if (i != j)   /* inutile d'échanger une case avec elle-même */
                {
                    swap_values(&a[i], &a[j]);
                    stats->swaps++;
                }
                i++;
                j--;
            }
        }
 
        /* Après la partition on a deux zones : [lo, j] et [i, hi].
           On récurse sur la plus petite, puis on continue la boucle
           sur la plus grande (en changeant lo ou hi). */
        if (j - lo < hi - i)
        {
            quick_rec(a, lo, j, order, stats, depth + 1, max_depth);
            lo = i;
        }
        else
        {
            quick_rec(a, i, hi, order, stats, depth + 1, max_depth);
            hi = j;
        }
    }
}
 
void quick_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    size_t n = t->nombre_elements;
    size_t max_depth = 0;
 
    if (n >= 2)
        quick_rec(t->elements, 0, (long long)n - 1, order, stats, 1, &max_depth);
 
    /* Estimation : chaque niveau de récursion stocke lo, hi, i, j et le pivot. */
    stats->auxiliary_memory = max_depth * (4 * sizeof(long long) + sizeof(double));
    SORT_EPILOGUE
}



    /* Tri par tas */
   static void sift_down(double *a, size_t start, size_t size, SortOrder order,
                      sort_stats *stats)
{
    size_t root = start;
 
    /* Tant que root a au moins un enfant gauche dans le tas */
    while (2 * root + 1 < size)
    {
        size_t child = 2 * root + 1;   /* enfant gauche */
        size_t big = root;             /* indice de celui qui doit remonter */
 
        /* Compare le parent avec l'enfant gauche. */
        stats->comparaisons++;
        if (before(a[big], a[child], order))
            big = child;
 
        /* S'il existe un enfant droit, on le compare aussi. */
        if (child + 1 < size)
        {
            stats->comparaisons++;
            if (before(a[big], a[child + 1], order))
                big = child + 1;
        }
 
        if (big == root) return;   /* le parent est bien placé : terminé */
 
        swap_values(&a[root], &a[big]);   /* on fait remonter l'enfant */
        stats->swaps++;
        root = big;   /* on continue à descendre depuis la nouvelle position */
    }
}
 

void heap_sort(table *t, SortOrder order, sort_stats *stats)
{
    SORT_PROLOGUE
    double *a = t->elements;
    size_t n = t->nombre_elements;
 
    if (n >= 2)
    {
        /* Étape 1 : construction du tas.
           "i-- > 0" : on décrémente après le test, ce qui permet de
           descendre jusqu'à i = 0 avec un size_t (non signé) sans bug. */
        for (size_t i = n / 2; i-- > 0; )
            sift_down(a, i, n, order, stats);
 
        /* Étape 2 : on vide le tas, un élément à la fois. */
        for (size_t end = n - 1; end > 0; end--)
        {
            swap_values(&a[0], &a[end]);   /* la racine va à sa place finale */
            stats->swaps++;
            sift_down(a, 0, end, order, stats);   /* on répare le tas réduit */
        }
    }
    SORT_EPILOGUE
}

#endif
