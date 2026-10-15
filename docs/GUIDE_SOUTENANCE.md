# Guide de Soutenance Orale — C Sorting Benchmark

## 1. Présentation Synthétique du Projet (Pitch 1 à 2 minutes)

> "Bonjour Monsieur le Professeur,
>
> Dans le cadre de notre projet de structures de données et d'algorithmique en C, nous avons développé **C Sorting Benchmark**, une plateforme modulaire dédiée à l'évaluation expérimentale, l'analyse statistique et la visualisation graphique de six algorithmes de tri canoniques : le tri à bulles, par insertion, par sélection, fusion, rapide et par tas.
>
> Notre application s'articule autour de trois piliers principaux :
> 1. **Un gestionnaire d'échantillons (`SampleManager`)** permettant de stocker plusieurs jeux de données avec un identifiant unique (ex: `E001`), de consulter les valeurs de façon paginée (20 éléments par page) et de filtrer par taille ou distribution.
> 2. **Un protocole d'expérimentation rigoureux** : pour chaque benchmark, l'application travaille sur des copies clonées strictement isolées des données d'origine et permet d'exécuter des campagnes de répétitions pour calculer des moyennes et des écarts-types.
> 3. **Un module d'analyse et de rendu graphique automatisé** communiquant avec **GNUPlot** pour générer des diagrammes en barres avec barres d'erreur ou des courbes de performance $T(N)$ au format PNG.
>
> L'ensemble du code respecte les principes du C11 modulaire, inclut une suite de tests unitaires validant 100% des fonctions et fournit des exports CSV réutilisables."

---

## 2. Vulgarisation de l'Architecture et Rôle des Modules

- `data` : C'est le "moteur de fabrication". Il alloue la mémoire RAM pour les tableaux et génère les 5 distributions de données (aléatoire, trié, inversé, presque trié, doublons).
- `sample_manager` : C'est le "catalogue / magasin d'échantillons". Il attribue les identifiants `E001`, `E002`, conserve les jeux de données et permet de consulter les valeurs sans engorger le terminal grâce à la pagination.
- `sorting` : C'est le "laboratoire". Il rassemble le code des 6 algorithmes de tri en comptabilisant précisément le temps, les comparaisons et les échanges.
- `benchmark` : C'est le "superviseur de test". Il duplique l'échantillon initial avant chaque tri pour qu'aucun algorithme ne modifie les données de ses concurrents, vérifie la validité du tri et réalise les répétitions.
- `analysis` : C'est le "calculateur statistique". Il regroupe les résultats par taille et distribution, puis calcule la moyenne, l'écart-type, le min et le max.
- `decision` : C'est l' "expert/conseiller". Il compare les statistiques pour recommander l'algorithme le plus rapide, le plus économe ou le meilleur compromis.
- `file_manager` : C'est l' "archiviste". Il sauvegarde les tableaux et les résultats dans des fichiers CSV lisibles par Excel ou Python.
- `report` : C'est le "graphiste". Il prépare les fichiers `.dat`, écrit les scripts `.gp` et demande à GNUPlot de générer les images `.png`.

---

## 3. Les 6 Algorithmes de Tri Expliqués Simplement

1. **Tri à bulles (`Bubble Sort`)** :
   - *Principe* : Parcourt le tableau et échange les éléments adjacents mal ordonnés. Les plus grandes valeurs "remontent" comme des bulles d'air.
   - *Analogie* : Trier des cartes en comparant deux cartes voisines et en les inversant.
2. **Tri par insertion (`Insertion Sort`)** :
   - *Principe* : Prend chaque élément un par un et l'insère à sa place exacte dans la partie déjà triée du tableau.
   - *Analogie* : Ranger une main de cartes à jouer en insérant la nouvelle carte au bon endroit.
3. **Tri par sélection (`Selection Sort`)** :
   - *Principe* : Recherche le plus petit élément du tableau et le met en première position, puis recommence pour le reste.
   - *Analogie* : Chercher la plus petite personne d'un groupe pour la mettre au début de la file.
4. **Tri fusion (`Merge Sort`)** :
   - *Principe* : Divise le tableau en deux moitiés, trie chaque moitié récursivement, puis "fusionne" les deux sous-tableaux triés.
   - *Analogie* : Diviser un paquet de cartes en deux, trier chaque tas, puis entrelacer les cartes dans l'ordre.
5. **Tri rapide (`Quick Sort`)** :
   - *Principe* : Choisit un élément "pivot", sépare le tableau entre les éléments plus petits et plus grands que le pivot, puis trie récursivement.
   - *Analogie* : Choisir une carte repère (ex: le 8), mettre les cartes $< 8$ à gauche et $> 8$ à droite.
