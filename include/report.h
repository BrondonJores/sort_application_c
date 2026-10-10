#ifndef REPORT_H
    #define REPORT_H

    #include <stddef.h>

    #include "analysis.h"

    typedef enum {
        REPORT_SUCCESS = 0,
        REPORT_INVALID_ARGUMENT = -1,
        REPORT_GENERATION_ERROR = -2
    } ReportStatus;

    int generate_execution_time_plot(const analysis_result analyses[], size_t analysis_count, const char *filename);
    int generate_comparisons_plot(const analysis_result analyses[], size_t analysis_count, const char *filename);
    int generate_auxiliary_memory_plot(const analysis_result analyses[], size_t analysis_count, const char *filename);

#endif