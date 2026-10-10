#include "data.h"
#include <stdlib.h>


int init_table(table *t, size_t n)
{
    /* Corps de la fonction. */
    if (!t) {
        return -1;
    }
    t->elements = (double *)malloc(n * sizeof(double));
    if (!t->elements) {
        return -1;
    }
    t->nombre_elements = n;
    return 0;
}

void free_table(table *t)
{
    /* Corps de la fonction. */
    if (t && t->elements) {
        free(t->elements);
        t->elements = NULL;
        t->nombre_elements = 0;
    }
}

int generate_data(table *t, DataDistribution distribution)
{
    /* Corps de la fonction. */
    if (!t) {
        return -1;
    }
    switch (distribution) {
        case RANDOM:
            // Générer des données aléatoires
            for (size_t i = 0; i < t->nombre_elements; i++) {
                t->elements[i] = (double)rand() / RAND_MAX;
            }
            break;
        case SORTED:
            // Générer des données triées
            for (size_t i = 0; i < t->nombre_elements; i++) {
                t->elements[i] = (double)i / t->nombre_elements;
            }
            break;
        case REVERSE_SORTED:
            // Générer des données triées dans l'ordre inverse
            for (size_t i = 0; i < t->nombre_elements; i++) {
                t->elements[i] = (double)(t->nombre_elements - i - 1) / t->nombre_elements;
            }
            break;
        case NEARLY_SORTED:
            // Générer des données presque triées
            for (size_t i = 0; i < t->nombre_elements; i++) {
                t->elements[i] = (double)i / t->nombre_elements;
            }
            // Introduire quelques éléments désordonnés
            for (size_t i = 0; i < t->nombre_elements / 10; i++) {
                size_t index1 = rand() % t->nombre_elements;
                size_t index2 = rand() % t->nombre_elements;
                double temp = t->elements[index1];
                t->elements[index1] = t->elements[index2];
                t->elements[index2] = temp;
            }
            break;
        case DUPLICATES:
            // Générer des données avec des doublons
            for (size_t i = 0; i < t->nombre_elements; i++) {
                t->elements[i] = (double)(rand() % (t->nombre_elements / 10));
            }
            break;
    }
    return 0;
}

int generate_table(table *t, size_t n, DataDistribution distribution)
{
    /* Corps de la fonction. */
    if (!t) {
        return -1;
    }
    if (init_table(t, n) != 0) {
        return -1;
    }
    return generate_data(t, distribution);
}

    