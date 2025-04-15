# gygy

# Projet: Réalisation d’un simulateur de CPU

## Description du projet

Ce projet en langage C a pour objectif de simuler une machine virtuelle simple avec un processeur (CPU)capable d’exécuter un programme écrit dans un langage assembleur basique.

## Fonctionnalités principales

1. Lecture et analyse de fichiers assembleur (étiquettes, sections .DATA et .CODE)
2. Gestion de registres : AX, BX, CX, DX
3. Support des instructions : MOV, ADD, SUB, JMP, JZ, JNZ, ALLOC, FREE, PUSH, POP, HALT, etc.
4. Gestion de segments mémoire :
   - DS : segment de données
   - CS : segment de code
   - SS : segment de pile (stack)
   - ES : segment supplémentaire (extra segment)
5. Prise en charge de plusieurs types d'adressage :
   immédiat, registre, mémoire directe, mémoire indirecte, avec surcharge de segment
6. Affichage étape par étape du programme
7. Système de flags (ZF, SF, IP, SP, BP)
8. Libération mémoire automatique en fin d'exécution

## Structure du projet

Le projet est composé des fichiers suivants :

### Fichier code:

- \*.h:
  1. hachage.h – Structures et prototypes pour la table de hachage (labels, constantes).
  2. parser.h – Fonctions de parsing des fichiers .txt (.DATA/.CODE).
  3. memory.h – Gestion des segments mémoire (DS, CS, SS, ES).
  4. cpu.h – Définition du CPU, registres, instructions et exécution.
- \*.c:
  1. hachage.c – Implémentation de la table de hachage.
  2. parser.c – Lecture, parsing, création des instructions, gestion des labels.
  3. memory.c – Allocation, libération, accès mémoire segmenté.
  4. cpu.c – Simulation du processeur, exécution des instructions.

### Fichier test:

- \*.txt:
  1. test_cpu_adressing.txt – Test des différents modes d’adressage.
  2. test_cpu.txt – Test général des sections DATA et CODE.
  3. test_stack.txt – Test du segment de pile (stack).
  4. test_extra.txt – Test du segment ES (extra) avec ALLOC/FREE.
- main:
  1. test_hachage.c – Test de la table de hachage (insertion, recherche).
  2. test_parser.c – Test du parsing des fichiers d’entrée.
  3. test_memory.c – Test de l’allocation et gestion mémoire.
  4. test_cpu_adressing.c – Test des modes d’adressage dans le CPU.
  5. test_cpu.c – Test complet du CPU (lecture fichier + exécution).
  6. test_stack.c – Test des instructions de pile (PUSH, POP).
  7. test_extra.c – Test de gestion du segment ES.
  8. main.c – Lancement global de tous les tests (scénarios complets).

### Fichier auxiliaires:

- Makefile - Automatisation de la compilation et des tests.
- debug.c – Fonctions d’affichage de l’état du CPU/mémoire (debug).
- debug.h – Déclarations des fonctions de debug.

## Compilation et Exécution

Le projet utilise un `Makefile` pour simplifier la compilation.

### Compilation :

Pour compiler tous les programmes :

> > make

Pour compiler uniquement le programme principal :

> > make main

Pour compiler un des modules de test :

> > make test_parser
> > make test_cpu
> > make test_cpu_adressing
> > make test_stack
> > make test_extra
> > make test_hachage
> > make test_memory

Pour nettoyer les fichiers objets (.o) et les exécutables :

> > make clean

### Exécution :

Pour exécuter le programme principal :

> > make run

Pour exécuter un module de test :

> > make run_parser
> > make run_cpu
> > make run_cpu_adressing
> > make run_stack
> > make run_extra

## Fonctionnalités et Menus des Programmes

### main.c (Tests CPU)

Le programme main.c exécute une série de tests interactifs simulant le fonctionnement d'un processeur simple, à travers plusieurs scénarios. Voici les principaux tests et leurs actions :

- Test 1 : Stockage et récupération dans le segment de données (DS)
  Affiche la liste des segments libres
  Crée un segment DS, y stocke une valeur, puis la lit
  Utilise les fonctions create_segment, store, load, remove_segment

- Test 2 : Lecture d'un programme assembleur (test_cpu.txt)
  Lit les sections .DATA et .CODE depuis un fichier
  Parse les instructions et résout les constantes (labels, variables)
  Alloue les segments DS/CS et exécute le programme instruction par instruction
  Affiche les registres, flags, et état mémoire à chaque étape

- Test 3 : Gestion de la pile (stack - SS)
  Alloue un segment SS pour la pile
  Exécute des instructions PUSH / POP
  Affiche la pile à chaque étape et sa position SP

- Test 4 : Segment Extra (ES)
  Exécute des instructions ALLOC / FREE pour gérer dynamiquement un segment ES
  Utilise des adresses comme [ES:AX] pour accéder à la mémoire
  Affiche le segment, détecte les erreurs de dépassement de mémoire

