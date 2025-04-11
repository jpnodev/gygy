#ifndef cs_allocdef
#define cs_allocdef

#include "memory.h"
#include "parser.h"
#include "cpu.h"
#include "hachage.h"

char* trim(char* str);
int search_and_replace(char** str, HashMap* values);
int resolve_constants(ParserResult *result);
void allocate_code_segment(CPU *cpu, Instruction **code_instructions,int code_count);

#endif