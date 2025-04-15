#ifndef debug
#define debug

#include "cpu.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Affiche une entrée de table de hachage si elle est valide. Indique si 
// l’entrée est un TOMBSTONE ou NULL.
void display_hashentry(HashEntry *entry);
// Affiche toutes les entrées non nulles d'une table de hachage. 
// Indique aussi le type de données stockées.
void display_hashmap(HashMap *map);
// Affiche un segment mémoire individuel (début, fin, taille). 
// Ne fait rien si le segment est NULL.
void display_segment(Segment *seg);
// Affiche tous les segments d'une liste chaînée. 
// Si la liste est vide, affiche un message spécifique.
void display_segments(Segment *seg);
// Affiche l’état global de la mémoire : taille totale, 
// segments libres, segments alloués. Précise si la mémoire est vide ou pleine.
void display_memory(MemoryHandler *mem);
// Cette fonction affiche une liste d'instructions assembleur.
// Elle prend en paramètre un tableau de pointeurs vers des structures Instruction, ainsi que leur nombre.
// Si la liste est vide ou NULL, elle affiche un message d'avertissement.
// Sinon, elle parcourt chaque instruction et affiche son mnémonique, son opérande 1,
// et son opérande 2 (si ce dernier existe), en les encadrant de guillemets.
void display_instructions(Instruction **liste, int count);
// Affiche le contenu du segment de pile (SS), avec les registres SP et BP. 
// Indique les cases utilisées ou si la pile est vide.
void display_stack_segment(CPU *cpu);
// Affiche le contenu du segment supplémentaire ES. 
// Indique s’il est vide ou non alloué.
void print_extra_segment(CPU *cpu);
#endif