### test_cpu_adressing.c – Test des différents modes d’adressage

Ce fichier contient un test spécifique pour valider les 4 types d’adressage implémentés dans le processeur. Il initialise manuellement le contexte et la mémoire, puis applique différentes instructions MOV avec des combinaisons d'opérandes.

1. Le test suit ce déroulé :

- Initialisation du CPU et de la mémoire
- Registres AX, BX, CX, DX sont initialisés avec des valeurs
- Segment DS est alloué et rempli de valeurs [5, 15, 25, 35, ...]

2. Test des types d’adressage :

Pour chaque cellule du segment DS, une source différente est utilisée :

- i == 0 :
  ➤ Adressage immédiat
  Utilise une valeur littérale comme "42"
  ➜ Fonction : immediate_addressing

- i == 1 :
  ➤ Adressage indirect via registre
  Utilise "[AX]" pour accéder à la mémoire à l'adresse contenue dans AX
  ➜ Fonction : register_indirect_addressing

- i == 2 :
  ➤ Adressage mémoire directe
  Utilise une adresse mémoire comme "[5]"
  ➜ Fonction : memory_direct_addressing

- i == 3 :
  ➤ Adressage registre
  Utilise le nom d’un registre comme "CX"
  ➜ Fonction : register_addressing

- Cas invalide (résolution impossible)
  ➤ Test avec resolve_addressing(cpu, "888888", "MOV")
  Permet de tester les erreurs de format ou d’accès hors limites.

3. Affichage après chaque opération
   Affiche le segment de données DS
   Affiche la mémoire complète

### test_cpu.c – Exécution d’un programme assembleur simple

Ce programme exécute un fichier assembleur (test_cpu.txt) contenant les sections .DATA et .CODE, afin de simuler un programme complet sur la machine virtuelle.

Étapes réalisées :

1.  Initialisation du processeur  
    Appel à cpu_init(MEMORY_SIZE)
    Allocation de la mémoire et initialisation des registres

2.  Lecture et parsing du fichier .txt
    Appel à parse("test_cpu.txt")
    Chargement des instructions .DATA et .CODE dans des structures de type Instruction

3.  Allocation du segment de données (DS)
    Appel à allocate_variables(...)
    Les variables définies dans .DATA sont stockées dans le segment DS

4.  Affichage de l’état initial
    Contenu de DS
    Table de hachage des noms de variables (via display_hashmap)

5.  Allocation du segment de code (CS)
    Instructions stockées dans CS à partir de la section .CODE
    Résolution des constantes et labels

6.  Appel à resolve_constants(...)
    Les noms de variables et étiquettes sont remplacés par leurs indices

7.  Exécution du programme
    Appel à run_program(cpu)
    Exécution instruction par instruction, avec affichage de l’état après chaque étape

8.  Libération des ressources
    Appel à free_parser_result(pr) et cpu_destroy(cpu)

### test_extra.c – Test de l’allocation dynamique avec le segment ES (Extra Segment)

Ce programme teste spécifiquement les instructions de gestion dynamique de mémoire : ALLOC, FREE, et l’accès mémoire via le segment ES.

Étapes réalisées :

1. Initialisation du processeur
   Appel à cpu_init(MEMORY_SIZE)
   Initialisation des registres et de la mémoire

2. Lecture du fichier test_extra.txt
   Contient des instructions .DATA et .CODE
   Analyse via parse(...)

3. Allocation des segments de données et de pile
   allocate_variables(...) pour DS
   allocate_stack_segment(...) pour SS

4. Allocation du segment de code (CS)
   Insertion des instructions dans CS via allocate_code_segment(...)

5. Affichage des instructions initiales
   Avant la résolution des variables et labels

6. Résolution des constantes
   Remplacement des noms de variables (ex. a, b, labels) par leurs indices
   Utilise resolve_constants(...)

7. Préparation et test du segment ES
   Initialise les registres AX, BX, ZF manuellement
   Alloue dynamiquement un segment ES via alloc_es_segment(...)
   Affiche le contenu de ES avec print_extra_segment(...)

8. Exécution du programme
   Via run_program(cpu)
   Teste les instructions MOV [ES:AX], ALLOC, FREE, etc.

9. Libération du segment ES
   Appel à free_es_segment(...)
   Affiche l’état du segment ES après libération

10. Nettoyage mémoire
    free_parser_result(...) et cpu_destroy(...)

### test_hachage.c – Test de la table de hachage

Ce fichier teste les opérations de base sur la structure HashMap, utilisée dans le projet pour stocker les constantes, labels, variables ou registres.

Fonctionnalités testées :

1. Création de la table de hachage
   hashmap_create(SIMPLE)

2. Insertion de paires clé-valeur
   key1 -> value1
   key2 -> value2
   key3 -> value3
   ➤ Via hashmap_insert(...)

3. Affichage du contenu
   Utilise display_hashmap(...) pour visualiser toutes les entrées

