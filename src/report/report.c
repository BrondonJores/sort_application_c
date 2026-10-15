#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *get_algo_name(SortAlgorithm algo)
{
    switch (algo)
    {
        case BUBBLE:    return "Bubble";
        case INSERTION: return "Insertion";
        case SELECTION: return "Selection";
        case MERGE:     return "Merge";
        case QUICK:     return "Quick";
        case HEAP:      return "Heap";
        default:        return "Inconnu";
    }
}

static void ensure_dir_exists(const char *filepath)
{
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%s", filepath);
    char *last_slash = strrchr(tmp, '/');
    if (last_slash)
    {
        *last_slash = '\0';
        char cmd[550];
        snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\"", tmp);
        (void)system(cmd);
    }
}

static int is_gnuplot_available(void)
{
    int ret = system("gnuplot --version > /dev/null 2>&1");
    return (ret == 0);
}

static int generate_plot_full(const analysis_result analyses[], size_t analysis_count, const char *filename, const char *metric_type, const char *title, const char *ylabel)
{
    if (analyses == NULL || analysis_count == 0 || filename == NULL)
        return REPORT_INVALID_ARGUMENT;

    ensure_dir_exists(filename);

    char dat_filename[512];
    char gp_filename[512];

    snprintf(dat_filename, sizeof(dat_filename), "%s.dat", filename);
    snprintf(gp_filename, sizeof(gp_filename), "%s.gp", filename);

    FILE *fdat = fopen(dat_filename, "w");
    if (!fdat)
        return REPORT_GENERATION_ERROR;

    fprintf(fdat, "# Algorithm Average StdDev Min Max\n");

    for (size_t i = 0; i < analysis_count; i++)
    {
        double avg = 0.0, stddev = 0.0, min_val = 0.0, max_val = 0.0;

        if (strcmp(metric_type, "execution_time") == 0)
        {
            avg = analyses[i].average_execution_time;
            stddev = analyses[i].stddev_execution_time;
            min_val = analyses[i].min_execution_time;
            max_val = analyses[i].max_execution_time;
        }
        else if (strcmp(metric_type, "comparisons") == 0)
        {
            avg = analyses[i].average_comparisons;
            stddev = analyses[i].stddev_comparisons;
            min_val = analyses[i].average_comparisons;
            max_val = analyses[i].average_comparisons;
        }
        else if (strcmp(metric_type, "auxiliary_memory") == 0)
        {
            avg = analyses[i].average_auxiliary_memory;
            stddev = analyses[i].stddev_auxiliary_memory;
            min_val = analyses[i].average_auxiliary_memory;
            max_val = analyses[i].average_auxiliary_memory;
        }

        fprintf(fdat, "%s %.6f %.6f %.6f %.6f\n",
                get_algo_name(analyses[i].algorithm),
                avg, stddev, min_val, max_val);
    }
    fclose(fdat);

    FILE *fgp = fopen(gp_filename, "w");
    if (!fgp)
        return REPORT_GENERATION_ERROR;

    fprintf(fgp, "set terminal png size 800,600 font \"sans,10\"\n");
    fprintf(fgp, "set output '%s'\n", filename);
    fprintf(fgp, "set style fill solid 0.8\n");
    fprintf(fgp, "set grid y\n");
    fprintf(fgp, "set title \"%s\"\n", title);
    fprintf(fgp, "set ylabel \"%s\"\n", ylabel);
    fprintf(fgp, "set boxwidth 0.5\n");
    fprintf(fgp, "set yrange [0:*]\n");
    fprintf(fgp, "set xtics rotate by -30\n");
    fprintf(fgp, "plot '%s' using 0:2:3:xtic(1) with boxerrorbars title 'Moyenne (+/- Ecart-type)'\n", dat_filename);
    fclose(fgp);

    if (!is_gnuplot_available())
    {
        return REPORT_GNUPLOT_NOT_FOUND;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "gnuplot \"%s\"", gp_filename);
    int res = system(cmd);
    if (res != 0)
        return REPORT_GENERATION_ERROR;

    return REPORT_SUCCESS;
}

int generate_execution_time_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "execution_time", "Temps d'execution moyen des algorithmes de tri", "Temps (s)");
}

int generate_comparisons_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "comparisons", "Nombre moyen de comparaisons", "Comparaisons");
}

int generate_auxiliary_memory_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "auxiliary_memory", "Memoire auxiliaire moyenne utilisee", "Memoire (octets)");
}
