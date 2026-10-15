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

#define NB_ALGORITHMES 6
#define MAX_SAMPLES 100
#define MAX_RESULTS (NB_ALGORITHMES * MAX_SAMPLES)
#define TAILLE_CHEMIN 256

#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

typedef struct
{
    table donnees;
    int donnees_initialisees;

    benchmark_result resultats[MAX_RESULTS];
    size_t nombre_resultats;
    int benchmark_disponible;

    analysis_result analyses[NB_ALGORITHMES];
    size_t nombre_analyses;
    int analyse_disponible;

    DataDistribution distribution;
    SortOrder ordre;
} Application;

static Application app = {0};

/* Fonctions internes */
static void afficher_separateur(void);
static void pause_utilisateur(void);
static int lire_entier(const char *message, int min, int max);
static int lire_taille(const char *message, size_t *valeur);
static int lire_chemin(const char *message, char chemin[], size_t capacite);

static void afficher_menu_donnees(void);
static void afficher_menu_benchmark(void);
static void afficher_menu_analyse(void);
static void afficher_menu_decision(void);
static void afficher_menu_export(void);
static void afficher_menu_rapports(void);

static void afficher_donnees(void);
static void afficher_resultats(void);
static void afficher_analyses(void);

static const char *nom_distribution(DataDistribution distribution);
static const char *nom_ordre(SortOrder ordre);
static const char *nom_algorithme(SortAlgorithm algorithme);
static void afficher_erreur(const char *operation, int statut);

/* Point d'entree */
int main(void)
{
    lancer_application();
    return EXIT_SUCCESS;
}

void lancer_application(void)
{
    int choix;

    do
    {
        afficher_banniere();
        afficher_menu_principal();

        choix = lire_entier("Votre choix : ", 0, 6);

        switch (choix)
        {
            case 1:
                afficher_menu_donnees();
                break;

            case 2:
                afficher_menu_benchmark();
                break;

            case 3:
                afficher_menu_analyse();
                break;

            case 4:
                afficher_menu_decision();
                break;

            case 5:
                afficher_menu_export();
                break;

            case 6:
                afficher_menu_rapports();
                break;

            case 0:
                printf("\n" GREEN
                       "Merci d'avoir utilise l'application !\n"
                       RESET);
                break;
        }

    } while (choix != 0);

    if (app.donnees_initialisees)
        free_table(&app.donnees);
}

void afficher_banniere(void)
{
    printf("\n");
    afficher_separateur();
    printf(CYAN BOLD
           "              TRI & PERFORMANCE\n"
           RESET);
    afficher_separateur();
    printf("  Bienvenue dans l'application de benchmark de tri.\n");
    printf("  Generez, mesurez, analysez et comparez vos algorithmes.\n");
    afficher_separateur();

    if (app.donnees_initialisees)
    {
        printf("  Donnees : %zu elements (%s)\n",
               app.donnees.nombre_elements, nom_distribution(app.distribution));
    }
    else
    {
        printf("  Donnees : non generees\n");
    }
    printf("  Benchmarks : %s\n",
           app.benchmark_disponible ? "disponibles" : "non executes");
    printf("  Analyses : %s\n",
           app.analyse_disponible ? "disponibles" : "non effectuees");
    afficher_separateur();
}

void afficher_menu_principal(void)
{
    printf("\n" BOLD "  MENU PRINCIPAL\n" RESET);
    afficher_separateur();

    printf("  [1] Gestion des donnees\n");
    printf("  [2] Execution des benchmarks\n");
    printf("  [3] Analyse des performances\n");
    printf("  [4] Aide a la decision\n");
    printf("  [5] Exportation des resultats\n");
    printf("  [6] Generation des graphiques\n");

    afficher_separateur();
    printf("  [0] Quitter\n");
    afficher_separateur();
}

