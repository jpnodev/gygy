#ifndef addressingdef
#define addressingdef

#include "memory.h"
#include "parser.h"
#include "cpu.h"
#include <regex.h>

typedef struct {
  MemoryHandler* memory_handler; 
  HashMap* context; 
  HashMap* constant_pool;
} CPU;

int matches(const char* pattern , const char* string);

void* immediate_addressing(CPU *cpu, const char *operand);
void* register_addressing(CPU *cpu, const char *operand);
void *memory_direct_addressing(CPU *cpu, const char *operand);
void* register_indirect_addressing(CPU *cpu, const char *operand);
void* resolve_addressing(CPU* cpu, const char* operand);
#endif