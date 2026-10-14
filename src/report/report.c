#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Définitions de sécurité si les constantes ne sont pas explicites dans report.h */
#ifndef REPORT_SUCCESS
#define REPORT_SUCCESS 0
#endif

#ifndef REPORT_INVALID_ARGUMENT
#define REPORT_INVALID_ARGUMENT -1
#endif

#ifndef REPORT_FILE_ERROR
#define REPORT_FILE_ERROR -2
#endif

/* Fonctions internes */

/**
 * @brief Valide les arguments transmis aux fonctions de génération de graphiques/données.
 */
static int validate_plot_arguments(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    if (analyses == NULL || analysis_count == 0 || filename == NULL) {
        return REPORT_INVALID_ARGUMENT;
    }
    return REPORT_SUCCESS;
}

/**
 * @brief Écrit les données d'analyse dans un fichier (CSV / dat) pour le traçage de la métrique spécifiée.
 */
static int write_plot_data(const analysis_result analyses[], size_t analysis_count, const char *filename, const char *metric)
{
    int validation = validate_plot_arguments(analyses, analysis_count, filename);
    if (validation != REPORT_SUCCESS) {
        return validation;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier de graphique");
        return REPORT_FILE_ERROR;
    }

    /* En-tête du fichier exporté */
    fprintf(file, "# Algorithm,ArraySize,Value\n");

    for (size_t i = 0; i < analysis_count; i++) {
        double value = 0.0;

        if (strcmp(metric, "execution_time") == 0) {
            value = analyses[i].average_execution_time;
        } else if (strcmp(metric, "comparisons") == 0) {
            value = analyses[i].average_comparisons;
        } else if (strcmp(metric, "auxiliary_memory") == 0) {
            value = analyses[i].average_auxiliary_memory;
        }

        fprintf(file, "%d,%zu,%.6f\n",
                (int)analyses[i].algorithm,
                analyses[i].array_size,
                value);
    }

    fclose(file);
    return REPORT_SUCCESS;
}

/* Fonctions publiques */

/**
 * @brief Génère le fichier de données pour le graphique du temps d'exécution.
 */
int generate_execution_time_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return write_plot_data(analyses, analysis_count, filename, "execution_time");
}

/**
 * @brief Génère le fichier de données pour le graphique du nombre de comparaisons.
 */
int generate_comparisons_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return write_plot_data(analyses, analysis_count, filename, "comparisons");
}

/**
 * @brief Génère le fichier de données pour le graphique de la mémoire auxiliaire.
 */
int generate_auxiliary_memory_plot(const analysis_result analyses[], size_t analysis_count, const char *filename)
{
    return write_plot_data(analyses, analysis_count, filename, "auxiliary_memory");
}
