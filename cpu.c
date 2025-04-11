#include "cpu.h"

CPU* cpu_init(int memory_size) {
  CPU* cpu = (CPU*) malloc(sizeof(CPU));
  if (cpu == NULL) {
    return NULL;
  }
  
  cpu->memory_handler = memory_init(memory_size);
  if (cpu->memory_handler == NULL) {
    free(cpu);
    return NULL;
  }

  cpu->context = hashmap_create();
  if (cpu->context == NULL) {
    free(cpu->memory_handler);
    free(cpu);
    return NULL;
  }

  cpu->constant_pool = hashmap_create();
  if (cpu->constant_pool == NULL) {
    hashmap_destroy(cpu->context);
    memory_destroy(cpu->memory_handler);
    free(cpu);
    return NULL;
  }
  int* IP = malloc(sizeof(int));
  if (IP == NULL) {
    hashmap_destroy(cpu->context);
    memory_destroy(cpu->memory_handler);
    hashmap_destroy(cpu->constant_pool);
    return NULL;
  }
  int* ZF = malloc(sizeof(int));
  if (ZF == NULL) {
    hashmap_destroy(cpu->context);
    memory_destroy(cpu->memory_handler);
    hashmap_destroy(cpu->constant_pool);
    free(IP);
    return NULL;
  }
  int* SF = malloc(sizeof(int));
  if (SF == NULL) {
    hashmap_destroy(cpu->context);
    memory_destroy(cpu->memory_handler);
    hashmap_destroy(cpu->constant_pool);
    free(IP);
    free(ZF);
    return NULL;
  }
  *IP = 0;
  *ZF = 0;
  *SF = 0;
  hashmap_insert(cpu->constant_pool, "IP", IP);
  hashmap_insert(cpu->constant_pool, "ZF", ZF);
  hashmap_insert(cpu->constant_pool, "SF", SF);
  return cpu;
}

void cpu_destroy(CPU* cpu) {
    if (cpu == NULL) return;

    // int* IP = hashmap_get(cpu->constant_pool, "IP");
    // int* ZF = hashmap_get(cpu->constant_pool, "ZF");
    // int* SF = hashmap_get(cpu->constant_pool, "SF");

    // if (IP) free(IP);
    // if (ZF) free(ZF);
    // if (SF) free(SF);

    hashmap_destroy(cpu->constant_pool);
    hashmap_destroy(cpu->context);
    memory_destroy(cpu->memory_handler);
    free(cpu);
}


void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data) {
  if (segment_name == NULL || handler == NULL || data == NULL) {
    printf("Erreur : argument invalide (segment_name, handler ou data est NULL).\n");
    return NULL;
  }

  Segment* s = hashmap_get(handler->allocated, segment_name);
  if (s == NULL) {
    printf("Erreur : segment \"%s\" introuvable dans la mémoire allouée.\n", segment_name);
    return NULL;
  } 

  if (pos < 0 || pos >= s->size) {
    printf("Erreur : position %d est hors des bornes du segment \"%s\" (taille %d).\n", pos, segment_name, s->size);
    return NULL;
  }
  
  handler->memory[s->start + pos] = data;
  return handler->memory[s->start + pos];
}


void* load(MemoryHandler *handler, const char *segment_name, int pos) {
  if (segment_name == NULL || handler == NULL) {
      printf("Erreur : argument invalide (segment_name ou handler est NULL).\n");
      return NULL;
  }
  Segment* s = hashmap_get(handler->allocated, segment_name);
  if (s == NULL) {
      printf("Erreur : segment \"%s\" introuvable dans la mémoire allouée.\n", segment_name);
      return NULL;
  }
  if (pos < 0 || pos >= s->size) {
    printf("Erreur : position %d est hors des bornes du segment \"%s\" (taille %d).\n", pos, segment_name, s->size);
    return NULL;
  }
      
  return handler->memory[s->start + pos];
}

int parserString(const char* str, int** tab) {
    if (str == NULL) return -1;

    char* copy = strdup(str);
    if (!copy) return -1;

    int count = 1;
    for (int i = 0; copy[i]; i++) {
        if (copy[i] == ',') count++;
    }

    *tab = malloc(sizeof(int) * count);
    if (!*tab) {
        free(copy);
        return -1;
    }

    char* token = strtok(copy, ",");
    int i = 0;
    while (token != NULL && i < count) {
        (*tab)[i++] = atoi(token);
        token = strtok(NULL, ",");
    }

    free(copy);
    return count;
}


