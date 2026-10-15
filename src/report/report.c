#define _POSIX_C_SOURCE 200809L
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
    if (ret == 0) return 1;
    ret = system("which gnuplot > /dev/null 2>&1");
    if (ret == 0) return 1;
    return 0;
}

static int generate_plot_full(const analysis_result analyses[], size_t analysis_count, const char *filename, const char *metric_type, const char *title, const char *ylabel)
{
    if (analyses == NULL || analysis_count == 0 || filename == NULL)
        return REPORT_INVALID_ARGUMENT;

    char png_filename[512];
    if (strlen(filename) >= 4 && strcmp(filename + strlen(filename) - 4, ".png") == 0)
    {
        snprintf(png_filename, sizeof(png_filename), "%s", filename);
    }
    else
    {
        snprintf(png_filename, sizeof(png_filename), "%s.png", filename);
    }

    ensure_dir_exists(png_filename);

    char dat_filename[512];
    char gp_filename[512];

    snprintf(dat_filename, sizeof(dat_filename), "%s.dat", png_filename);
    snprintf(gp_filename, sizeof(gp_filename), "%s.gp", png_filename);

    /* Count distinct sizes */
    size_t distinct_sizes = 0;
    size_t sizes[64];
    for (size_t i = 0; i < analysis_count; i++)
    {
        int found = 0;
        for (size_t j = 0; j < distinct_sizes; j++)
        {
            if (sizes[j] == analyses[i].array_size)
            {
                found = 1;
                break;
            }
        }
        if (!found && distinct_sizes < 64)
        {
            sizes[distinct_sizes++] = analyses[i].array_size;
        }
    }

    FILE *fdat = fopen(dat_filename, "w");
    if (!fdat)
    {
        fprintf(stderr, "Erreur: Impossible de creer le fichier de donnees %s\n", dat_filename);
        return REPORT_GENERATION_ERROR;
    }

    FILE *fgp = fopen(gp_filename, "w");
    if (!fgp)
    {
        fclose(fdat);
        fprintf(stderr, "Erreur: Impossible de creer le fichier script %s\n", gp_filename);
        return REPORT_GENERATION_ERROR;
    }

    if (distinct_sizes <= 1)
    {
        /* Single size bar chart with error bars */
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

        fprintf(fgp, "set terminal png size 800,600 font \"sans,10\"\n");
        fprintf(fgp, "set output '%s'\n", png_filename);
        fprintf(fgp, "set style fill solid 0.8\n");
        fprintf(fgp, "set grid y\n");
        fprintf(fgp, "set title \"%s\"\n", title);
        fprintf(fgp, "set ylabel \"%s\"\n", ylabel);
        fprintf(fgp, "set boxwidth 0.5\n");
        fprintf(fgp, "set yrange [0:*]\n");
        fprintf(fgp, "set xtics rotate by -30\n");
        fprintf(fgp, "plot '%s' using 0:2:3:xtic(1) with boxerrorbars title 'Moyenne (+/- Ecart-type)'\n", dat_filename);
        fclose(fgp);
    }
    else
    {
        /* Multi-size line curves */
        fprintf(fdat, "# ArraySize Bubble Insertion Selection Merge Quick Heap\n");
        for (size_t s = 0; s < distinct_sizes; s++)
        {
            size_t current_size = sizes[s];
            fprintf(fdat, "%zu", current_size);

            SortAlgorithm algos[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};
            for (int a = 0; a < 6; a++)
            {
                double val = 0.0;
                for (size_t i = 0; i < analysis_count; i++)
                {
                    if (analyses[i].array_size == current_size && analyses[i].algorithm == algos[a])
                    {
                        if (strcmp(metric_type, "execution_time") == 0) val = analyses[i].average_execution_time;
                        else if (strcmp(metric_type, "comparisons") == 0) val = analyses[i].average_comparisons;
                        else if (strcmp(metric_type, "auxiliary_memory") == 0) val = analyses[i].average_auxiliary_memory;
                        break;
                    }
                }
                fprintf(fdat, " %.6f", val);
            }
            fprintf(fdat, "\n");
        }
        fclose(fdat);

        fprintf(fgp, "set terminal png size 800,600 font \"sans,10\"\n");
        fprintf(fgp, "set output '%s'\n", png_filename);
        fprintf(fgp, "set grid\n");
        fprintf(fgp, "set title \"%s en fonction de la taille\"\n", title);
        fprintf(fgp, "set xlabel \"Taille du tableau N\"\n");
        fprintf(fgp, "set ylabel \"%s\"\n", ylabel);
        fprintf(fgp, "plot '%s' using 1:2 with linespoints lw 2 title 'Bubble', \\\n", dat_filename);
        fprintf(fgp, "     '%s' using 1:3 with linespoints lw 2 title 'Insertion', \\\n", dat_filename);
        fprintf(fgp, "     '%s' using 1:4 with linespoints lw 2 title 'Selection', \\\n", dat_filename);
        fprintf(fgp, "     '%s' using 1:5 with linespoints lw 2 title 'Merge', \\\n", dat_filename);
        fprintf(fgp, "     '%s' using 1:6 with linespoints lw 2 title 'Quick', \\\n", dat_filename);
        fprintf(fgp, "     '%s' using 1:7 with linespoints lw 2 title 'Heap'\n", dat_filename);
        fclose(fgp);
    }

    if (!is_gnuplot_available())
    {
        fprintf(stderr, "Erreur: GNUPlot n'est pas installe ou n'est pas accessible dans le PATH.\n");
        return REPORT_GNUPLOT_NOT_FOUND;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "gnuplot \"%s\" 2>&1", gp_filename);
    FILE *pipe = popen(cmd, "r");
    if (pipe)
    {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            fprintf(stderr, "[GNUPlot] %s", buffer);
        }
        int pstatus = pclose(pipe);
        if (pstatus != 0)
        {
            fprintf(stderr, "Erreur: GNUPlot s'est termine avec le code d'erreur %d\n", pstatus);
            return REPORT_GENERATION_ERROR;
        }
    }

    FILE *fcheck = fopen(png_filename, "rb");
    if (!fcheck)
    {
        fprintf(stderr, "Erreur: Le fichier graphique PNG n'a pas ete cree a %s\n", png_filename);
        return REPORT_GENERATION_ERROR;
    }
    fseek(fcheck, 0, SEEK_END);
    long size = ftell(fcheck);
    fclose(fcheck);

    if (size == 0)
    {
        fprintf(stderr, "Erreur: Le fichier graphique PNG genere est vide (%s)\n", png_filename);
        return REPORT_GENERATION_ERROR;
    }

    return REPORT_SUCCESS;
}

int generate_execution_time_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "execution_time", "Temps d'execution moyen", "Temps (s)");
}

int generate_comparisons_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "comparisons", "Nombre moyen de comparaisons", "Comparaisons");
}

int generate_auxiliary_memory_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return generate_plot_full(analyses, analysis_count, filename, "auxiliary_memory", "Memoire auxiliaire moyenne utilisee", "Memoire (octets)");
}
