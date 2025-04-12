#ifndef cpudef
#define cpudef

#include "memory.h"
#include "parser.h"
#include <stdint.h>
#include <regex.h>


//Cette structure pour simuler le travail du CPU. Un CPU est  composé 
//d’un gestionnaire de mémoire (MemoryHandler) permettant d’allouer
//et de gérer la mémoire du programme et d’une table de hachage (HashMap) permettant de stocker
//les valeurs des registres (emplacements mémoire internes au processeur).
//Pour simplifier, nous ne considérons ici que les registres généraux AX, BX, CX, DX
//En plus on ajout table de hachage pour stocker les valeurs immdiates
typedef struct {
  MemoryHandler* memory_handler; 
  HashMap* context; 
  HashMap* constant_pool;
} CPU;

// Cette fonction initialise une structure CPU.
CPU* cpu_init(int memory_size);
// Cette fonction libère toute la mémoire associée à une structure CPU.
void cpu_destroy(CPU* cpu);
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
void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
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
void* load(MemoryHandler *handler, const char *segment_name, int pos);
void allocate_variables(CPU *cpu, Instruction** data_instructions, int data_count);
int parserString(const char* str, int** tab);
// Cette fonction affiche le contenu du segment de données "DS" dans la mémoire du CPU.
// Elle vérifie d'abord que le CPU et le segment "DS" existent.
// Ensuite, elle parcourt chaque cellule du segment et affiche la valeur entière stockée.
// Si le segment "DS" n'existe pas, elle affiche un message d'erreur.
void print_data_segment(CPU *cpu);
// Cette fonction initialise un objet CPU avec une mémoire de taille donnée.
// Elle retourne un pointeur vers le CPU initialisé ou NULL en cas d'échec.
CPU* init_CPU(int size);
char* trim(char* str);
int search_and_replace(char** str, HashMap* values);
int resolve_constants(ParserResult *result);
void allocate_code_segment(CPU *cpu, Instruction **code_instructions,int code_count);
int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest);
void handle_MOV(CPU* cpu, void* src, void* dest);
void handle_ADD(CPU* cpu, void* src, void* dest);
void handle_CMP(CPU* cpu, void* src, void* dest);
void handle_JMP(CPU* cpu, void* adress);
int execute_instruction(CPU *cpu, Instruction *instr);
Instruction* fetch_next_instruction(CPU *cpu);
int matches(const char* pattern , const char* string);
void* immediate_addressing(CPU *cpu, const char *operand);
void* register_addressing(CPU *cpu, const char *operand);
void *memory_direct_addressing(CPU *cpu, const char *operand);
void* register_indirect_addressing(CPU *cpu, const char *operand);
void* resolve_addressing(CPU* cpu, const char* operand);
#endif