# sort_application_c
Documentation des Modules : data.c et file_manager.c

📊 1. data.c - Gestion et Génération des Données

Ce fichier gère l'allocation dynamique de la mémoire pour les tableaux de nombres et permet de créer des jeux de données selon différents scénarios de test.

Utilité principale :

Allocation et nettoyage : Alloue proprement l'espace mémoire (init_table) et libère les ressources (free_table) pour éviter les fuites mémoire.

Génération de distributions : Remplit le tableau avec des données spécifiques pour stresser les algorithmes de tri dans diverses conditions.

Schéma du fonctionnement de data.c :

 [ Demande de génération ] 
           │
           ▼
   ( init_table() ) ──► Allocation dynamique en mémoire (RAM)
           │
           ▼
   ( generate_data() ) ──► Sélection de la distribution :
         ├── RANDOM         (Aléatoire)
         ├── SORTED         (Déjà trié)
         ├── REVERSE_SORTED (Tri inversé)
         ├── NEARLY_SORTED  (Presque trié)
         └── DUPLICATES     (Avec doublons)


📁 2. file_manager.c - Gestion des Fichiers et Exports

Ce fichier prend les données manipulées ou les résultats de performance des tris et les sauvegarde dans des fichiers (au format CSV).

Utilité principale :

Exportation des tableaux : Sauvegarde un tableau de valeurs (export_data).

Exportation des benchmarks : Enregistre les métriques de performance (temps, comparaisons, échanges, mémoire auxiliaire) lors des tests de tri (export_benchmark_results).

Exportation des analyses : Sauvegarde les statistiques globales calculées sur plusieurs échantillons (export_analysis_results).

Schéma du fonctionnement de file_manager.c :

 [ Données / Résultats en mémoire ]
               │
               ▼
   ( Fonctions d'exportation )
         ├── export_data()                ──► Fichier de données brutes (.csv)
         ├── export_benchmark_results()   ──► Fichier des performances (.csv)
         └── export_analysis_results()    ──► Fichier des statistiques (.csv)
               │
               ▼
   [ Sauvegarde sur le disque dur ]


🔗 Interaction entre les deux modules

+-------------------+       Données initiales       +----------------------+
|     data.c        | ─────────────────────────────►| Algorithmes de Tri   |
| (Génération jeux) |                               | (Bubble, Quick, etc.)|
+-------------------+                               +----------------------+
                                                               │
                                                               ▼
+-------------------+       Résultats / Métriques   +----------------------+
|  file_manager.c   | ◄─────────────────────────────┤ Mesure des temps,    |
|  (Export fichiers)|                               | comparatifs, etc.    |
+-------------------+                               +----------------------+