/* Gestion des donnees */
static void afficher_menu_donnees(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  GESTION DES DONNEES\n" RESET);
        afficher_separateur();
        printf("  [1] Generer un tableau\n");
        printf("  [2] Afficher le tableau\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 2);

        switch (choix)
        {
            case 1:
            {
                size_t taille;

                if (!lire_taille("Taille du tableau : ", &taille))
                    break;

                int distribution = lire_entier(
                    "Distribution (0 aleatoire, 1 triee, "
                    "2 inversee, 3 presque triee, 4 doublons) : ",
                    0, 4);

                if (app.donnees_initialisees)
                {
                    free_table(&app.donnees);
                    app.donnees_initialisees = 0;
                }

                app.donnees.nombre_elements = 0;
                app.donnees.elements = NULL;

                app.distribution = (DataDistribution)distribution;

                int statut = generate_table(
                    &app.donnees,
                    taille,
                    app.distribution
                );

                if (statut == 0)
                {
                    app.donnees_initialisees = 1;
                    app.benchmark_disponible = 0;
                    app.analyse_disponible = 0;
                    app.nombre_resultats = 0;
                    app.nombre_analyses = 0;

                    printf(GREEN
                           "Tableau genere avec succes (%zu elements).\n"
                           RESET, taille);
                }
                else
                {
                    afficher_erreur("Generation du tableau", statut);
                }

                break;
            }

            case 2:
                afficher_donnees();
                break;

            case 0:
                break;
        }

        if (choix != 0)
            pause_utilisateur();

    } while (choix != 0);
}

/* Benchmark des six algorithmes */
static void afficher_menu_benchmark(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  EXECUTION DES BENCHMARKS\n" RESET);
        afficher_separateur();
        printf("  [1] Benchmark des six algorithmes\n");
        printf("  [2] Afficher les derniers resultats\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 2);

        switch (choix)
        {
            case 1:
            {
                if (!app.donnees_initialisees)
                {
                    printf(YELLOW
                           "Generez d'abord un tableau.\n"
                           RESET);
                    break;
                }

                int ordre = lire_entier(
                    "Ordre (0 croissant, 1 decroissant) : ", 0, 1);

                int samples = lire_entier(
                    "Nombre d'echantillons (1 a 100, defaut 10) : ", 1, MAX_SAMPLES);

                app.ordre = (SortOrder)ordre;

                int statut = benchmark_multi_samples(
                    &app.donnees,
                    app.ordre,
                    app.distribution,
                    (size_t)samples,
                    app.resultats,
                    MAX_RESULTS
                );

                if (statut == BENCHMARK_SUCCESS)
                {
                    app.nombre_resultats = NB_ALGORITHMES * samples;
                    app.benchmark_disponible = 1;
                    app.analyse_disponible = 0;
                    app.nombre_analyses = 0;

                    printf(GREEN
                           "Benchmark termine avec succes (%d echantillon(s)).\n"
                           RESET, samples);

                    afficher_resultats();
                }
                else
                {
                    app.benchmark_disponible = 0;
                    app.analyse_disponible = 0;
                    app.nombre_resultats = 0;
                    app.nombre_analyses = 0;
                    afficher_erreur("Benchmark", statut);
                }

                break;
            }

            case 2:
                afficher_resultats();
                break;

            case 0:
                break;
        }

        if (choix != 0)
            pause_utilisateur();

    } while (choix != 0);
}

/* Analyse des performances */
static void afficher_menu_analyse(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  ANALYSE DES PERFORMANCES\n" RESET);
        afficher_separateur();
        printf("  [1] Analyser les six algorithmes\n");
        printf("  [2] Afficher les analyses\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 2);

        switch (choix)
        {
            case 1:
            {
                if (!app.benchmark_disponible)
                {
                    printf(YELLOW
                           "Executez d'abord les benchmarks.\n"
                           RESET);
                    break;
                }

                int statut = analyze_all_algorithms(
                    app.resultats,
                    app.nombre_resultats,
                    app.donnees.nombre_elements,
                    app.distribution,
                    app.ordre,
                    app.analyses,
                    NB_ALGORITHMES
                );

                if (statut == ANALYSIS_SUCCESS)
                {
                    app.nombre_analyses = NB_ALGORITHMES;
                    app.analyse_disponible = 1;

                    printf(GREEN "Analyse terminee.\n" RESET);
                    afficher_analyses();
                }
                else
                {
                    app.analyse_disponible = 0;
                    app.nombre_analyses = 0;
                    afficher_erreur("Analyse", statut);
                }

                break;
            }

            case 2:
                afficher_analyses();
                break;

            case 0:
                break;
        }

        if (choix != 0)
            pause_utilisateur();

    } while (choix != 0);
}

