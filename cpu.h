#ifndef cpudef
#define cpudef

#include "memory.h"
#include "parser.h"
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SS_SIZE 128

// Cette structure pour simuler le travail du CPU. Un CPU est  composé
// d’un gestionnaire de mémoire (MemoryHandler) permettant d’allouer
// et de gérer la mémoire du programme et d’une table de hachage (HashMap) permettant de stocker
// les valeurs des registres (emplacements mémoire internes au processeur).
// Pour simplifier, nous ne considérons ici que les registres généraux AX, BX, CX, DX
// En plus on ajout table de hachage pour stocker les valeurs immdiates
typedef struct {
    MemoryHandler *memory_handler;
    HashMap *context;
    HashMap *constant_pool;
} CPU;


#pragma region CPU
    // Cette fonction initialise une structure CPU.
    CPU *cpu_init(int memory_size);
    // Cette fonction libère toute la mémoire associée à une structure CPU.
    void cpu_destroy(CPU *cpu);
    // Cette fonction permet de stocker une donnée dans un segment mémoire géré par MemoryHandler.
    // Elle prend en paramètre :
    //  - un pointeur vers le gestionnaire de mémoire (handler),
    //  - le nom du segment (segment_name),
    //  - une position (pos) relative au début du segment,
    //  - un pointeur vers les données à stocker (data).
    // Elle commence par vérifier que les paramètres sont valides.
    // Ensuite, elle récupère le segment correspondant au nom dans la table de hachage.
    // Si le segment existe et que la position est dans les limites, elle stocke la donnée
    // à l’adresse correspondante dans le tableau `memory` du handler.
    // Elle retourne un pointeur vers la donnée stockée ou NULL en cas d’erreur.
    void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
    // Cette fonction récupère une donnée depuis un segment mémoire à une position donnée.
    // Elle prend en paramètre :
    //  - le gestionnaire de mémoire (handler),
    //  - le nom du segment (segment_name),
    //  - une position relative au début du segment (pos).
    // Elle vérifie que les arguments sont valides,
    // que le segment existe dans la table de hachage,
    // et que la position est dans les bornes autorisées du segment.
    // En cas d’erreur, elle affiche un message explicatif et retourne NULL.
    // Sinon, elle retourne la donnée située à la position spécifiée dans le segment.
    void *load(MemoryHandler *handler, const char *segment_name, int pos);
    // Affiche la valeur des registres généraux (AX, BX, CX, DX)
    // ainsi que des drapeaux de contrôle (IP, ZF, SF).
    // Si un registre n'est pas initialisé, un message spécifique est affiché.
    void print_registres_et_drapeaux(CPU *cpu);
#pragma endregion

#pragma region DATA
    // Cette fonction alloue dynamiquement le segment de données "DS" en fonction des déclarations
    // issues du parser. Pour chaque instruction .DATA, elle extrait les valeurs avec parserString,
    // puis insère les entiers correspondants dans le segment DS.
    void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count);
    // Cette fonction affiche le contenu du segment de données "DS" présent dans la mémoire du CPU.
    // Si le segment n’existe pas, elle affiche un message d’erreur.
    void print_data_segment(CPU *cpu);
    // Cette fonction prend en paramètre une chaîne contenant une liste de valeurs (par exemple "1,2,3").
    // Elle compte le nombre d'entiers, les convertit en tableau, et remplit le tableau passé par adresse.
    // Elle renvoie le nombre total de valeurs trouvées.
    int parserString(const char *str, int **tab);
    // Cette fonction prend une chaîne représentant une liste de valeurs (par exemple "10,20,30").
    // Elle retourne simplement le nombre d’éléments dans la chaîne, séparés par des virgules.
    int parserStringVal(const char *str);
#pragma endregion

#pragma region Addressing
    // Vérifie si une chaîne correspond à un motif regex.
    int matches(const char *pattern, const char *string);
    // Résout un opérande immédiat comme "42".
    // Retourne un pointeur vers un entier.
    void *immediate_addressing(CPU *cpu, const char *operand);
    // Résout un registre comme "AX", "BX".
    // Retourne un pointeur vers la valeur du registre.
    void *register_addressing(CPU *cpu, const char *operand);
    // Résout un accès direct à la mémoire comme "[5]".
    // Accède à DS ou CS selon l'instruction (MOV/ADD ou JMP/JZ...).
    void *memory_direct_addressing(CPU *cpu, const char *operand, const char *mnemonic);
    // Résout un accès indirect par registre comme "[AX]".
    // Retourne la valeur pointée par l'adresse dans le registre.
    void *register_indirect_addressing(CPU *cpu, const char *operand);
    // Tente automatiquement tous les types d’adressage pour un opérande donné.
    // Retourne un pointeur vers la valeur trouvée ou NULL si aucun type valide.
    void *resolve_addressing(CPU *cpu, const char *operand, const char *mnemonic);
    // Cette fonction gère l’adressage explicite de type [SEGMENT:REGISTRE]
    // Elle retourne un pointeur vers la donnée située à l’adresse effective dans le segment donné
    void *segment_override_addressing(CPU *cpu, const char *operand);