4. Récupération d’une valeur
   hashmap_get(map, "key1") retourne "value1"

5. Suppression de paires
   Suppression progressive de key1 puis key2
   ➤ Vérifie que la suppression n’affecte pas les autres entrées

6. Destruction complète
   hashmap_destroy(map) libère toute la mémoire utilisée

### test_memory.c – Test de gestion de la mémoire et des segments

Ce test vérifie le bon fonctionnement des fonctions de gestion mémoire : allocation, suppression, affichage et nettoyage des segments.

Fonctionnalités testées :

1. Initialisation de la mémoire
   memory_init(MEMORY_SIZE) initialise le gestionnaire de mémoire avec une taille fixée

2. Affichage de la mémoire initiale
   Utilise display_memory(...) pour visualiser l’état des segments (libres et alloués)

3. Création de segments
   create_segment(...) est appelée trois fois :
   segment_1 à la position 1, taille 5
   segment_2 à la position 52, taille 20
   segment_3 à la position 72, taille 20

   Messages d'erreur s'affichent si l’allocation échoue

4. Affichage mémoire après allocations
   Vérifie que les segments ont bien été insérés dans la structure

5. Suppression de segments
   remove_segment(...) est utilisée pour libérer les trois segments
   Utilisation de hashmap_get(...) pour vérifier leur existence

6. Affichage final de la mémoire
   Visualisation de la liste des segments libres après suppression

7. Libération complète de la mémoire
   memory_destroy(...) nettoie toute la structure et libère les pointeurs

### test_parser.c – Test du parser et de la mémoire (Exercices 3 et 4)

Ce test vérifie le bon fonctionnement du parseur (.DATA / .CODE) ainsi que les fonctions de base d’allocation et d’accès à la mémoire (store / load) via le segment DS.

Fonctionnalités testées :

1. Lecture et parsing du fichier test_parser.txt
   Appel à parse(...) pour extraire les instructions .DATA et .CODE
   Affiche les instructions via display_instructions(...)

2. Affichage des structures de hachage
   Affiche :
   labels : pour les étiquettes dans .CODE
   memory_locations : pour les variables dans .DATA
   Utilise display_hashmap(...)

3. Initialisation du CPU et de la mémoire
   Appel à cpu_init(...)
   Allocation du segment de données DS via allocate_variables(...)
   Affiche l’état initial du segment DS

4. Test des fonctions store et load
   Stocke manuellement la valeur 764 dans la cellule DS[0]
   Charge la valeur de DS[0] et vérifie qu’elle a bien été récupérée

5. Libération de la mémoire
   Nettoie correctement les structures via free_parser_result(...) et cpu_destroy(...)

### test_stack.c – Test du segment de pile (SS) et instructions associées

Ce test vérifie la bonne gestion du segment de pile (SS) ainsi que l’exécution d’instructions utilisant la pile.

Fonctionnalités testées :

1. Initialisation du CPU
   Création d’un gestionnaire de mémoire et des registres via cpu_init(...)

2. Lecture du fichier test_stack.txt
   Chargement des instructions .DATA et .CODE avec parse(...)

3. Allocation du segment de données DS
   Utilise allocate_variables(...) pour stocker les variables

4. Affichage de l’état initial
   Affiche le contenu du segment DS
   Affiche la liste des segments libres (print_free_list(...))

5. Allocation du segment de pile SS
   Alloue dynamiquement la pile avec allocate_stack_segment(...)
   Affiche l’état initial de la pile (display_stack_segment(...))

6. Allocation du segment de code CS
   Charge les instructions avec allocate_code_segment(...)

7. Résolution des constantes
   Remplace les noms des variables/labels par des indices dans .CODE via resolve_constants(...)

8. Exécution du programme
   Lance l’interpréteur avec run_program(cpu)
   Teste les opérations PUSH, POP, accès à la pile, manipulation de SP, etc.

9. Libération de la mémoire
   Appel à free_parser_result(...) et cpu_destroy(...) pour tout nettoyer

## Liste des Fonctions

### `hachage.h`

Cette bibliothèque implémente une table de hachage simple avec une taille fixe et résolution des collisions par sondage linéaire. La valeur associée à chaque clé peut être de tout type (gérée via void \*).

1. ## unsigned long simple_hash(const char \*str)

   Calcule une clé de hachage à partir d'une chaîne.
   -> Paramètre : const char\* str — chaîne à hacher
   -> Retour : unsigned long — clé de hachage

2. ## HashMap \*hashmap_create(hashmap_value_t type)

   Initialise une table de hachage vide.
   -> Paramètre : hashmap_value_t type — type de valeur
   -> Retour : pointeur vers la table ou NULL si erreur

3. ## int hashmap_insert(HashMap *map, const char *key, void \*value)

   Insère une paire (clé, valeur) dans la table.
   -> Paramètres :

   - HashMap\* map : table cible
   - const char\* key : clé
   - void\* value : valeur associée
     -> Retour :
   - 1 si insertion réussie
   - -1 en cas d'erreur