/* Aide a la decision */
static void afficher_menu_decision(void)
{
    int choix;

    if (!app.analyse_disponible)
    {
        printf(YELLOW
               "\nEffectuez d'abord une analyse des performances.\n"
               RESET);
        pause_utilisateur();
        return;
    }

    do
    {
        printf("\n" CYAN BOLD "  AIDE A LA DECISION\n" RESET);
        afficher_separateur();
        printf("  [1] Algorithme le plus rapide\n");
        printf("  [2] Algorithme le moins gourmand en memoire\n");
        printf("  [3] Selection selon un critere global\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 3);

        if (choix == 0)
            break;

        SortAlgorithm selection;
        int statut;

        if (choix == 1)
        {
            statut = select_fastest_algorithm(
                app.analyses, app.nombre_analyses, &selection);
        }
        else if (choix == 2)
        {
            statut = select_memory_efficient_algorithm(
                app.analyses, app.nombre_analyses, &selection);
        }
        else
        {
            int critere = lire_entier(
                "Critere (0 vitesse, 1 memoire, 2 meilleur global) : ",
                0, 2);

            statut = select_best_algorithm(
                app.analyses,
                app.nombre_analyses,
                (DecisionCriterion)critere,
                &selection
            );
        }

        if (statut == DECISION_SUCCESS)
        {
            printf(GREEN "\nAlgorithme selectionne : %s\n" RESET,
                   nom_algorithme(selection));
        }
        else
        {
            afficher_erreur("Selection de l'algorithme", statut);
        }

        pause_utilisateur();

    } while (choix != 0);
}

/* Exportation */
static void afficher_menu_export(void)
{
    int choix;

    do
    {
        printf("\n" CYAN BOLD "  EXPORTATION DES RESULTATS\n" RESET);
        afficher_separateur();
        printf("  [1] Exporter le tableau\n");
        printf("  [2] Exporter les benchmarks\n");
        printf("  [3] Exporter les analyses\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 3);

        if (choix == 0)
            break;

        char chemin[TAILLE_CHEMIN];

        if (!lire_chemin("Nom du fichier de sortie : ",
                         chemin, sizeof chemin))
        {
            printf(RED "Nom de fichier invalide.\n" RESET);
            pause_utilisateur();
            continue;
        }

        int statut;

        switch (choix)
        {
            case 1:
                if (!app.donnees_initialisees)
                {
                    printf(YELLOW "Aucune donnee a exporter.\n" RESET);
                    pause_utilisateur();
                    continue;
                }

                statut = export_data(chemin, &app.donnees);
                break;

            case 2:
                if (!app.benchmark_disponible)
                {
                    printf(YELLOW "Aucun benchmark a exporter.\n" RESET);
                    pause_utilisateur();
                    continue;
                }

                statut = export_benchmark_results(
                    chemin, app.resultats, app.nombre_resultats);
                break;

            default:
                if (!app.analyse_disponible)
                {
                    printf(YELLOW "Aucune analyse a exporter.\n" RESET);
                    pause_utilisateur();
                    continue;
                }

                statut = export_analysis_results(
                    chemin, app.analyses, app.nombre_analyses);
                break;
        }

        if (statut == FILE_MANAGER_SUCCESS)
            printf(GREEN "Export termine : %s\n" RESET, chemin);
        else
            afficher_erreur("Exportation", statut);

        pause_utilisateur();

    } while (choix != 0);
}

/* Generation des graphiques */
static void afficher_menu_rapports(void)
{
    int choix;

    if (!app.analyse_disponible)
    {
        printf(YELLOW
               "\nEffectuez d'abord une analyse des performances.\n"
               RESET);
        pause_utilisateur();
        return;
    }

    do
    {
        printf("\n" CYAN BOLD "  GENERATION DES GRAPHIQUES\n" RESET);
        afficher_separateur();
        printf("  [1] Temps d'execution\n");
        printf("  [2] Nombre de comparaisons\n");
        printf("  [3] Memoire auxiliaire\n");
        printf("  [0] Retour\n");
        afficher_separateur();

        choix = lire_entier("Votre choix : ", 0, 3);

        if (choix == 0)
            break;

        char chemin[TAILLE_CHEMIN];

        if (!lire_chemin("Nom du fichier de sortie : ",
                         chemin, sizeof chemin))
        {
            printf(RED "Nom de fichier invalide.\n" RESET);
            pause_utilisateur();
            continue;
        }

        int statut;

        switch (choix)
        {
            case 1:
                statut = generate_execution_time_plot(
                    app.analyses, app.nombre_analyses, chemin);
                break;

            case 2:
                statut = generate_comparisons_plot(
                    app.analyses, app.nombre_analyses, chemin);
                break;

            default:
                statut = generate_auxiliary_memory_plot(
                    app.analyses, app.nombre_analyses, chemin);
                break;
        }

        if (statut == REPORT_SUCCESS)
            printf(GREEN "Graphique genere : %s\n" RESET, chemin);
        else
            afficher_erreur("Generation du graphique", statut);

        pause_utilisateur();

    } while (choix != 0);
}

/* Affichage des donnees */
static void afficher_donnees(void)
{
    if (!app.donnees_initialisees)
    {
        printf(YELLOW "Aucun tableau disponible.\n" RESET);
        return;
    }

    printf("\nTableau (%zu elements) :\n",
           app.donnees.nombre_elements);

    for (size_t i = 0; i < app.donnees.nombre_elements; i++)
    {
        printf("%10.2f", app.donnees.elements[i]);

        if ((i + 1) % 6 == 0)
            putchar('\n');
    }

    putchar('\n');
}

/* Affichage des resultats du benchmark */
static void afficher_resultats(void)
{
    if (!app.benchmark_disponible)
    {
        printf(YELLOW "Aucun resultat de benchmark disponible.\n"
                      RESET);
        return;
    }

    printf("\nOrdre : %s\n", nom_ordre(app.ordre));
    printf("%-12s %14s %14s %14s\n",
           "Algorithme", "Temps (s)", "Comparaisons", "Swaps");

    afficher_separateur();

    for (size_t i = 0; i < app.nombre_resultats; i++)
    {
        const benchmark_result *r = &app.resultats[i];

        printf("%-12s %14.6f %14d %14d\n",
               nom_algorithme(r->algorithm),
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

    printf("\n%-12s %12s %14s %14s %14s %14s\n",
           "Algorithme", "Echantillons", "Temps moyen", "Ecart-type (s)",
           "Comp. moyennes", "Swaps moyens");

    afficher_separateur();

    for (size_t i = 0; i < app.nombre_analyses; i++)
    {
        const analysis_result *a = &app.analyses[i];

        printf("%-12s %12zu %14.6f %14.6f %14.2f %14.2f\n",
               nom_algorithme(a->algorithm),
               a->sample_count,
               a->average_execution_time,
               a->stddev_execution_time,
               a->average_comparisons,
               a->average_swaps);
    }
}

/* Noms lisibles des enums */
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
        case REVERSE_SORTED: return "Inverse";
        case NEARLY_SORTED:  return "Presque triee";
        case DUPLICATES:     return "Doublons";
        default:             return "Inconnue";
    }
}

static const char *nom_ordre(SortOrder ordre)
{
    return ordre == ASCENDING ? "Croissant" : "Decroissant";
}

/* Lecture robuste d'un entier */
static int lire_entier(const char *message, int min, int max)
{
    char ligne[128];

    for (;;)
    {
        printf("%s", message);

        if (fgets(ligne, sizeof ligne, stdin) == NULL)
            exit(EXIT_SUCCESS);

        if (strchr(ligne, '\n') == NULL && !feof(stdin))
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf(RED "Saisie trop longue.\n" RESET);
            continue;
        }

        errno = 0;
        char *fin;
        long valeur = strtol(ligne, &fin, 10);

        while (*fin == ' ' || *fin == '\t' ||
               *fin == '\r' || *fin == '\n')
            fin++;

        if (fin == ligne || *fin != '\0' || errno == ERANGE ||
            valeur < min || valeur > max)
        {
            printf(RED "Entrez un entier entre %d et %d.\n" RESET,
                   min, max);
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

        if (fgets(ligne, sizeof ligne, stdin) == NULL)
            return 0;

        errno = 0;
        char *fin;
        unsigned long long n = strtoull(ligne, &fin, 10);

        while (*fin == ' ' || *fin == '\t' ||
               *fin == '\r' || *fin == '\n')
            fin++;

        if (fin == ligne || *fin != '\0' || errno == ERANGE ||
            ligne[0] == '-' || n == 0 || n > (size_t)-1)
        {
            printf(RED "Entrez une taille strictement positive.\n"
                       RESET);
            continue;
        }

        *valeur = (size_t)n;
        return 1;
    }
}

static int lire_chemin(const char *message, char chemin[], size_t capacite)
{
    printf("%s", message);

    if (fgets(chemin, capacite, stdin) == NULL)
        return 0;

    char *newline = strchr(chemin, '\n');

    if (newline != NULL)
        *newline = '\0';
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
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
    for (int i = 0; i < 66; i++)
        putchar('=');

    putchar('\n');
}

static void afficher_erreur(const char *operation, int statut)
{
    printf(RED "%s : echec (code %d).\n" RESET, operation, statut);
}