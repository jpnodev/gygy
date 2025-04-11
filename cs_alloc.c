#include "cs_alloc.h"

char* trim(char* str) {
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }

    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    return str;
}

int search_and_replace(char** str, HashMap* values) {
    if (!str || !*str || !values) return 0;

    int replaced = 0;
    char* input = *str;

    for (int i = 0; i < values->size; i++) {
        if (values->table[i].key && values->table[i].key != (void*)-1) {
            char* key = values->table[i].key;
            int value = (int)(long)values->table[i].value;

            char* substr = strstr(input, key);
            if (substr) {
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                int remain_len = strlen(substr + key_len);

                char* new_str = (char*)malloc(strlen(input) - key_len + repl_len + 1);
                if (!new_str) return 0;

                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Nettoyage final (trim)
    if (replaced) {
        char* trimmed = trim(input);
        if (trimmed != input) {
            memmove(input, trimmed, strlen(trimmed) + 1);
        }
    }

    return replaced;
}


int resolve_constants(ParserResult *result) {
    if (result == NULL) {
        printf("La structure est vide\n");
        return -1;
    }

    for (int i = 0; i < result->data_count; i++) {
        Instruction* instr = result->data_instructions[i];
        if (instr->operand1)
            search_and_replace(&(instr->operand1), result->memory_locations);
        if (instr->operand2)
            search_and_replace(&(instr->operand2), result->memory_locations);
    }

    for (int j = 0; j < result->code_count; j++) {
        Instruction* instr = result->code_instructions[j];
        if (instr->operand1)
            search_and_replace(&(instr->operand1), result->labels);
        if (instr->operand2)
            search_and_replace(&(instr->operand2), result->labels);
    }

    return 0;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions,int code_count) {
    if (cpu == NULL || code_instructions || code_count < 0) {
        printf("Erreur : argument invalide (cpu ou code_instructions est NULL).\n");
        return;
    }

    for (int i = 0; i < code_count; i++) {
        
    }
}
