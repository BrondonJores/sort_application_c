#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "main.h"
#include "data.h"
#include "sorting.h"
#include "benchmark.h"
#include "analysis.h"
#include "decision.h"
#include "file_manager.h"
#include "report.h"
#include "sample_manager.h"

#define NB_ALGORITHMES 6
#define MAX_SAMPLES_IN_APP 100
#define MAX_RESULTS (NB_ALGORITHMES * MAX_SAMPLES_IN_APP * 10)
#define TAILLE_CHEMIN 256

#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

typedef struct
{
    SampleManager manager;

    benchmark_result resultats[MAX_RESULTS];
    size_t nombre_resultats;
    int benchmark_disponible;

    analysis_result analyses[NB_ALGORITHMES * 20];
    size_t nombre_analyses;
    int analyse_disponible;

    SortOrder ordre;
} Application;

static Application app = {0};

/* Fonctions internes UI */
static void afficher_separateur(void);
static void pause_utilisateur(void);
static int lire_entier(const char *message, int min, int max);
static int lire_taille(const char *message, size_t *valeur);
static int lire_chemin(const char *message, char chemin[], size_t capacite);

static void afficher_menu_gestion_echantillons(void);
static void afficher_menu_consultation(void);
static void afficher_menu_benchmark(void);
static void afficher_menu_analyse(void);
static void afficher_menu_decision(void);
static void afficher_menu_export(void);
static void afficher_menu_rapports(void);

static void afficher_resultats(void);
static void afficher_analyses(void);

static const char *nom_distribution(DataDistribution distribution);
static const char *nom_ordre(SortOrder ordre);
static const char *nom_algorithme(SortAlgorithm algorithme);
static void afficher_erreur(const char *operation, int statut);

int main(void)
{
    lancer_application();
    return EXIT_SUCCESS;
}

void lancer_application(void)
{
    sample_manager_init(&app.manager);
    int choix;

    do
    {
        afficher_banniere();
        afficher_menu_principal();

        choix = lire_entier("Votre choix : ", 0, 7);

        switch (choix)
        {
            case 1:
                afficher_menu_gestion_echantillons();
                break;

            case 2:
                afficher_menu_consultation();
                break;

            case 3:
                afficher_menu_benchmark();
                break;

            case 4:
                afficher_menu_analyse();
                break;

            case 5:
                afficher_menu_decision();
                break;

            case 6:
                afficher_menu_export();
                break;

            case 7:
                afficher_menu_rapports();
                break;

            case 0:
                printf("\n" GREEN "Merci d'avoir utilise l'application !\n" RESET);
                break;
        }

    } while (choix != 0);

    sample_manager_free(&app.manager);
}

void afficher_banniere(void)
{
    printf("\n");
    afficher_separateur();
    printf(CYAN BOLD "              TRI & PERFORMANCE — GESTIONNAIRE D'ECHANTILLONS\n" RESET);
    afficher_separateur();
    printf("  Generez, consultez, sélectionnez, mesurez et analysez vos échantillons.\n");
    afficher_separateur();

    printf("  Echantillons en memoire : %zu\n", app.manager.count);
    if (app.manager.count > 0)
    {
        for (size_t i = 0; i < app.manager.count; i++)
        {
            printf("    - %s : %zu elements (%s)\n",
                   app.manager.samples[i].id,
                   app.manager.samples[i].donnees.nombre_elements,
                   nom_distribution(app.manager.samples[i].distribution));
        }
    }

    printf("  Benchmarks : %s (%zu mesures enregistrees)\n",
           app.benchmark_disponible ? "disponibles" : "non executes", app.nombre_resultats);
    printf("  Analyses : %s (%zu analyses disponibles)\n",
           app.analyse_disponible ? "disponibles" : "non effectuees", app.nombre_analyses);
    afficher_separateur();
}

void afficher_menu_principal(void)
{
    printf("\n" BOLD "  MENU PRINCIPAL\n" RESET);
    afficher_separateur();

    printf("  [1] Gerer les echantillons\n");
    printf("  [2] Afficher et consulter les echantillons\n");
    printf("  [3] Executer des benchmarks selectifs\n");
    printf("  [4] Analyse des performances\n");
    printf("  [5] Aide a la decision\n");
    printf("  [6] Exportation des resultats\n");
    printf("  [7] Generation des graphiques GNUPlot\n");

    afficher_separateur();
    printf("  [0] Quitter\n");
    afficher_separateur();
}

