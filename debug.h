#ifndef debug
#define debug

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hachage.h"
#include "memory.h"

void display_hashentry(HashEntry* entry);
void display_hashmap(HashMap* map);
void display_segment(Segment* seg);
void display_segments(Segment* seg);
void display_memory(MemoryHandler* mem);
#endif