4. ## void *hashmap_get(HashMap *map, const char \*key)

   Récupère la valeur associée à une clé.
   -> Paramètres :

   - HashMap\* map : table cible
   - const char\* key : clé à chercher
     -> Retour :
   - pointeur vers la valeur, ou NULL si non trouvée

5. ## int hashmap_remove(HashMap *map, const char *key)

   Supprime un élément de la table (en marquant une TOMBSTONE).
   -> Paramètres :

   - HashMap\* map : table cible
   - const char\* key : clé à supprimer
     -> Retour :
   - 1 si suppression réussie
   - 0 si clé non trouvée
   - -1 en cas d'erreur

6. ## void hashmap_destroy(HashMap \*map)
   Libère toute la mémoire utilisée par la table.
   -> Paramètre : HashMap\* map — table à libérer
   -> Retour : aucun (void)

### `parser.h`

Ce bibliothèque lit un fichier assembleur simplifié (.DATA et .CODE), extrait les instructions et les stocke dans une structure ParserResult.

1. ## ParserResult \*init()

   Initialise une structure `ParserResult` vide.
   -> Retour :

   - Pointeur vers une structure ParserResult initialisée
   - NULL en cas d'erreur d'allocation

2. ## Instruction *parse_data_instruction(const char *line, HashMap \*memory_locations)

   Analyse une ligne .DATA et retourne une instruction.
   -> Paramètres :

   - const char \*line : ligne .DATA à analyser
   - HashMap \*memory_locations : table pour stocker les emplacements mémoire
     -> Retour :
   - Pointeur vers Instruction créée
   - NULL si erreur de format ou d’allocation

3. ## Instruction *parse_code_instruction(const char *line, HashMap \*labels, int code_count)

   Analyse une ligne .CODE et retourne une instruction.
   -> Paramètres :

   - const char \*line : ligne de code à analyser
   - HashMap \*labels : table des étiquettes
   - int code_count : position de l’instruction dans le code
     -> Retour :
   - Pointeur vers Instruction créée
   - NULL en cas d’erreur

4. ## ParserResult *parse(const char *filename)

   Analyse un fichier assembleur contenant des sections .DATA et .CODE.
   -> Paramètre :

   - const char \*filename : nom du fichier à lire
     -> Retour :
   - Pointeur vers structure ParserResult remplie
   - NULL en cas d’erreur de parsing ou d’allocation

5. ## void free_parser_result(ParserResult \*result)

   Libère complètement la mémoire allouée pour un `ParserResult`.
   -> Paramètre :

   - ParserResult \*result : structure à libérer
     -> Retour : aucun

6. ## void liberer_instruction(Instruction \*i)

   Libère la mémoire d’une instruction individuelle.
   -> Paramètre :

   - Instruction \*i : instruction à libérer
     -> Retour : aucun

7. ## void reset_parse_offset()
   Réinitialise l’offset global de position mémoire (utilisé dans .DATA).
   -> Aucun paramètre
   -> Retour : aucun

### `memory.h`

Ce module gère la mémoire segmentée d’un processeur virtuel.
Il permet de créer, supprimer, stocker et lire des données dans des segments nommés,
tout en maintenant une liste des segments libres et une table de hachage des segments alloués.

1. ## MemoryHandler \*memory_init(int size)

   Initialise une structure `MemoryHandler` représentant la mémoire principale.
   -> Paramètre :

   - int size : taille totale de la mémoire à allouer
     -> Retour :
   - Pointeur vers la structure MemoryHandler initialisée
   - NULL en cas d'erreur

2. ## Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment\*\* prev)

   Recherche un segment libre pouvant accueillir une allocation.
   -> Paramètres :

   - MemoryHandler \*handler : le gestionnaire mémoire
   - int start : adresse de départ souhaitée
   - int size : taille de segment souhaitée
   - Segment \*\*prev : pointeur vers le segment précédent (pour modification de la liste chaînée)
     -> Retour :
   - Pointeur vers un segment libre satisfaisant les conditions
   - NULL si aucun segment valide n’a été trouvé

3. ## int create_segment(MemoryHandler *handler, const char *name, int start, int size)

   Crée un segment nommé dans la mémoire.
   -> Paramètres :

   - MemoryHandler \*handler : le gestionnaire mémoire
   - const char \*name : nom du segment
   - int start : adresse de départ
   - int size : taille du segment
     -> Retour :
   - 1 si la création est réussie
   - 0 si aucun segment libre n’est disponible
   - -1 en cas d’erreur

4. ## int remove_segment(MemoryHandler *handler, const char *name)

   Supprime un segment de la mémoire.
   -> Paramètres :

   - MemoryHandler \*handler : le gestionnaire mémoire
   - const char \*name : nom du segment à supprimer
     -> Retour :
   - 1 si la suppression a réussi
   - -1 en cas d’erreur (segment introuvable)

