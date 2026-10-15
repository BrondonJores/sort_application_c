# Synthèse pour le Rapport Académique — C Sorting Benchmark

Ce document présente une synthèse rédigée dans un style universitaire et rigoureux, directement exploitable pour la rédaction du rapport final du projet.

---

## 1. Contexte et Problématique

L'analyse de la complexité algorithmique constitue un pilier fondamental de l'informatique théorique. Si la notation Grand-$O$ permet de classifier le comportement asymptotique des algorithmes lorsque la taille des données $N$ tend vers l'infini, la pratique informatique réelle est confrontée à des contraintes matérielles concrètes : architecture du processeur, hiérarchie des mémoires caches, coût des allocations dynamiques et structure initiale des données.

La problématique scientifique de ce projet s'énonce ainsi :
> *Comment mesurer, analyser et visualiser de façon rigoureuse et équitable les performances réelles de différents algorithmes de tri en C, et dans quelle mesure les résultats expérimentaux confirment-ils ou nuancent-ils les prédictions de la complexité théorique ?*

---

## 2. Objectifs du Projet

1. **Développer un banc d'essai (*benchmark framework*) robuste et modulaire en C11**.
2. **Gérer un catalogue d'échantillons de données (`SampleManager`)** permettant la coexistence de multiples jeux de données caractérisés par leur taille, leur distribution et leur identifiant unique (`E001`, `E002`...).
3. **Isoler strictement les exécutions** de manière à ce qu'aucun algorithme de tri ne modifie les données d'origine destinées aux autres algorithmes.
4. **Instrumenter la collecte de métriques clés** : temps CPU, nombre de comparaisons, nombre d'échanges (*swaps*) et mémoire auxiliaire.
5. **Agréger les mesures par campagnes d'expérimentation** en calculant les moyennes, min, max et écarts-types.
6. **Générer automatiquement des représentations graphiques vectorielles/PNG via GNUPlot** (diagrammes en barres avec barres d'erreur et courbes de performance $T(N)$).

---

## 3. Cahier des Charges Réalisé

| Fonctionnalité | État | Description & Implémentation |
| :--- | :---: | :--- |
| **Gestionnaire d'échantillons** | **Réalisé** | Catalogue dynamique (`sample_manager.c`) avec IDs `E001`+, pagination 20 éléments/page, filtrage et suppression. |
| **6 Algorithmes de tri** | **Réalisé** | Tri à bulles, insertion, sélection, fusion, rapide (3 voies) et par tas dans `sorting.c`. |
| **5 Distributions de données** | **Réalisé** | Aléatoire, triée, inversée, presque triée, doublons dans `data.c`. |
| **Isolement des données** | **Réalisé** | Clonage de chaque échantillon avant tri dans `benchmark.c`. |
| **Calculs statistiques** | **Réalisé** | Moyennes, écarts-types, min et max calculés dans `analysis.c`. |
| **Aide à la décision** | **Réalisé** | Recommandation basée sur des critères factuels dans `decision.c`. |
| **Exports CSV** | **Réalisé** | Exportation structurée dans `file_manager.c`. |
| **Graphiques GNUPlot** | **Réalisé** | Génération de scripts `.gp`, fichiers `.dat` et images `.png` dans `report.c`. |
| **Tests automatisés** | **Réalisé** | Suite de tests unitaires complète dans `tests/test_main.c` (`make test`). |

---

## 4. Architecture et Principes de Conception

Le logiciel applique une architecture modulaire stricte garantissant le découplage des responsabilités (*Separation of Concerns*) :

```
[ UI Main Menu (main.c) ]
       │
       ├──────► [ SampleManager ] ──► [ Data Engine ]
       │
       ├──────► [ Benchmark Engine ] ──► [ Sorting Algorithms ]
       │               │
       │               ▼
       ├──────► [ Analysis Engine ] ──► [ Decision Helper ]
       │               │
       └──────► [ FileManager Export ] & [ GNUPlot Report Engine ]
```

---

## 5. Synthèse des Résultats Expérimentaux et Interprétation

### 5.1. Comparaison des Temps d'Exécution
Les mesures expérimentales confirment la rupture fondamentale de performance entre les algorithmes quadratiques en $O(N^2)$ (*Bubble*, *Insertion*, *Selection*) et les algorithmes quasi-linéaires en $O(N \log N)$ (*Merge*, *Quick*, *Heap*) :
- Pour de petites tailles ($N \le 50$), le **Tri par Insertion** surpasse souvent les algorithmes complexes en raison de la faible constante cachée dans sa boucle interne.
- Pour des tailles plus importantes ($N \ge 1000$), **Quick Sort** et **Merge Sort** surpasse le Tri à Bulles de plusieurs ordres de grandeur.

### 5.2. Impact des Distributions de Données
- **Sur données déjà triées (`SORTED`)** : Le Tri par Insertion s'exécute en temps linéaire $O(N)$ avec 0 échange.
- **Sur données avec doublons (`DUPLICATES`)** : Notre implémentation de Quick Sort à 3 voies (*Bentley-McIlroy*) conserve une complexité $O(N \log N)$ ou $O(N)$ là où un Quick Sort classique avec pivot simple se dégraderait.

### 5.3. Clarification sur la Mémoire Auxiliaire
- **Tri Fusion (`Merge Sort`)** : Seul algorithme nécessitant une allocation mémoire auxiliaire dynamique de taille $O(N)$ (tableau temporaire de fusion de $N \times 8$ octets).
- **Tri Rapide (`Quick Sort`)** : Utilise un espace auxiliaire $O(\log N)$ sur la pile d'appels récursive.
- **Autres tris** : Tris en place nécessitant un espace constant $O(1)$.

---

## 6. Limites de l'Étude Expérimentale

1. **Précision de la mesure temporelle** : La fonction `clock()` mesure le temps CPU avec la résolution du signal d'horloge du système. Pour des tableaux de très petite taille ($N < 100$), le temps mesuré peut être proche de 0 seconde.
2. **Effets du cache processeur** : Les accès mémoire séquentiels (comme dans le Tri par Insertion ou le Tri Fusion) bénéficient de la prélecture du cache L1/L2, tandis que les sauts d'index dans Heap Sort génèrent davantage de *cache misses*.

---

## 7. Conclusion et Perspectives

Le projet **C Sorting Benchmark** offre une solution logicielle complète, exacte et pédagogique pour l'étude comparative des algorithmes de tri. Grâce au gestionnaire d'échantillons, à l'isolement des données et au rendu graphique automatisé GNUPlot, il constitue un outil d'expérimentation rigoureux et directement exploitable.

### Perspectives d'amélioration :
- Implémentation d'algorithmes de tri non-comparatifs (ex: *Radix Sort*, *Counting Sort*).
- Mesure dynamique fine de l'empreinte mémoire maximale du processus via `getrusage()` (Linux).
