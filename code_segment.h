#ifndef codesegmentdef
#define codesegmentdef

#include "memory.h"
#include "parser.h"
#include "cpu.h"

char *trim(char *str);
int searchandreplace(char **str, HashMap*values);

int resolve_constants(ParserResult *result);

#endif