/* 1. Gestion des echantillons */
static void afficher_menu_gestion_echantillons(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  GESTION DES ECHANTILLONS\n" RESET);
        afficher_separateur();
        printf("  [1] Creer un echantillon\n");
        printf("  [2] Creer plusieurs echantillons (campagne de tailles)\n");
        printf("  [3] Supprimer un echantillon par ID\n");
        printf("  [4] Filtrer les echantillons par taille / distribution\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 4);

        switch (choix)
        {
            case 1:
            {
                size_t taille;
                if (!lire_taille("Taille de l'echantillon : ", &taille))
                    break;

                int dist = lire_entier("Distribution (0 aleatoire, 1 triee, 2 inversee, 3 presque triee, 4 doublons) : ", 0, 4);
                Sample *s = sample_manager_add(&app.manager, taille, (DataDistribution)dist);

                if (s)
                {
                    printf(GREEN "Echantillon %s cree avec succes (%zu elements).\n" RESET, s->id, taille);
                }
                else
                {
                    printf(RED "Erreur de creation de l'echantillon.\n" RESET);
                }
                break;
            }

            case 2:
            {
                int nb_groupes = lire_entier("Nombre de groupes de tailles differentes : ", 1, 10);
                for (int g = 0; g < nb_groupes; g++)
                {
                    printf("\n--- Groupe %d ---\n", g + 1);
                    size_t taille;
                    if (!lire_taille("Taille des tableaux : ", &taille)) continue;
                    int count = lire_entier("Nombre de tableaux de cette taille : ", 1, 20);
                    int dist = lire_entier("Distribution (0 aleatoire, 1 triee, 2 inversee, 3 presque triee, 4 doublons) : ", 0, 4);

                    for (int k = 0; k < count; k++)
                    {
                        Sample *s = sample_manager_add(&app.manager, taille, (DataDistribution)dist);
                        if (s)
                        {
                            printf(GREEN "Cree: %s (Taille %zu, Dist: %s)\n" RESET, s->id, taille, nom_distribution((DataDistribution)dist));
                        }
                    }
                }
                break;
            }

            case 3:
            {
                char id_buf[32];
                printf("Entrez l'ID de l'echantillon a supprimer (ex: E001) : ");
                if (fgets(id_buf, sizeof(id_buf), stdin))
                {
                    char *nl = strchr(id_buf, '\n');
                    if (nl) *nl = '\0';

                    if (sample_manager_delete_by_id(&app.manager, id_buf))
                    {
                        printf(GREEN "Echantillon %s supprime avec succes.\n" RESET, id_buf);
                    }
                    else
                    {
                        printf(YELLOW "Identifiant %s introuvable.\n" RESET, id_buf);
                    }
                }
                break;
            }

            case 4:
            {
                int use_size = lire_entier("Filtrer par taille ? (1 Oui, 0 Non) : ", 0, 1);
                size_t size_val = 0;
                if (use_size) lire_taille("Valeur de la taille : ", &size_val);

                int use_dist = lire_entier("Filtrer par distribution ? (1 Oui, 0 Non) : ", 0, 1);
                int dist_val = 0;
                if (use_dist) dist_val = lire_entier("Distribution (0..4) : ", 0, 4);

                Sample *matches[MAX_SAMPLES_IN_APP];
                size_t found = sample_manager_filter(&app.manager, size_val, use_size, (DataDistribution)dist_val, use_dist, matches, MAX_SAMPLES_IN_APP);

                printf("\n%zu echantillon(s) trouve(s) :\n", found);
                for (size_t i = 0; i < found; i++)
                {
                    printf("  - %s : Taille %zu, Dist: %s\n",
                           matches[i]->id, matches[i]->donnees.nombre_elements, nom_distribution(matches[i]->distribution));
                }
                break;
            }

            case 0:
                break;
        }

        if (choix != 0) pause_utilisateur();

    } while (choix != 0);
}