5. ## void memory_destroy(MemoryHandler \*handler)

   Libère toute la mémoire allouée à la structure `MemoryHandler`.
   -> Paramètre :

   - MemoryHandler \*handler : le gestionnaire à détruire
     -> Retour : aucun

6. ## int getSegFreePos(MemoryHandler \*handler, int size)

   Donne la première position disponible pour un segment de taille donnée.
   -> Paramètres :

   - MemoryHandler \*handler : le gestionnaire mémoire
   - int size : taille du segment à placer
     -> Retour :
   - index de départ libre si trouvé
   - -1 si aucun espace ne convient

7. ## void print_free_list(MemoryHandler \*handler)
   Affiche l’état de la liste des segments libres.
   -> Paramètre :
   - MemoryHandler \*handler : le gestionnaire mémoire
     -> Retour : aucun

### `debug.h`

Ce module contient des fonctions d'affichage utiles pour le débogage du processeur virtuel.
Il permet d’inspecter facilement les tables de hachage, les segments mémoire, les instructions et l’état de la pile.
Ces fonctions sont utilisées à des fins de visualisation lors des tests ou de l’exécution pas à pas du programme.

1. ## void display_hashentry(HashEntry \*entry)

   Affiche une seule entrée d'une table de hachage.
   -> Paramètre :

   - HashEntry \*entry : pointeur vers l’entrée à afficher
     -> Retour : aucun

2. ## void display_hashmap(HashMap \*map)

   Affiche l’ensemble des entrées non nulles d’une table de hachage.
   -> Paramètre :

   - HashMap \*map : table de hachage à afficher
     -> Retour : aucun

3. ## void display_segment(Segment \*seg)

   Affiche un segment mémoire (début, fin, taille).
   -> Paramètre :

   - Segment \*seg : segment à afficher
     -> Retour : aucun

4. ## void display_segments(Segment \*seg)

   Affiche récursivement une liste chaînée de segments mémoire.
   -> Paramètre :

   - Segment \*seg : premier segment de la liste
     -> Retour : aucun

5. ## void display_memory(MemoryHandler \*mem)

   Affiche un résumé de la mémoire : segments libres et segments alloués.
   -> Paramètre :

   - MemoryHandler \*mem : gestionnaire de mémoire
     -> Retour : aucun

6. ## void display_instructions(Instruction \*\*liste, int count)

   Affiche une liste d’instructions assembleur (mnemonic + opérandes).
   -> Paramètres :

   - Instruction \*\*liste : tableau de pointeurs vers instructions
   - int count : nombre d’instructions à afficher
     -> Retour : aucun

7. ## void display_stack_segment(CPU \*cpu)

   Affiche le contenu du segment de pile (SS) avec les pointeurs SP et BP.
   -> Paramètre :

   - CPU \*cpu : processeur contenant le segment SS
     -> Retour : aucun

8. ## void print_extra_segment(CPU \*cpu)
   Affiche le contenu du segment extra (ES), s’il est alloué.
   -> Paramètre :
   - CPU \*cpu : processeur contenant le segment ES
     -> Retour : aucun

### `cpu.h`

============= INITIALISATION ET DESTRUCTION =============

1. ## CPU \*cpu_init(int memory_size)

   Initialise une structure CPU avec un gestionnaire mémoire et des registres.
   -> Paramètre : int memory_size — taille totale de la mémoire
   -> Retour : pointeur vers structure CPU initialisée ou NULL

2. ## void cpu_destroy(CPU \*cpu)
   Libère toute la mémoire utilisée par la structure CPU.
   -> Paramètre : CPU \*cpu — CPU à libérer
   -> Retour : aucun

============= MÉMOIRE (STORE / LOAD) =============

3. ## void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data)

   Stocke une donnée dans un segment mémoire à la position spécifiée.
   -> Retour : pointeur vers la donnée stockée ou NULL

4. ## void *load(MemoryHandler *handler, const char \*segment_name, int pos)

   Charge une donnée depuis un segment mémoire.
   -> Retour : pointeur vers la donnée ou NULL

5. ## void print_registres_et_drapeaux(CPU \*cpu)
   Affiche l’état des registres généraux (AX, BX...) et des drapeaux IP, ZF, SF.

============= DATA =============

1. ## void allocate_variables(CPU \*cpu, Instruction \*\*data_instructions, int data_count)

   Alloue dynamiquement le segment de données "DS" en fonction des instructions .DATA.
   -> Paramètres :

   - CPU \*cpu : le processeur contenant le gestionnaire mémoire
   - Instruction \*\*data_instructions : tableau d'instructions de la section .DATA
   - int data_count : nombre d'instructions .DATA à traiter
     -> Retour : aucun
     -> Fonctionnement :
   - Calcule la taille totale nécessaire pour les données déclarées
   - Alloue le segment DS avec `create_segment`
   - Utilise `parserString` pour convertir les chaînes de valeurs
   - Stocke les valeurs dans le segment DS avec `store`

