#ifndef SAMPLE_MANAGER_H
#define SAMPLE_MANAGER_H

#include <stddef.h>
#include "data.h"

#define MAX_SAMPLE_ID_LEN 16

typedef struct {
    char id[MAX_SAMPLE_ID_LEN];
    table donnees;
    DataDistribution distribution;
    int benchmark_disponible;
} Sample;

typedef struct {
    Sample *samples;
    size_t count;
    size_t capacity;
    size_t next_id_counter;
} SampleManager;

void sample_manager_init(SampleManager *sm);
void sample_manager_free(SampleManager *sm);
Sample *sample_manager_add(SampleManager *sm, size_t size, DataDistribution distribution);
Sample *sample_manager_find_by_id(const SampleManager *sm, const char *id);
int sample_manager_delete_by_id(SampleManager *sm, const char *id);
void sample_manager_list_all(const SampleManager *sm);
void sample_manager_display_paginated(const Sample *s, size_t offset, size_t limit);
size_t sample_manager_filter(const SampleManager *sm, size_t size_filter, int use_size, DataDistribution dist_filter, int use_dist, Sample *results[], size_t max_results);

#endif
