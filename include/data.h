/* include/data.h*/
#include <stddef.h>
#ifndef DATA_H
    #define DATA_H 
    typedef struct 
    {
        double *elements;
        size_t nombre_elements;
    }table;

    typedef enum
    {
        RANDOM,
        SORTED, 
        REVERSE_SORTED,
        NEARLY_SORTED,
        DUPLICATES
    }DataDistribution;

    /* Initialisation et Libération */
    int init_table(table *t, size_t n);
    void free_table(table *t);

    /* Génération de données */
    int generate_data(table *t, DataDistribution distribution);
    int generate_table(table *t, size_t n, DataDistribution distribution);
#endif