2. ## void print_data_segment(CPU \*cpu)

   Affiche le contenu actuel du segment de données "DS".
   -> Paramètre :

   - CPU \*cpu : le processeur contenant la mémoire
     -> Retour : aucun
     -> Affiche :
   - Adresse de départ et taille du segment DS
   - Contenu de chaque cellule mémoire du segment DS

3. ## int parserString(const char \*str, int \*\*tab)

   Analyse une chaîne de caractères contenant des entiers séparés par des virgules
   et remplit un tableau d'entiers correspondant.
   -> Paramètres :

   - const char \*str : la chaîne à analyser, exemple "1,2,3"
   - int \*\*tab : pointeur vers le tableau à remplir
     -> Retour :
   - Le nombre d'entiers extraits de la chaîne
     -> Remarque :
   - La mémoire pour `*tab` est allouée dynamiquement et doit être libérée après usage

4. ## int parserStringVal(const char \*str)
   Compte combien de valeurs numériques sont présentes dans une chaîne.
   -> Paramètre :
   - const char \*str : la chaîne contenant les valeurs (ex. "5,10,15")
     -> Retour :
   - Le nombre total de valeurs séparées par des virgules

============= ADDRESSING =============

1. ## int matches(const char *pattern, const char *string)

   Vérifie si une chaîne correspond à une expression régulière.
   -> Paramètres :

   - const char \*pattern : motif regex (ex. "^\\[AX\\]$")
   - const char \*string : chaîne à tester
     -> Retour :
   - 1 si la chaîne correspond au motif
   - 0 sinon

2. ## void *immediate_addressing(CPU *cpu, const char \*operand)

   Résout un opérande immédiat comme "42".
   -> Paramètres :

   - CPU \*cpu : processeur contenant la constant_pool
   - const char \*operand : opérande immédiat à convertir
     -> Retour :
   - Pointeur vers une valeur entière allouée dynamiquement
   - NULL si l'opérande n'est pas un entier valide

3. ## void *register_addressing(CPU *cpu, const char \*operand)

   Résout un accès direct à un registre (ex : "AX").
   -> Paramètres :

   - CPU \*cpu : processeur contenant les registres
   - const char \*operand : nom du registre
     -> Retour :
   - Pointeur vers la valeur entière contenue dans le registre
   - NULL si le registre est introuvable

4. ## void *memory_direct_addressing(CPU *cpu, const char *operand, const char *mnemonic)

   Résout un accès direct à une cellule mémoire (ex : "[5]").
   -> Paramètres :

   - CPU \*cpu : processeur contenant la mémoire
   - const char \*operand : adresse directe entre crochets
   - const char \*mnemonic : instruction concernée ("MOV", "JMP", etc.)
     -> Retour :
   - Pointeur vers la valeur à l'adresse indiquée dans le segment approprié (DS ou CS)
   - NULL si erreur de format ou d'adresse

5. ## void *register_indirect_addressing(CPU *cpu, const char \*operand)

   Résout un accès indirect via un registre (ex : "[AX]").
   -> Paramètres :

   - CPU \*cpu : processeur contenant les registres et la mémoire
   - const char \*operand : registre entre crochets
     -> Retour :
   - Pointeur vers la valeur stockée à l'adresse contenue dans le registre
   - NULL si le registre est introuvable ou que l'adresse est invalide

6. ## void *resolve_addressing(CPU *cpu, const char *operand, const char *mnemonic)

   Détecte et applique automatiquement le bon mode d'adressage.
   -> Paramètres :

   - CPU \*cpu : processeur concerné
   - const char \*operand : opérande à interpréter
   - const char \*mnemonic : instruction associée
     -> Retour :
   - Pointeur vers la valeur correspondante
   - NULL si aucun type d'adressage ne convient

7. ## void *segment_override_addressing(CPU *cpu, const char \*operand)
   Résout un adressage de type "[SEGMENT:REGISTRE]".
   -> Paramètres :
   - CPU \*cpu : processeur contenant la mémoire et les registres
   - const char \*operand : chaîne du type "[DS:AX]"
     -> Retour :
   - Pointeur vers la valeur située à l'adresse effective calculée
   - NULL en cas d’erreur de syntaxe ou d'adresse

============= CODE =============

1. ## void allocate_code_segment(CPU \*cpu, Instruction \*\*code_instructions, int code_count)

   Alloue dynamiquement le segment "CS" (Code Segment) et y stocke les instructions.
   -> Paramètres :

   - CPU \*cpu : processeur à modifier
   - Instruction \*\*code_instructions : tableau d'instructions
   - int code_count : nombre total d'instructions
     -> Retour : aucun

2. ## int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest)

   Exécute l'instruction en appelant la fonction appropriée selon le mnémonique.
   -> Paramètres :

   - CPU \*cpu : processeur exécutant l'instruction
   - Instruction \*instr : instruction à traiter
   - void \*src : valeur source
   - void \*dest : emplacement destination
     -> Retour :
   - 0 si succès
   - -1 en cas d'erreur

