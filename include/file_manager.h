#ifndef FILE_MANAGER_H
    #define FILE_MANAGER_H

    #include <stddef.h>
    #include "data.h"
    #include "benchmark.h"
    #include "analysis.h"

    typedef enum {
        FILE_MANAGER_SUCCESS = 0,
        FILE_MANAGER_INVALID_ARGUMENT = -1,
        FILE_MANAGER_OPEN_ERROR = -2,
        FILE_MANAGER_READ_ERROR = -3,
        FILE_MANAGER_WRITE_ERROR = -4,
        FILE_MANAGER_INVALID_FORMAT = -5,
        FILE_MANAGER_ALLOCATION_ERROR = -6
    } FileManagerStatus;
   
    int export_data(const char *filename, const table *t);
    int export_benchmark_results(const char *filename, const benchmark_result results[], size_t result_count);
    int export_analysis_results(const char *filename, const analysis_result analyses[], size_t analysis_count);

#endif