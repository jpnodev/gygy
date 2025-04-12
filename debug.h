#ifndef debug
#define debug

#include "hachage.h"
#include "memory.h"
#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void display_hashentry(HashEntry *entry);
void display_hashmap(HashMap *map);
void display_segment(Segment *seg);
void display_segments(Segment *seg);
void display_memory(MemoryHandler *mem);
// Cette fonction affiche une liste d'instructions assembleur.
// Elle prend en paramètre un tableau de pointeurs vers des structures Instruction, ainsi que leur nombre.
// Si la liste est vide ou NULL, elle affiche un message d'avertissement.
// Sinon, elle parcourt chaque instruction et affiche son mnémonique, son opérande 1,
// et son opérande 2 (si ce dernier existe), en les encadrant de guillemets.
void display_instructions(Instruction **liste, int count);
#endif