3. ## int handle_MOV(CPU *cpu, void *src, void \*dest)

   Copie la valeur source dans la destination.
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*src : adresse source
   - void \*dest : adresse de destination
     -> Retour :
   - 0 si succès
   - -1 en cas d'erreur

4. ## int handle_ADD(CPU *cpu, void *src, void \*dest)

   Ajoute la source à la destination et stocke le résultat dans celle-ci.
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*src : valeur à ajouter
   - void \*dest : emplacement du résultat
     -> Retour :
   - 0 si succès
   - -1 si erreur

5. ## int handle_CMP(CPU *cpu, void *src, void \*dest)

   Compare dest - src, met à jour ZF (zero flag) et SF (sign flag).
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*src : opérande à soustraire
   - void \*dest : opérande de référence
     -> Retour :
   - 0 si succès
   - -1 si erreur

6. ## int handle_JMP(CPU *cpu, void *adress)

   Effectue un saut inconditionnel vers une adresse donnée dans le code.
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*adress : adresse cible dans CS
     -> Retour :
   - 0 si succès
   - -1 si erreur

7. ## int handle_JZ(CPU *cpu, void *adress)

   Effectue un saut si le registre ZF (zero flag) est égal à 1.
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*adress : adresse de saut
     -> Retour :
   - 0 si succès
   - -1 si erreur

8. ## int handle_JNZ(CPU *cpu, void *adress)

   Effectue un saut si le registre ZF est égal à 0.
   -> Paramètres :

   - CPU \*cpu : processeur
   - void \*adress : adresse de saut
     -> Retour :
   - 0 si succès
   - -1 si erreur

9. ## int handle_HALT(CPU \*cpu)

   Arrête l'exécution du programme.
   -> Paramètre :

   - CPU \*cpu : processeur
     -> Retour :
   - 1 pour signaler l'arrêt
   - 0 ou -1 selon les conventions d'erreur

10. ## int handle_PUSH(CPU *cpu, void *src)

    Empile une valeur dans le segment de pile (SS).
    -> Paramètres :

    - CPU \*cpu : processeur
    - void \*src : valeur à empiler
      -> Retour :
    - 0 si succès
    - -1 si erreur

11. ## int handle_POP(CPU *cpu, void *dest)

    Dépile une valeur de la pile et la stocke à l'adresse de destination.
    -> Paramètres :

    - CPU \*cpu : processeur
    - void \*dest : emplacement de destination
      -> Retour :
    - 0 si succès
    - -1 si erreur

12. ## int handle_ALLOC(CPU \*cpu)

    Gère l’instruction ALLOC : alloue dynamiquement un segment mémoire "ES".
    -> Paramètre :

    - CPU \*cpu : processeur
      -> Retour :
    - 0 si le segment a été alloué
    - -1 si échec

13. ## int handle_FREE(CPU \*cpu)

    Gère l’instruction FREE : libère le segment "ES" s’il existe.
    -> Paramètre :

    - CPU \*cpu : processeur
      -> Retour :
    - 0 si le segment a été libéré
    - -1 si erreur

14. ## int execute_instruction(CPU *cpu, Instruction *instr)

    Résout les opérandes et exécute l'instruction avec le gestionnaire approprié.
    -> Paramètres :

    - CPU \*cpu : processeur
    - Instruction \*instr : instruction à exécuter
      -> Retour :
    - 0 si succès
    - 1 si HALT
    - -1 en cas d’erreur

15. ## Instruction *fetch_next_instruction(CPU *cpu)

    Récupère l'instruction pointée par le registre IP (Instruction Pointer), puis l’incrémente.
    -> Paramètre :

    - CPU \*cpu : processeur
      -> Retour :
    - Pointeur vers l’instruction courante
    - NULL si erreur ou fin de programme

16. ## int run_program(CPU \*cpu)

    Exécute toutes les instructions du programme jusqu’à HALT.
    -> Paramètre :

    - CPU \*cpu : processeur
      -> Retour :
    - 0 si programme terminé normalement
    - -1 si erreur d'exécution

17. ## void afficher_instructions(Instruction \*\*liste, int count)

    Affiche une liste d’instructions (mnemonique et opérandes).
    -> Paramètres : - Instruction \*\*liste : tableau d’instructions - int count : nombre d’instructions
    -> Retour : aucun

============= CONST =============

1. ## char *trim(char *str)

   Supprime les espaces, tabulations et retours à la ligne au début et à la fin d'une chaîne.
   -> Paramètre :

   - char \*str : chaîne de caractères à nettoyer
     -> Retour :
   - Pointeur vers la chaîne nettoyée (modifiée en place)