/* 2. Affichage et consultation */
static void afficher_menu_consultation(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  CONSULTATION DES ECHANTILLONS\n" RESET);
        afficher_separateur();
        printf("  [1] Lister tous les echantillons\n");
        printf("  [2] Afficher le contenu detaille d'un echantillon (pagine)\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 2);

        switch (choix)
        {
            case 1:
                sample_manager_list_all(&app.manager);
                break;

            case 2:
            {
                if (app.manager.count == 0)
                {
                    printf(YELLOW "Aucun echantillon disponible.\n" RESET);
                    break;
                }

                char id_buf[32];
                printf("Entrez l'ID de l'echantillon à consulter (ex: E001) : ");
                if (fgets(id_buf, sizeof(id_buf), stdin))
                {
                    char *nl = strchr(id_buf, '\n');
                    if (nl) *nl = '\0';

                    Sample *s = sample_manager_find_by_id(&app.manager, id_buf);
                    if (!s)
                    {
                        printf(YELLOW "Echantillon %s non trouve.\n" RESET, id_buf);
                        break;
                    }

                    size_t offset = 0;
                    size_t page_size = 20;
                    int sub;
                    do
                    {
                        sample_manager_display_paginated(s, offset, page_size);
                        printf("[1] Page suivante  [2] Page precedente  [0] Quitter consultation : ");
                        sub = lire_entier("", 0, 2);
                        if (sub == 1)
                        {
                            if (offset + page_size < s->donnees.nombre_elements) offset += page_size;
                        }
                        else if (sub == 2)
                        {
                            if (offset >= page_size) offset -= page_size;
                            else offset = 0;
                        }
                    } while (sub != 0);
                }
                break;
            }

            case 0:
                break;
        }

        if (choix != 0) pause_utilisateur();

    } while (choix != 0);
}