6. **Tri par tas (`Heap Sort`)** :
   - *Principe* : Organise le tableau sous forme d'arbre (un tas binaire max), puis extrait la racine (le maximum) répétitivement pour la placer à la fin.
   - *Analogie* : Construire une pyramide d'éléments où le plus grand est au sommet, puis retirer le sommet successivements.

---

## 4. Signification et Interprétation des Métriques

- **Temps d'exécution ($s$)** : Mesuré via `clock()` CPU. Il varie selon la taille $N$, la complexité de l'algorithme et la vitesse du processeur.
- **Comparaisons** : Nombre exact d'évaluations de conditions `$a < b$` réalisées par l'algorithme. Indépendant du matériel.
- **Échanges (`swaps`)** : Nombre de permutations physiques entre 2 cases mémoire distinctes.
  - *Piège à éviter* : Dans le tri par insertion, les décalages d'éléments `a[j] = a[j-1]` ne sont pas des swaps complets. Le compteur d'échanges indique 0, ce qui est normal.
- **Mémoire Auxiliaire** :
  - *Merge Sort* : Consomme $O(N)$ octets dynamiques (`malloc` du tableau temporaire de fusion).
  - *Quick Sort* : Consomme $O(\log N)$ octets sur la pile de récursion.
  - *Tris en place* : Consomment $0$ octet auxiliaire.

---

## 5. 20 Questions Probables du Professeur et Réponses Justifiées

### Q1 : Quelle est la différence entre un tri en place et un tri stable ?
**Réponse** : Un tri est **en place** s'il n'utilise qu'un espace mémoire auxiliaire constant $O(1)$ (ex: Bubble, Insertion, Selection, Heap). Un tri est **stable** s'il conserve l'ordre relatif initial des éléments possédant des clés égales (ex: Bubble, Insertion, Merge).

### Q2 : Pourquoi le Tri par Sélection effectue-t-il toujours $O(N^2)$ comparaisons, même sur un tableau déjà trié ?
**Réponse** : Car son algorithme ne s'arrête pas prématurément : pour chaque indice $i$, la boucle interne parcourt systématiquement l'intégralité du reste du tableau de $i+1$ à $N-1$ pour trouver le minimum, sans condition de sortie anticipée.

### Q3 : Pourquoi Merge Sort consomme-t-il de la mémoire auxiliaire alors que Quick Sort n'en alloue pas dynamiquement ?
**Réponse** : Merge Sort nécessite un tableau temporaire de taille $N$ (`tmp = malloc(n * sizeof(double))`) dans `src/sorting/sorting.c` pour fusionner les sous-parties triées sans écraser les données. Quick Sort partitionne directement dans le tableau d'origine, mais utilise implicitement de la mémoire sur la pile d'appels pour la récursion ($O(\log N)$).

### Q4 : Comment garantissez-vous que les comparaisons entre algorithmes sont équitables ?
**Réponse** : Grâce à `benchmark_sample_repetitions()` dans `src/benchmark/benchmark.c`. Avant chaque tri, les données de l'échantillon d'origine sont clonées (`copy_table()`). Tous les 6 algorithmes s'exécutent ainsi sur une copie initiale identique du tableau.

### Q5 : Pourquoi le compteur de swaps du Tri par Insertion est-il égal à 0 ?
**Réponse** : Le Tri par Insertion effectue des décalages unidirectionnels (`a[j] = a[j-1]`) puis insère la clé à la position finale (`a[j] = key`). Notre fonction `swap_values()` ne compte que les échanges bilatéraux entre deux cases distinctes. Il n'y a donc pas de swap au sens strict.

### Q6 : Comment avez-vous résolu l'échec de GNUPlot (Code -3) sous Kali Linux ?
**Réponse** : L'erreur était due à l'absence de la macro POSIX `#define _POSIX_C_SOURCE 200809L` en tête de `src/report/report.c`. En mode ANSI C11 strict, le compilateur déclarait implicitement `popen()` avec un retour `int` (32 bits), ce qui tronquait le pointeur 64 bits et provoquait un crash mémoire. Nous avons également ajouté l'extension `.png` automatique et la vérification de la taille du fichier généré ($> 0$ octet).

### Q7 : Quelle est la complexité du Tri Rapide dans le pire des cas et comment l'avez-vous optimisé dans votre code ?
**Réponse** : Le pire cas théorique est $O(N^2)$ lorsque le pivot choisi est le pire élément. Dans `src/sorting/sorting.c`, nous avons implémenté un partitionnement à 3 voies (*Dutch National Flag*) ainsi qu'un appel récursif uniquement sur la plus petite sous-partition, ce qui limite la profondeur de pile à $O(\log N)$ et gère parfaitement les doublons.