2. ## int search_and_replace(char \**str, HashMap *values)

   Remplace chaque mot dans la chaîne `str` correspondant à une clé de la table `values`
   par la valeur associée (sous forme de chaîne).
   -> Paramètres :

   - char \*\*str : pointeur vers la chaîne à modifier
   - HashMap \*values : table contenant les correspondances nom -> valeur
     -> Retour :
   - 1 si au moins un remplacement a été effectué
   - 0 si aucun mot n’a été remplacé

3. ## int resolve_constants(ParserResult \*result)

   Applique `search_and_replace` à toutes les instructions de .DATA et .CODE,
   pour remplacer noms de variables ou étiquettes par leurs indices.
   -> Paramètre :

   - ParserResult \*result : structure contenant les instructions et les tables
     -> Retour :
   - 0 si tous les remplacements ont été effectués avec succès
   - -1 en cas d’erreur (chaîne invalide ou valeur non trouvée)

============= STACK =============

1. ## int push_value(CPU \*cpu, int value)

   Empile une valeur entière dans le segment de pile (SS).
   -> Paramètres :

   - CPU \*cpu : structure du processeur contenant la pile
   - int value : valeur entière à empiler
     -> Effets :
   - Met à jour le registre SP (stack pointer)
   - Insère la valeur dans la mémoire au nouvel emplacement SP
     -> Retour :
   - 0 en cas de succès
   - -1 en cas d'erreur (pile non allouée ou débordement)

2. ## int pop_value(CPU *cpu, int *dest)

   Dépile une valeur du haut de la pile et la stocke dans `dest`.
   -> Paramètres :

   - CPU \*cpu : structure du processeur
   - int \*dest : pointeur vers une variable où stocker la valeur dépilée
     -> Effets :
   - Met à jour SP (décalage vers le haut de la pile)
   - Libère la case mémoire utilisée
     -> Retour :
   - 0 en cas de succès
   - -1 en cas d’erreur (pile vide, non allouée ou SP invalide)

3. ## void print_stack_segment(CPU \*cpu)

   Affiche toutes les valeurs actuellement stockées dans le segment de pile (SS).
   -> Paramètre :

   - CPU \*cpu : structure du processeur
     -> Affichage :
   - Affiche chaque case utilisée dans la pile
   - Affiche les positions actuelles de SP et BP
     -> Retour : aucun

4. ## void allocate_stack_segment(CPU \*cpu)
   Alloue dynamiquement le segment de pile "SS" après le segment de données "DS".
   -> Paramètre :
   - CPU \*cpu : structure du processeur
     -> Effets :
   - Crée un segment mémoire de taille fixe (définie par SS_SIZE)
   - Initialise SP et BP à la fin du segment
     -> Retour : aucun (affiche une erreur si DS introuvable ou allocation échoue)

============= EXTRA =============

1. ## int find_free_address_strategy(MemoryHandler \*handler, int size, int strategy)

   Recherche une zone mémoire libre capable d'accueillir un segment de taille donnée,
   en fonction de la stratégie choisie.
   -> Paramètres :

   - MemoryHandler \*handler : gestionnaire mémoire
   - int size : taille du segment à allouer
   - int strategy : stratégie utilisée (0 = First Fit, 1 = Best Fit, 2 = Worst Fit)
     -> Retour :
   - Adresse de départ du segment trouvé
   - -1 si aucune zone valide n’est trouvée

2. ## int alloc_es_segment(CPU \*cpu)

   Alloue dynamiquement un segment "ES" (Extra Segment) dans la mémoire du CPU.
   -> Paramètre :

   - CPU \*cpu : structure contenant les registres et la mémoire
     -> Effets :
   - Lit la taille du segment depuis AX et la stratégie depuis BX
   - Crée un segment de taille AX selon la stratégie BX
   - Initialise toutes les cases à 0
   - Met à jour le registre ES avec l’indice de départ du segment
   - Met à jour le drapeau ZF (Zero Flag) à 0 (succès) ou 1 (échec)
     -> Retour :
   - 0 en cas de succès
   - -1 en cas d’erreur (mauvais paramètres, échec d’allocation)

3. ## int free_es_segment(CPU \*cpu)

   Libère le segment "ES" précédemment alloué.
   -> Paramètre :

   - CPU \*cpu : structure du processeur
     -> Effets :
   - Libère la mémoire de toutes les cases du segment ES
   - Supprime l’entrée "ES" dans la table des segments alloués
   - Met le registre ES à -1
     -> Retour :
   - 0 si libération réussie
   - -1 si le segment "ES" est introuvable

4. ## void print_extra_segment(CPU \*cpu)
   Affiche le contenu du segment "ES" (Extra Segment) s’il existe.
   -> Paramètre :
   - CPU \*cpu : structure du processeur
     -> Affichage :
   - Affiche toutes les cases occupées du segment ES
   - Affiche un message si le segment n’est pas alloué
     -> Retour : aucun

## Auteurs

Projet réalisé par **GURENKO Annastasia** et **NOEL Jean-Paul** dans le cadre du projet LU2IN006 - Structures de Données.
