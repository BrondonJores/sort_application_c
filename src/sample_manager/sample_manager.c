#include "sample_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *distribution_to_str(DataDistribution d)
{
    switch (d)
    {
        case RANDOM:         return "Aleatoire";
        case SORTED:         return "Triee";
        case REVERSE_SORTED: return "Inversee";
        case NEARLY_SORTED:  return "Presque triee";
        case DUPLICATES:     return "Doublons";
        default:             return "Inconnue";
    }
}

void sample_manager_init(SampleManager *sm)
{
    if (!sm) return;
    sm->samples = NULL;
    sm->count = 0;
    sm->capacity = 0;
    sm->next_id_counter = 1;
}

void sample_manager_free(SampleManager *sm)
{
    if (!sm) return;
    for (size_t i = 0; i < sm->count; i++)
    {
        free_table(&sm->samples[i].donnees);
    }
    free(sm->samples);
    sm->samples = NULL;
    sm->count = 0;
    sm->capacity = 0;
}

Sample *sample_manager_add(SampleManager *sm, size_t size, DataDistribution distribution)
{
    if (!sm) return NULL;

    if (sm->count >= sm->capacity)
    {
        size_t new_cap = sm->capacity == 0 ? 10 : sm->capacity * 2;
        Sample *new_arr = realloc(sm->samples, new_cap * sizeof(Sample));
        if (!new_arr) return NULL;
        sm->samples = new_arr;
        sm->capacity = new_cap;
    }

    Sample *s = &sm->samples[sm->count];
    snprintf(s->id, sizeof(s->id), "E%03zu", sm->next_id_counter++);
    s->distribution = distribution;
    s->benchmark_disponible = 0;

    if (generate_table(&s->donnees, size, distribution) != 0)
    {
        return NULL;
    }

    sm->count++;
    return s;
}

Sample *sample_manager_find_by_id(const SampleManager *sm, const char *id)
{
    if (!sm || !id) return NULL;
    for (size_t i = 0; i < sm->count; i++)
    {
        if (strcmp(sm->samples[i].id, id) == 0)
        {
            return &sm->samples[i];
        }
    }
    return NULL;
}

int sample_manager_delete_by_id(SampleManager *sm, const char *id)
{
    if (!sm || !id) return 0;
    for (size_t i = 0; i < sm->count; i++)
    {
        if (strcmp(sm->samples[i].id, id) == 0)
        {
            free_table(&sm->samples[i].donnees);
            for (size_t j = i; j < sm->count - 1; j++)
            {
                sm->samples[j] = sm->samples[j + 1];
            }
            sm->count--;
            return 1;
        }
    }
    return 0;
}

void sample_manager_list_all(const SampleManager *sm)
{
    if (!sm || sm->count == 0)
    {
        printf("Aucun echantillon disponible.\n");
        return;
    }

    printf("%-8s %12s %18s %16s\n", "ID", "Taille", "Distribution", "Benchmark pret");
    printf("========================================================\n");
    for (size_t i = 0; i < sm->count; i++)
    {
        const Sample *s = &sm->samples[i];
        printf("%-8s %12zu %18s %16s\n",
               s->id,
               s->donnees.nombre_elements,
               distribution_to_str(s->distribution),
               s->benchmark_disponible ? "Oui" : "Non");
    }
}

void sample_manager_display_paginated(const Sample *s, size_t offset, size_t limit)
{
    if (!s) return;
    size_t total = s->donnees.nombre_elements;
    printf("\nEchantillon %s | Taille: %zu | Distribution: %s\n",
           s->id, total, distribution_to_str(s->distribution));

    if (offset >= total)
    {
        printf("Offset %zu hors limites (total %zu elements).\n", offset, total);
        return;
    }

    size_t end = offset + limit;
    if (end > total) end = total;

    printf("Affichage des indices %zu a %zu :\n", offset, end > 0 ? end - 1 : 0);
    for (size_t i = offset; i < end; i++)
    {
        printf("[%zu] %10.2f  ", i, s->donnees.elements[i]);
        if ((i - offset + 1) % 5 == 0) putchar('\n');
    }
    putchar('\n');
}

size_t sample_manager_filter(const SampleManager *sm, size_t size_filter, int use_size, DataDistribution dist_filter, int use_dist, Sample *results[], size_t max_results)
{
    if (!sm || !results) return 0;
    size_t match_count = 0;

    for (size_t i = 0; i < sm->count && match_count < max_results; i++)
    {
        Sample *s = &sm->samples[i];
        if (use_size && s->donnees.nombre_elements != size_filter) continue;
        if (use_dist && s->distribution != dist_filter) continue;

        results[match_count++] = s;
    }

    return match_count;
}