### Q8 : Comment est calculé l'écart-type dans votre module d'analyse ?
**Réponse** : Dans `src/analysis/analysis.c`, l'écart-type est calculé par la formule de la racine carrée de la variance :
$$\sigma = \sqrt{\frac{1}{S} \sum_{i=1}^{S} (x_i - \bar{x})^2}$$
où $S$ est le nombre d'échantillons/répétitions et $\bar{x}$ la moyenne.

### Q9 : Pourquoi ne faut-il pas mélanger les statistiques d'échantillons de tailles différentes ?
**Réponse** : Car la complexité temporelle varie de façon non-linéaire (ex: $N^2$ vs $N \log N$). Faire la moyenne entre un test à $N=100$ et un test à $N=5000$ produirait une valeur mathématiquement absurde sans aucune signification physique.

### Q10 : Que se passe-t-il si un utilisateur essaie de faire un benchmark sur un tableau vide ($N=0$) ?
**Réponse** : Le programme gère ce cas limite sans crasher : les algorithmes vérifient `nombre_elements == 0` et retournent immédiatement. Le module de vérification d'intégrité valide le tableau vide comme trié.

### Q11 : Quelle est l'utilité du module `SampleManager` ?
**Réponse** : `SampleManager` permet de conserver en mémoire plusieurs jeux de données identifiés par un code (ex: `E001`, `E002`), de les filtrer par taille ou distribution et de les consulter de manière paginée sans encombrer la console.

### Q12 : Quelle fonction C utilisez-vous pour mesurer le temps d'exécution ?
**Réponse** : La fonction standard `clock()` définie dans `<time.h>`. Le résultat est divisé par `CLOCKS_PER_SEC` pour obtenir le temps CPU en secondes sous forme de `double`.

### Q13 : Comment vérifiez-vous qu'un algorithme n'a pas corrompu les données du tableau ?
**Réponse** : Dans `verify_integrity()` (`src/benchmark/benchmark.c`), nous créons une copie du tableau d'origine, nous la trions avec la fonction standard de C `qsort()`, puis nous comparons élément par élément avec le tableau trié par notre algorithme.

### Q14 : Quel est le rôle du fichier `Makefile` et quelles cibles propose-t-il ?
**Réponse** : Le `Makefile` automatise la compilation et la gestion des dépendances (`-MMD -MP`). Il propose les cibles `all` (programme principal), `test` (exécute `test_runner`), `debug` (flags `-g -O0`) et `clean`.

### Q15 : Pourquoi le Tri par Tas (`Heap Sort`) est-il intéressant en pratique ?
**Réponse** : Car il garantit une complexité temporelle optimale en $O(N \log N)$ dans le pire des cas tout en étant un tri **en place** ($O(1)$ mémoire auxiliaire), contrairement au Tri Fusion qui nécessite $O(N)$ mémoire.

### Q16 : Pourquoi le Tri à Bulles est-il considéré comme inefficace sur de grands tableaux ?
**Réponse** : En raison de sa complexité en $O(N^2)$ et du nombre élevé d'échanges mémoire physiques adjacents, ce qui génère un coût d'accès au cache très défavorable par rapport à Quick Sort ou Heap Sort.

### Q17 : Comment le module `report` décide-t-il d'afficher des barres d'erreur ou des courbes $T(N)$ ?
**Réponse** : Dans `src/report/report.c`, le programme compte le nombre de tailles distinctes dans les analyses. S'il n'y a qu'une seule taille, il produit un diagramme en barres avec barres d'erreur. S'il y a plusieurs tailles distinctes, il génère des courbes de performance $T(N)$ reliées par algorithme.

### Q18 : Quel est le critère utilisé par le module `decision` pour choisir le "meilleur algorithme global" ?
**Réponse** : Il calcule un score pondéré combiné : $\text{Score} = 0.7 \times \text{temps\_moyen} + 0.3 \times \text{mémoire\_auxiliaire}$. L'algorithme obtenant le score le plus faible est sélectionné.

### Q19 : Qu'apporte la distribution `NEARLY_SORTED` lors d'un benchmark ?
**Réponse** : Elle permet de mettre en évidence les algorithmes adaptatifs comme le Tri par Insertion, qui s'exécute en temps quasi-linéaire $O(N)$ sur des tableaux presque triés.

### Q20 : Comment sont gérées les erreurs lors de la saisie utilisateur dans le terminal ?
**Réponse** : Les fonctions `lire_entier()`, `lire_taille()` et `lire_chemin()` dans `main.c` utilisent `fgets()`, contrôlent le débordement de tampon, vident le flux en cas de ligne trop longue, et valident la conversion avec `strtol()` / `strtoull()`.