#pragma endregion

#pragma region CODE
    // Alloue le segment "CS" et y insère les instructions de code.
    void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count);
    // Exécute une instruction en appelant le gestionnaire adapté (MOV, ADD...).
    int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest);
    // Copie la valeur de la source vers la destination.
    int handle_MOV(CPU *cpu, void *src, void *dest);
    // Additionne src à dest et stocke le résultat dans dest.
    int handle_ADD(CPU *cpu, void *src, void *dest);
    // Compare dest - src et met à jour les drapeaux ZF et SF.
    int handle_CMP(CPU *cpu, void *src, void *dest);
    // Effectue un saut inconditionnel vers une adresse du code.
    int handle_JMP(CPU *cpu, void *adress);
    // Effectue un saut si ZF == 1.
    int handle_JZ(CPU *cpu, void *adress);
    // Effectue un saut si ZF == 0.
    int handle_JNZ(CPU *cpu, void *adress);
    // Arrête l’exécution du programme.
    int handle_HALT(CPU *cpu);
    // Empile la valeur src dans le segment de pile.
    int handle_PUSH(CPU *cpu, void *src);
    // Dépile une valeur et la stocke dans dest.
    int handle_POP(CPU *cpu, void *dest);
    // Gère l'instruction ALLOC : alloue dynamiquement un segment mémoire "ES" (Extra Segment).
    // Retourne 0 en cas de succès, -1 en cas d'échec.
    int handle_ALLOC(CPU *cpu);
    // Gère l'instruction FREE : libère le segment mémoire "ES" précédemment alloué.
    // Retourne 0 en cas de succès, -1 en cas d'échec.
    int handle_FREE(CPU *cpu);
    // Résout les opérandes et exécute une instruction.
    int execute_instruction(CPU *cpu, Instruction *instr);
    // Récupère l’instruction pointée par IP et incrémente IP.
    Instruction *fetch_next_instruction(CPU *cpu);
    // Lance l'exécution complète du programme, instruction par instruction.
    int run_program(CPU *cpu);
    // Affiche la liste d'instructions avec leur mnémotechnique et opérandes.
    void afficher_instructions(Instruction **liste, int count);
#pragma endregion

#pragma region Const
    // Supprime les espaces, tabulations et retours à la ligne au début et à la fin d'une chaîne.
    // Retourne un pointeur vers la version "nettoyée" de la chaîne.
    char *trim(char *str);
    // Cherche et remplace chaque clé (variable ou label) dans la chaîne par sa valeur (index numérique).
    // Utilise la table de hachage 'values' pour retrouver les correspondances.
    // Modifie directement la chaîne passée par adresse.
    // Retourne 1 si un remplacement a été effectué, 0 sinon.
    int search_and_replace(char **str, HashMap *values);
    // Applique le remplacement des variables et des labels dans toutes les instructions .DATA et .CODE.
    // Utilise search_and_replace pour remplacer les noms par leurs adresses ou indices.
    // Retourne -1 en cas d'erreur, sinon 0.
    int resolve_constants(ParserResult *result);
#pragma endregion

#pragma region STACK
    // Empile une valeur entière dans le segment de pile (SS).
    // Met à jour le registre SP (stack pointer) et insère la valeur en mémoire.
    int push_value(CPU *cpu, int value);
    // Dépile la valeur située en haut de la pile et la copie dans le registre destination.
    // Met à jour SP et libère la mémoire allouée.
    int pop_value(CPU *cpu, int *dest);
    // Affiche toutes les valeurs actuellement stockées dans le segment de pile (SS).
    // Indique également les positions des registres SP et BP.
    void print_stack_segment(CPU *cpu);
    // Alloue dynamiquement le segment de pile "SS", juste après le segment de données "DS".
    // Initialise les registres SP et BP à la fin du segment de pile.
    void allocate_stack_segment(CPU *cpu);
#pragma endregion

#pragma region EXTRA
    // Recherche une zone mémoire libre pour un segment, selon la stratégie donnée :
    // 0 = First Fit, 1 = Best Fit, 2 = Worst Fit.
    // Retourne l'adresse de départ du segment trouvé ou -1 en cas d'erreur.
    int find_free_address_strategy(MemoryHandler *handler, int size, int strategy);
    // Alloue dynamiquement un segment "ES" selon la taille contenue dans AX et la stratégie dans BX.
    // Initialise le segment avec des zéros. Met à jour le registre ES et le drapeau ZF.
    // Retourne 0 en cas de succès, -1 en cas d'erreur.
    int alloc_es_segment(CPU *cpu);
    // Libère le segment de mémoire "ES" alloué précédemment.
    // Libère chaque case mémoire, retire le segment de la table et remet ES à -1.
    // Retourne 0 en cas de succès, -1 en cas d'échec.
    int free_es_segment(CPU *cpu);
    // Cette fonction affiche EXTRA segment
    void print_extra_segment(CPU *cpu);
#pragma endregion

#endif