void allocate_variables(CPU *cpu, Instruction** data_instructions, int data_count) {
    if (cpu == NULL || data_instructions == NULL || *data_instructions == NULL || data_count <= 0) {
        printf("Erreur : argument invalide.\n");
        return;
    }

    static int pos = 0; 
    int total_values = 0;

    for (int i = 0; i < data_count; i++) {
        int* temp_tab;
        int k = parserString(data_instructions[i]->operand2, &temp_tab);
        if (k > 0) {
            total_values += k;
            free(temp_tab);
        }
    }

    int r = create_segment(cpu->memory_handler, "DS", pos, total_values);
    if (r <= 0) return;

    int current_pos = pos;

    // Счётчик для отслеживания уже выделенных значений
    int values_written = 0;
    int** allocated_vals = malloc(sizeof(int*) * total_values);

    for (int i = 0; i < data_count; i++) {
        int* tab;
        int k = parserString(data_instructions[i]->operand2, &tab);
        if (k <= 0 || tab == NULL) continue;

        for (int j = 0; j < k; j++) {
            int* val = malloc(sizeof(int));
            if (val == NULL) {
                perror("Erreur d'allocation pour val");

                // Очистка ранее выделенных значений
                for (int x = 0; x < values_written; x++) {
                    free(allocated_vals[x]);
                }
                free(allocated_vals);
                free(tab);
                remove_segment(cpu->memory_handler, "DS");
                return;
            }

            *val = tab[j];

            if (store(cpu->memory_handler, "DS", current_pos - pos, val) == NULL) {
                free(val);
                for (int x = 0; x < values_written; x++) {
                    free(allocated_vals[x]);
                }
                free(allocated_vals);
                free(tab);
                remove_segment(cpu->memory_handler, "DS");
                return;
            }

            allocated_vals[values_written++] = val;
            current_pos++;
        }

        free(tab);
    }

    free(allocated_vals);
    pos += total_values;
}



void print_data_segment(CPU *cpu) {
    if (cpu == NULL) return;

    Segment* s = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (s == NULL) {
        printf("Pas de segment 'DS' dans la mémoire.\n");
        return;
    }

    printf("Contenu du segment de données 'DS' :\n");
    for (int i = 0; i < s->size; i++) {
        int index = s->start + i;
        int* valeur = (int*)(cpu->memory_handler->memory[index]);

        if (valeur != NULL) {
            printf("DS[%d] = %d\n", i, *valeur);
        } else {
            printf("DS[%d] = (vide)\n", i);
        }
    }
}

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

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count) {
    if (cpu == NULL || code_instructions == NULL || code_count < 0) {
        printf("Erreur : argument invalide (cpu ou code_instructions est NULL).\n");
        return;
    }

    int pos = getSegFreePos(cpu->memory_handler, code_count);
    if (pos == -1) {
        printf("Pas assez de mémoire pour initialiser le segment de code\n");
        return;
    }

    int r = create_segment(cpu->memory_handler, "CS", pos, code_count);
    if (r <= 0) {
        printf("Erreur lors de la création du segment de code\n");
        return;
    }

    for (int i = 0; i < code_count; i++) {
        if (store(cpu->memory_handler, "CS", i, code_instructions[i]) == NULL) {
            printf("Erreur lors du stockage de l'instruction %d\n", i);
            remove_segment(cpu->memory_handler, "CS");
            return;
        }
    }

    int* IP = hashmap_get(cpu->constant_pool, "IP");
    if (IP != NULL) {
        *IP = pos; 
    } else {
        printf("Erreur : registre IP introuvable\n");
    }
}
void handle_MOV(CPU* cpu, void* src, void* dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    *(int*)dest = *(int*)src;
}

/*void handle_ADD(CPU* cpu, void* src, void* dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    *(int*)dest = (*(int*)src) + (*(int*)dest);
}

void handle_CMP(CPU* cpu, void* src, void* dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    int res = (*(int*)dest) - (*(int*)src);
    
    if (res == 0) {
        int* ZF = (int*)hashmap_get(cpu -> context, "ZF");
        *ZF = 1;
    } else if (res < 0) {
        int* SF = (int*)hashmap_get(cpu -> context, "ZF");
        *SF = 1;    
    }
}

void handle_JMP(CPU* cpu, void* adress) {
    if (cpu == NULL || adress == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    int* IP = (int*)hashmap_get(cpu -> context, "IP");
    *IP = adress;
}




int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest) {
  if (cpu == NULL || instr == NULL || src == NULL || dest == NULL) {
    printf("Erreur : argument invalide.\n");
    return -1;
  }

  if (strncmp(instr -> mnemonic, "MOV", 3) == 0) {
    handle_MOV(cpu, src, dest);
  } else if (strncmp(instr -> mnemonic, "ADD", 3) == 0) {
    handle_ADD(cpu, src, dest);
  } else if (strncmp(instr -> mnemonic, "CMP", 3) == 0) {
    handle_CMP(cpu, src, dest);
  } else if (strncmp(instr -> mnemonic, "JMP", 3) == 0) {
    handle_JMP(cpu, ?);
  } else if (strncmp(instr -> mnemonic, "JZ", 2) == 0) {

  } else if (strncmp(instr -> mnemonic, "JNZ", 3) == 0) {

  }
} 