/* 3. Executer des benchmarks selectifs */
static void afficher_menu_benchmark(void)
{
    if (app.manager.count == 0)
    {
        printf(YELLOW "\nGenerez ou creez d'abord des echantillons dans le gestionnaire.\n" RESET);
        pause_utilisateur();
        return;
    }

    int choix;
    do
    {
        printf("\n" CYAN BOLD "  BENCHMARKS SELECTIFS\n" RESET);
        afficher_separateur();
        printf("  [1] Selectionner un echantillon par ID\n");
        printf("  [2] Selectionner tous les echantillons d'une taille donnee\n");
        printf("  [3] Selectionner tous les echantillons d'une distribution donnee\n");
        printf("  [4] Selectionner TOUS les echantillons disponibles\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 4);
        if (choix == 0) break;

        Sample *selected_samples[MAX_SAMPLES_IN_APP];
        size_t selected_count = 0;

        if (choix == 1)
        {
            char id_buf[32];
            printf("Entrez l'ID de l'echantillon : ");
            if (fgets(id_buf, sizeof(id_buf), stdin))
            {
                char *nl = strchr(id_buf, '\n');
                if (nl) *nl = '\0';
                Sample *s = sample_manager_find_by_id(&app.manager, id_buf);
                if (s) selected_samples[selected_count++] = s;
                else printf(YELLOW "ID %s introuvable.\n" RESET, id_buf);
            }
        }
        else if (choix == 2)
        {
            size_t size_val;
            if (lire_taille("Taille recherchee : ", &size_val))
            {
                selected_count = sample_manager_filter(&app.manager, size_val, 1, RANDOM, 0, selected_samples, MAX_SAMPLES_IN_APP);
            }
        }
        else if (choix == 3)
        {
            int dist_val = lire_entier("Distribution (0..4) : ", 0, 4);
            selected_count = sample_manager_filter(&app.manager, 0, 0, (DataDistribution)dist_val, 1, selected_samples, MAX_SAMPLES_IN_APP);
        }
        else if (choix == 4)
        {
            for (size_t i = 0; i < app.manager.count; i++)
            {
                selected_samples[selected_count++] = &app.manager.samples[i];
            }
        }

        if (selected_count == 0)
        {
            printf(YELLOW "Avis: Aucun echantillon ne correspond a la selection.\n" RESET);
            pause_utilisateur();
            continue;
        }

        printf(GREEN "\n%zu echantillon(s) selectionne(s) pour la campagne.\n" RESET, selected_count);

        int ordre = lire_entier("Ordre de tri (0 croissant, 1 decroissant) : ", 0, 1);
        int reps = lire_entier("Nombre de repetitions par echantillon (1..100) : ", 1, 100);

        app.ordre = (SortOrder)ordre;

        SortAlgorithm all_algos[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};
        size_t total_written = 0;
        app.nombre_resultats = 0;

        for (size_t i = 0; i < selected_count; i++)
        {
            Sample *s = selected_samples[i];
            size_t written_for_sample = 0;

            int code = benchmark_sample_repetitions(
                s->id,
                &s->donnees,
                app.ordre,
                s->distribution,
                (size_t)reps,
                all_algos,
                6,
                &app.resultats[total_written],
                MAX_RESULTS - total_written,
                &written_for_sample
            );

            if (code == BENCHMARK_SUCCESS)
            {
                s->benchmark_disponible = 1;
                total_written += written_for_sample;
            }
            else
            {
                afficher_erreur("Benchmark echantillon", code);
            }
        }

        app.nombre_resultats = total_written;
        app.benchmark_disponible = (total_written > 0);
        app.analyse_disponible = 0;

        printf(GREEN "\nCampagne de benchmark terminee : %zu mesures enregistrees.\n" RESET, total_written);
        afficher_resultats();
        pause_utilisateur();

    } while (choix != 0);
}

/* 4. Analyse des performances */
static void afficher_menu_analyse(void)
{
    if (!app.benchmark_disponible)
    {
        printf(YELLOW "\nExecutez d'abord des benchmarks sur un ou plusieurs echantillons.\n" RESET);
        pause_utilisateur();
        return;
    }

    /* Compute analysis grouped by size & distribution */
    size_t total_analyses = 0;
    SortAlgorithm algos[6] = {BUBBLE, INSERTION, SELECTION, MERGE, QUICK, HEAP};

    /* Find distinct (size, distribution) pairs in results */
    for (size_t r = 0; r < app.nombre_resultats; r++)
    {
        size_t cur_size = app.resultats[r].array_size;
        DataDistribution cur_dist = app.resultats[r].distribution;

        /* Check if already analyzed */
        int already = 0;
        for (size_t a = 0; a < total_analyses; a++)
        {
            if (app.analyses[a].array_size == cur_size && app.analyses[a].distribution == cur_dist)
            {
                already = 1;
                break;
            }
        }

        if (!already && total_analyses + 6 <= NB_ALGORITHMES * 20)
        {
            for (int k = 0; k < 6; k++)
            {
                analyze_algorithm(app.resultats, app.nombre_resultats, algos[k], cur_size, cur_dist, app.ordre, &app.analyses[total_analyses + k]);
            }
            total_analyses += 6;
        }
    }

    app.nombre_analyses = total_analyses;
    app.analyse_disponible = (total_analyses > 0);

    printf(GREEN "\nAnalyse terminee avec succes.\n" RESET);
    afficher_analyses();
    pause_utilisateur();
}

/* 5. Aide a la decision */
static void afficher_menu_decision(void)
{
    if (!app.analyse_disponible)
    {
        printf(YELLOW "\nEffectuez d'abord une analyse des performances.\n" RESET);
        pause_utilisateur();
        return;
    }

    printf("\n" CYAN BOLD "  AIDE A LA DECISION PAR TAILLE ET DISTRIBUTION\n" RESET);
    afficher_separateur();

    size_t group_count = app.nombre_analyses / 6;
    for (size_t g = 0; g < group_count; g++)
    {
        analysis_result *sub = &app.analyses[g * 6];
        printf("\nGroupe Taille: %zu | Distribution: %s :\n", sub[0].array_size, nom_distribution(sub[0].distribution));

        SortAlgorithm fast, mem, best;
        select_fastest_algorithm(sub, 6, &fast);
        select_memory_efficient_algorithm(sub, 6, &mem);
        select_best_algorithm(sub, 6, BEST_OVERALL, &best);

        printf("  - Le plus rapide : " GREEN "%s\n" RESET, nom_algorithme(fast));
        printf("  - Le moins gourmand en memoire : " GREEN "%s\n" RESET, nom_algorithme(mem));
        printf("  - Meilleur compromis global : " GREEN "%s\n" RESET, nom_algorithme(best));
    }

    afficher_separateur();
    pause_utilisateur();
}

/* 6. Exportation */
static void afficher_menu_export(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  EXPORTATION DES RESULTATS\n" RESET);
        afficher_separateur();
        printf("  [1] Exporter les benchmarks (CSV)\n");
        printf("  [2] Exporter les analyses (CSV)\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 2);
        if (choix == 0) break;

        char chemin[TAILLE_CHEMIN];
        if (!lire_chemin("Nom du fichier CSV de sortie : ", chemin, sizeof chemin))
        {
            printf(RED "Nom de fichier invalide.\n" RESET);
            pause_utilisateur();
            continue;
        }

        int statut = 0;
        if (choix == 1)
        {
            if (!app.benchmark_disponible)
            {
                printf(YELLOW "Aucun resultat de benchmark a exporter.\n" RESET);
                pause_utilisateur();
                continue;
            }
            statut = export_benchmark_results(chemin, app.resultats, app.nombre_resultats);
        }
        else
        {
            if (!app.analyse_disponible)
            {
                printf(YELLOW "Aucune analyse a exporter.\n" RESET);
                pause_utilisateur();
                continue;
            }
            statut = export_analysis_results(chemin, app.analyses, app.nombre_analyses);
        }

        if (statut == FILE_MANAGER_SUCCESS)
            printf(GREEN "Export reussi dans : %s\n" RESET, chemin);
        else
            afficher_erreur("Exportation", statut);

        pause_utilisateur();

    } while (choix != 0);
}

/* 7. Generation des graphiques GNUPlot */
static void afficher_menu_rapports(void)
{
    if (!app.analyse_disponible)
    {
        printf(YELLOW "\nEffectuez d'abord une analyse des performances.\n" RESET);
        pause_utilisateur();
        return;
    }

    int choix;
    do
    {
        printf("\n" CYAN BOLD "  GENERATION DES GRAPHIQUES GNUPLOT\n" RESET);
        afficher_separateur();
        printf("  [1] Temps d'execution\n");
        printf("  [2] Nombre de comparaisons\n");
        printf("  [3] Memoire auxiliaire\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 3);
        if (choix == 0) break;

        char chemin[TAILLE_CHEMIN];
        if (!lire_chemin("Nom du fichier image (ex: output/plots/execution.png) : ", chemin, sizeof chemin))
        {
            printf(RED "Nom de fichier invalide.\n" RESET);
            pause_utilisateur();
            continue;
        }

        int statut = 0;
        if (choix == 1)
        {
            statut = generate_execution_time_plot(app.analyses, app.nombre_analyses, chemin);
        }
        else if (choix == 2)
        {
            statut = generate_comparisons_plot(app.analyses, app.nombre_analyses, chemin);
        }
        else
        {
            statut = generate_auxiliary_memory_plot(app.analyses, app.nombre_analyses, chemin);
        }

        if (statut == REPORT_SUCCESS)
        {
            printf(GREEN "Graphique genere avec succes : %s\n" RESET, chemin);
        }
        else
        {
            afficher_erreur("Generation du graphique GNUPlot", statut);
        }

        pause_utilisateur();

    } while (choix != 0);
}

/* Affichage des resultats du benchmark */
static void afficher_resultats(void)
{
    if (!app.benchmark_disponible)
    {
        printf(YELLOW "Aucun resultat de benchmark disponible.\n" RESET);
        return;
    }

    printf("\n%-8s %-12s %10s %16s %12s %14s %12s %10s\n",
           "Sample", "Algorithme", "Taille", "Distribution", "Repetition", "Temps (s)", "Comparaisons", "Swaps");
    afficher_separateur();

    for (size_t i = 0; i < app.nombre_resultats; i++)
    {
        const benchmark_result *r = &app.resultats[i];
        printf("%-8s %-12s %10zu %16s %12zu %14.6f %12d %10d\n",
               r->sample_id[0] ? r->sample_id : "E000",
               nom_algorithme(r->algorithm),
               r->array_size,
               nom_distribution(r->distribution),
               r->repetition,
               r->stats.execution_time,
               r->stats.comparaisons,
               r->stats.swaps);
    }
}

/* Affichage des analyses */
static void afficher_analyses(void)
{
    if (!app.analyse_disponible)
    {
        printf(YELLOW "Aucune analyse disponible.\n" RESET);
        return;
    }

    size_t group_count = app.nombre_analyses / 6;

    for (size_t g = 0; g < group_count; g++)
    {
        analysis_result *sub = &app.analyses[g * 6];

        printf("\n" BOLD "GROUPE D'ANALYSE %zu/%zu | Taille: %zu | Distribution: %s | Ordre: %s\n" RESET,
               g + 1, group_count,
               sub[0].array_size,
               nom_distribution(sub[0].distribution),
               nom_ordre(sub[0].order));

        afficher_separateur();
        printf("%-10s %7s %11s %11s %11s %11s %11s %11s\n",
               "Algo", "Echant.", "Temps moy", "Ecart-type", "Min (s)", "Max (s)", "Comp. moy", "Swaps moy");
        afficher_separateur();

        for (size_t i = 0; i < 6; i++)
        {
            const analysis_result *a = &sub[i];
            printf("%-10s %7zu %11.6f %11.6f %11.6f %11.6f %11.1f %11.1f\n",
                   nom_algorithme(a->algorithm),
                   a->sample_count,
                   a->average_execution_time,
                   a->stddev_execution_time,
                   a->min_execution_time,
                   a->max_execution_time,
                   a->average_comparisons,
                   a->average_swaps);
        }
        afficher_separateur();
    }
}

static const char *nom_algorithme(SortAlgorithm algorithme)
{
    switch (algorithme)
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

static const char *nom_distribution(DataDistribution distribution)
{
    switch (distribution)
    {
        case RANDOM:         return "Aleatoire";
        case SORTED:         return "Triee";
        case REVERSE_SORTED: return "Inversee";
        case NEARLY_SORTED:  return "Presque triee";
        case DUPLICATES:     return "Doublons";
        default:             return "Inconnue";
    }
}

static const char *nom_ordre(SortOrder ordre)
{
    return ordre == ASCENDING ? "Croissant" : "Decroissant";
}

static int lire_entier(const char *message, int min, int max)
{
    char ligne[128];

    for (;;)
    {
        if (message && message[0] != '\0') printf("%s", message);

        if (fgets(ligne, sizeof ligne, stdin) == NULL)
            exit(EXIT_SUCCESS);

        if (strchr(ligne, '\n') == NULL && !feof(stdin))
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) ;
            printf(RED "Saisie trop longue.\n" RESET);
            continue;
        }

        errno = 0;
        char *fin;
        long valeur = strtol(ligne, &fin, 10);

        while (*fin == ' ' || *fin == '\t' || *fin == '\r' || *fin == '\n') fin++;

        if (fin == ligne || *fin != '\0' || errno == ERANGE || valeur < min || valeur > max)
        {
            printf(RED "Entrez un entier entre %d et %d.\n" RESET, min, max);
            continue;
        }

        return (int)valeur;
    }
}

static int lire_taille(const char *message, size_t *valeur)
{
    char ligne[128];

    for (;;)
    {
        printf("%s", message);

        if (fgets(ligne, sizeof ligne, stdin) == NULL) return 0;

        errno = 0;
        char *fin;
        unsigned long long n = strtoull(ligne, &fin, 10);

        while (*fin == ' ' || *fin == '\t' || *fin == '\r' || *fin == '\n') fin++;

        if (fin == ligne || *fin != '\0' || errno == ERANGE || ligne[0] == '-' || n == 0 || n > (size_t)-1)
        {
            printf(RED "Entrez une taille strictement positive.\n" RESET);
            continue;
        }

        *valeur = (size_t)n;
        return 1;
    }
}

static int lire_chemin(const char *message, char chemin[], size_t capacite)
{
    printf("%s", message);

    if (fgets(chemin, capacite, stdin) == NULL) return 0;

    char *newline = strchr(chemin, '\n');
    if (newline != NULL) *newline = '\0';
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) ;
        return 0;
    }

    return chemin[0] != '\0';
}

static void pause_utilisateur(void)
{
    char ligne[8];
    printf("\nAppuyez sur Entree pour continuer...");
    (void)fgets(ligne, sizeof ligne, stdin);
}

static void afficher_separateur(void)
{
    for (int i = 0; i < 78; i++) putchar('=');
    putchar('\n');
}

static void afficher_erreur(const char *operation, int statut)
{
    printf(RED "%s : echec (code %d).\n" RESET, operation, statut);
}
