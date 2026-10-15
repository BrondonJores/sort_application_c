# Documentation Technique — Application C Sorting Benchmark

## 1. Présentation et Objectifs du Projet

L'application **C Sorting Benchmark** (`sort_application_c`) est une plateforme modulaire développée en C (norme C11) dédiée à l'évaluation, l'analyse comparative et la visualisation graphique des performances d'algorithmes de tri.

### Objectifs principaux :
1. **Gestion structurée d'échantillons de données** : Générer, conserver et consulter simultanément plusieurs jeux de données (échantillons) identifiés par un code unique (ex: `E001`, `E002`), avec pagination et filtrage.
2. **Mesure expérimentale rigoureuse** : Évaluer 6 algorithmes de tri canoniques (*Bubble Sort*, *Insertion Sort*, *Selection Sort*, *Merge Sort*, *Quick Sort*, *Heap Sort*) sur des copies strictement isolées afin de garantir une comparaison équitable.
3. **Multi-échantillonnage et statistiques** : Réaliser des campagnes d'expérimentation avec plusieurs répétitions, en calculant pour chaque métrique la moyenne, le minimum, le maximum et l'écart-type (*standard deviation*).
4. **Analyse groupée & Aide à la décision** : Regrouper les résultats par taille de tableau et par distribution afin de recommander l'algorithme le plus rapide, le moins gourmand en mémoire ou le meilleur compromis global.
5. **Visualisation GNUPlot & Exports CSV** : Produire des exports au format CSV et générer automatiquement des graphiques au format PNG via **GNUPlot** (diagrammes en barres avec barres d'erreur ou courbes de performance $T(N)$ selon la taille).

---

## 2. Fonctionnalités Réellement Disponibles

- **Gestionnaire d'échantillons (`SampleManager`)** :
  - Création individuelle d'échantillons avec choix de la taille $N$ et de la distribution.
  - Création de campagnes multi-tailles (ex: 3 tableaux de 100, 2 tableaux de 1000, 5 tableaux de 5000).
  - Consultation paginée (20 éléments par page) avec navigation par indices.
  - Recherche et filtrage par taille, distribution et identifiant (`E001`...).
  - Suppression d'un échantillon par ID.
- **5 Distributions de données** :
  - `RANDOM` (Aléatoire)
  - `SORTED` (Déjà trié)
  - `REVERSE_SORTED` (Trié en ordre inverse)
  - `NEARLY_SORTED` (Presque trié — 10% de permutations)
  - `DUPLICATES` (Contenant de nombreuses valeurs dupliquées)
- **6 Algorithmes de tri instrumentés** :
  - Prise en charge des ordres `ASCENDING` (croissant) et `DESCENDING` (décroissant).
  - Vérification automatique de la validité du tri et de l'intégrité des éléments (conservation des données).
- **Moteur de Benchmark & Répétitions** :
  - Sélection flexible des échantillons (par ID, par taille, par distribution ou tous).
  - Exécution de $R$ répétitions sans altération du tableau d'origine conservé dans le gestionnaire.
- **Analyse statistique & Aide à la décision** :
  - Calcul du temps moyen, écart-type, min, max, comparaisons moyennes, échanges moyens et mémoire auxiliaire.
  - Regroupement strict par taille et distribution (aucune moyenne trompeuse inter-tailles).
  - Recommandations algorithmiques selon le critère sélectionné (`FASTEST`, `MEMORY_EFFICIENT`, `BEST_OVERALL`).
- **Génération graphique GNUPlot & Exports CSV** :
  - Export CSV complet pour les données, les résultats individuels et les analyses statistiques.
  - Génération de fichiers de données `.dat`, scripts `.gp` et exécution de GNUPlot pour produire des images `.png` (diagrammes en barres ou courbes $T(N)$).

---

## 3. Arborescence et Architecture du Projet

```
sort_application_c/
├── Makefile                       # Système de compilation (all, test, debug, clean)
├── README.md                      # Documentation succincte initiale
├── main.c                         # Point d'entrée principal et interface utilisateur CLI
├── include/                       # Fichiers d'en-tête (interfaces publiques)
│   ├── analysis.h                 # Structures et déclarations pour l'analyse statistique
│   ├── benchmark.h                # Structures et moteur d'exécution des benchmarks
│   ├── data.h                     # Structures de base pour les tableaux et distributions
│   ├── decision.h                 # Interface d'aide à la décision
│   ├── file_manager.h             # Fonctions d'exportation CSV
│   ├── main.h                     # Interface du module principal
│   ├── report.h                   # Génération des graphiques GNUPlot
│   ├── sample_manager.h           # Gestionnaire d'échantillons (E001, E002...)
│   └── sorting.h                  # Signatures des 6 algorithmes de tri
├── src/                           # Implémentations des modules C
│   ├── analysis/analysis.c        # Calcul des moyennes, min, max, écarts-types
│   ├── benchmark/benchmark.c      # Isolation des tris, vérification d'intégrité et répétitions
│   ├── data/data.c                # Allocation, libération et génération de distributions
│   ├── decision/decision.c        # Sélection des meilleurs algorithmes par critère
│   ├── file_manager/file_manager.c # Écriture des fichiers CSV
│   ├── report/report.c            # Génération des scripts .gp, .dat et exécution gnuplot
│   ├── sample_manager/sample_manager.c # Logique du gestionnaire d'échantillons
│   └── sorting/sorting.c          # Code source des 6 algorithmes de tri
├── tests/                         # Suite de tests automatisée
│   └── test_main.c                # Harnais de test unitaires (make test)
└── output/                        # Dossier généré pour les exports et graphiques
    └── plots/                     # Emplacement des fichiers .dat, .gp et .png
```

---

## 4. Description Détaillée des Modules

### 4.1. Module `data` (`include/data.h`, `src/data/data.c`)
- **Rôle** : Allocation mémoire dynamique (`init_table`), libération (`free_table`) et remplissage des tableaux (`generate_data`, `generate_table`).
- **Structure** :
  ```c
  typedef struct {
      double *elements;
      size_t nombre_elements;
  } table;
  ```
- **Distributions** : Remplit le tableau de `double` selon les modes `RANDOM`, `SORTED`, `REVERSE_SORTED`, `NEARLY_SORTED` et `DUPLICATES`.

### 4.2. Module `sample_manager` (`include/sample_manager.h`, `src/sample_manager/sample_manager.c`)
- **Rôle** : Catalogue dynamique en mémoire conservant plusieurs échantillons.
- **Structures** :
  ```c
  typedef struct {
      char id[16];                // Identifiant unique (ex: "E001")
      table donnees;              // Tableau de données
      DataDistribution distribution;
      int benchmark_disponible;   // Indicateur d'état
  } Sample;

  typedef struct {
      Sample *samples;
      size_t count;
      size_t capacity;
      size_t next_id_counter;
  } SampleManager;
  ```
- **Fonctions clés** :
  - `sample_manager_add()` : Génère un nouvel échantillon avec identifiant incrémental `E%03zu`.
  - `sample_manager_display_paginated()` : Affiche les éléments par tranche (ex: 20 par page).
  - `sample_manager_filter()` : Filtre les échantillons par taille et/ou distribution.
  - `sample_manager_delete_by_id()` : Supprime un échantillon sans réattribuer de façon ambiguë l'identifiant.

### 4.3. Module `sorting` (`include/sorting.h`, `src/sorting/sorting.c`)
- **Rôle** : Implémentation instrumentée des 6 algorithmes de tri.
- **Structure de comptage** :
  ```c
  typedef struct {
      int comparaisons;
      int swaps;
      double execution_time;
      size_t auxiliary_memory;
  } sort_stats;
  ```
- **Algorithmes** : `bubble_sort`, `insertion_sort`, `selection_sort`, `merge_sort`, `quick_sort`, `heap_sort`.

### 4.4. Module `benchmark` (`include/benchmark.h`, `src/benchmark/benchmark.c`)
- **Rôle** : Exécution isolée des benchmarks, mesure du temps (`clock()`), vérification d'intégrité (`qsort` comparatif) et répétitions.
- **Fonction clé** :
  - `benchmark_sample_repetitions()` : Prend un échantillon, clone ses données dans un tableau temporaire pour chaque répétition/algorithme, garantissant que le tableau d'origine n'est jamais modifié par le tri.

### 4.5. Module `analysis` (`include/analysis.h`, `src/analysis/analysis.c`)
- **Rôle** : Agrégation des résultats de benchmark.
- **Structure** : `analysis_result` conserve pour une combinaison (Algorithme, Taille, Distribution, Ordre) :
  - `sample_count` : Nombre d'exécutions comptabilisées.
  - `average_execution_time` et `stddev_execution_time`.
  - `min_execution_time` et `max_execution_time`.
  - `average_comparisons`, `stddev_comparisons`.
  - `average_swaps`, `stddev_swaps`.
  - `average_auxiliary_memory`, `stddev_auxiliary_memory`.

### 4.6. Module `decision` (`include/decision.h`, `src/decision/decision.c`)
- **Rôle** : Comparaison d'analyses d'un même groupe pour recommander l'algorithme optimal.
- **Fonctions** : `select_fastest_algorithm`, `select_memory_efficient_algorithm`, `select_best_algorithm` (score combiné $0.7 \times \text{temps} + 0.3 \times \text{mémoire}$).

### 4.7. Module `file_manager` (`include/file_manager.h`, `src/file_manager/file_manager.c`)
- **Rôle** : Exportation des données brutes, des résultats de benchmark individuels et des statistiques d'analyse au format CSV.

### 4.8. Module `report` (`include/report.h`, `src/report/report.c`)
- **Rôle** : Génération des rapports graphiques via GNUPlot.
- **Correction apportée** : Inscription de `#define _POSIX_C_SOURCE 200809L` en tête de fichier pour garantir la déclaration conforme de `popen()` / `pclose()`, éliminant le crash d'adresse 64-bits et l'erreur `-3`.

---

## 5. Étude Approfondie des 6 Algorithmes de Tri

| Algorithme | Complexité Meilleur Cas | Complexité Cas Moyen | Complexité Pire Cas | Mémoire Auxiliaire Théorique | Mémoire Mesurée / Estimée | Stabilité | En Place |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Bubble Sort** | $O(N)$ | $O(N^2)$ | $O(N^2)$ | $O(1)$ | $0$ octet | Stable | Oui |
| **Insertion Sort** | $O(N)$ | $O(N^2)$ | $O(N^2)$ | $O(1)$ | $0$ octet | Stable | Oui |
| **Selection Sort** | $O(N^2)$ | $O(N^2)$ | $O(N^2)$ | $O(1)$ | $0$ octet | Instable | Oui |
| **Merge Sort** | $O(N \log N)$ | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ | $N \times 8$ octets | Stable | Non |
| **Quick Sort** (3-voies) | $O(N \log N)$ | $O(N \log N)$ | $O(N^2)$ | $O(\log N)$ | $\text{profondeur} \times 24$ octets | Instable | Oui |
| **Heap Sort** | $O(N \log N)$ | $O(N \log N)$ | $O(N \log N)$ | $O(1)$ | $0$ octet | Instable | Oui |

### Remarques sur l'implémentation C :
- **Merge Sort** : Effectue une allocation dynamique unique `malloc(N * sizeof(double))` avant la récursion, ce qui donne une mesure exacte de la mémoire auxiliaire consommée ($N \times 8$ octets).
- **Quick Sort** : Implémenté avec un partitionnement à 3 voies (*Bentley-McIlroy*) gérant très efficacement les doublons, et éliminant la récursion terminale sur la sous-partition la plus grande pour borner la pile à $O(\log N)$.

---

## 6. Audit des Métriques et de leur Validité Critique

1. **Temps d'exécution** : Mesuré via `clock()` en secondes (`(fin - debut) / CLOCKS_PER_SEC`). Reflète le temps CPU du processus.
2. **Nombre de comparaisons (`comparaisons`)** : Incrémenté explicitement à chaque évaluation de condition de tri.
3. **Nombre d'échanges (`swaps`)** : Incrémenté uniquement dans `swap_values()` lors d'un permute physique entre 2 indices distincts.
   - *Rappel important* : Dans le Tri par Insertion, les décalages d'éléments `a[j] = a[j-1]` ne sont pas comptabilisés comme des swaps complets (`swaps = 0`), ce qui est théoriquement exact.
4. **Mémoire auxiliaire (`auxiliary_memory`)** :
   - *Merge Sort* : $N \times 8$ octets (allocation dynamique du tableau temporaire de fusion).
   - *Quick Sort* : Estimation de la taille des cadres de pile récursifs ($\text{profondeur max} \times 24$ octets).
   - *Tris en place* : $0$ octet.

---

## 7. Compilation, Installation et Exécution

### Prérequis :
- Compilateur `gcc` supportant C11.
- Utilitaire `make`.
- Paquet `gnuplot` (ex: `sudo apt install gnuplot-nox`).

### Commandes Makefile :
```bash
# Compilation du programme principal
make

# Exécution de la suite de tests automatisée
make test

# Mode débogage (symboles gdb et absence d'optimisation)
make debug

# Nettoyage des artefacts de compilation
make clean
```

### Exécution :
```bash
./sorting_benchmark
```

---

## 8. Bilan des Tests Automatisés

La suite de tests unitaires située dans `tests/test_main.c` valide :
- L'exactitude du tri des 6 algorithmes en ordre croissant et décroissant.
- Le traitement des cas limites : tableaux vides ($N=0$), tableaux à un seul élément ($N=1$), et tableaux saturés de doublons.
- Les fonctionnalités du gestionnaire d'échantillons (création, filtrage, suppression).
- Le moteur de benchmark sélectif et le calcul des statistiques multi-échantillons.
- La génération des fichiers de données `.dat`, des scripts `.gp` et la création effective des fichiers images PNG par GNUPlot.
