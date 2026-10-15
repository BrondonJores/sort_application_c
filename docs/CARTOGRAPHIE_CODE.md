# Cartographie du Code Source — C Sorting Benchmark

Ce document constitue un index de référence technique du projet, indiquant les fichiers, les fonctions clés, les structures de données, les chemins d'exécution et les emplacements exacts dans le code.

---

## 1. Index Général des Fichiers et Volume de Code

| Module | Fichier En-tête (`.h`) | Fichier Source (`.c`) | Lignes Totales | Rôle Principal |
| :--- | :--- | :--- | :---: | :--- |
| **Main** | `include/main.h` (11 l.) | `main.c` (881 l.) | 892 | Interface utilisateur interactive CLI, boucles de menus, gestion d'état applicatif. |
| **SampleManager** | `include/sample_manager.h` (32 l.) | `src/sample_manager/sample_manager.c` (161 l.) | 193 | Catalogue d'échantillons en mémoire (`E001`, `E002`), pagination, filtrage, suppression. |
| **Data** | `include/data.h` (26 l.) | `src/data/data.c` (89 l.) | 115 | Allocation/libération mémoire des tableaux, génération des 5 distributions. |
| **Sorting** | `include/sorting.h` (26 l.) | `src/sorting/sorting.c` (398 l.) | 424 | Implémentation instrumentée des 6 algorithmes de tri (Bubble, Insertion, Selection, Merge, Quick, Heap). |
| **Benchmark** | `include/benchmark.h` (47 l.) | `src/benchmark/benchmark.c` (349 l.) | 396 | Exécution isolée sur copies clonées, mesure du temps `clock()`, répétitions, intégrité. |
| **Analysis** | `include/analysis.h` (38 l.) | `src/analysis/analysis.c` (216 l.) | 254 | Calcul des moyennes, min, max, écarts-types pour chaque groupe (Taille, Distribution). |
| **Decision** | `include/decision.h` (25 l.) | `src/decision/decision.c` (144 l.) | 169 | Sélection du meilleur algorithme (plus rapide, économe, compromis). |
| **FileManager** | `include/file_manager.h` (22 l.) | `src/file_manager/file_manager.c` (141 l.) | 163 | Exportation des données, des résultats individuels et des analyses statistiques au format CSV. |
| **Report** | `include/report.h` (18 l.) | `src/report/report.c` (251 l.) | 269 | Génération des fichiers `.dat`, scripts `.gp` et exécution de GNUPlot pour fabriquer les PNG. |
| **Tests** | — | `tests/test_main.c` (171 l.) | 171 | Harnais de tests unitaires automatisé (exécuté via `make test`). |

---

## 2. Emplacement des Structures de Données Essentielles

### 2.1. `table` (`include/data.h`)
```c
typedef struct {
    double *elements;
    size_t nombre_elements;
} table;
```

### 2.2. `Sample` & `SampleManager` (`include/sample_manager.h`)
```c
typedef struct {
    char id[16];                // Ex: "E001"
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
```

### 2.3. `sort_stats` (`include/sorting.h`)
```c
typedef struct {
    int comparaisons;
    int swaps;
    double execution_time;
    size_t auxiliary_memory;
} sort_stats;
```

### 2.4. `benchmark_result` (`include/benchmark.h`)
```c
typedef struct {
    char sample_id[16];
    SortAlgorithm algorithm;
    size_t array_size;
    DataDistribution distribution;
    SortOrder order;
    size_t repetition;
    sort_stats stats;
} benchmark_result;
```

### 2.5. `analysis_result` (`include/analysis.h`)
```c
typedef struct {
    SortAlgorithm algorithm;
    size_t array_size;
    DataDistribution distribution;
    SortOrder order;
    size_t sample_count;
    double average_execution_time;
    double average_comparisons;
    double average_swaps;
    double average_auxiliary_memory;
    double min_execution_time;
    double max_execution_time;
    double stddev_execution_time;
    double stddev_comparisons;
    double stddev_swaps;
    double stddev_auxiliary_memory;
} analysis_result;
```

---

## 3. Emplacement des Fonctions Clés par Fichier

### `src/sorting/sorting.c`
- `bubble_sort()` : ligne 56
- `insertion_sort()` : ligne 90
- `selection_sort()` : ligne 121
- `merge_sort()` : ligne 185
- `quick_sort()` : ligne 285
- `heap_sort()` : ligne 365

### `src/benchmark/benchmark.c`
- `benchmark_one()` : ligne 198 — Exécute un tri isolé sur une copie, mesure `clock()` et vérifie l'intégrité avec `qsort`.
- `benchmark_sample_repetitions()` : ligne 310 — Exécute $R$ répétitions d'un benchmark sans modifier l'échantillon d'origine.

### `src/analysis/analysis.c`
- `analyze_algorithm()` : ligne 39 — Agrège les résultats de benchmark et calcule les moyennes, min, max et écarts-types (`sqrt`).
- `analyze_all_algorithms()` : ligne 153 — Lance l'analyse sur les 6 algorithmes pour une taille/distribution donnée.

### `src/report/report.c`
- `generate_plot_full()` : ligne 43 — Écrit les fichiers `.dat` et `.gp`, vérifie GNUPlot, pclose, et contrôle la présence du PNG généré ($> 0$ octet).
- `generate_execution_time_plot()` : ligne 234
- `generate_comparisons_plot()` : ligne 239
- `generate_auxiliary_memory_plot()` : ligne 244

---

## 4. Chemin du Flux d'Exécution d'un Benchmark Complet

1. **Création de l'échantillon** : `main.c` -> `sample_manager_add()` -> `generate_table()` (`src/data/data.c`).
2. **Lancement du benchmark** : `main.c` (`afficher_menu_benchmark`) -> `benchmark_sample_repetitions()` (`src/benchmark/benchmark.c`).
3. **Isolation et exécution du tri** : `benchmark_sample_repetitions()` -> `benchmark_one()` -> `copy_table()` -> `dispatch_sort()` -> `quick_sort()` (`src/sorting/sorting.c`).
4. **Vérification d'intégrité** : `benchmark_one()` -> `is_sorted_correctly()` & `verify_integrity()` (`qsort`).
5. **Analyse statistique** : `main.c` (`afficher_menu_analyse`) -> `analyze_all_algorithms()` (`src/analysis/analysis.c`).
6. **Génération du graphique** : `main.c` (`afficher_menu_rapports`) -> `generate_execution_time_plot()` (`src/report/report.c`) -> Fichiers `.dat`, `.gp` -> Commande `gnuplot` -> Fichier image `.png`.
