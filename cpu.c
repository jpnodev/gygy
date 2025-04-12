#include "cpu.h"

CPU *cpu_init(int memory_size) {
    if (memory_size <= 0) {
        printf("Erreur : taille de mémoire invalide.\n");
        return NULL;
    }

    // Initialiser le CPU
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    if (cpu == NULL) {
        printf("Erreur : échec de l'allocation de mémoire pour le CPU.\n");
        return NULL;
    }

    // Initialiser la mémoire
    cpu->memory_handler = memory_init(memory_size);
    if (cpu->memory_handler == NULL) {
        printf("Erreur : échec de l'initialisation de la mémoire.\n");
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    // Initialiser les registres
    int *ax = malloc(sizeof(int));
    int *bx = malloc(sizeof(int));
    int *cx = malloc(sizeof(int));
    int *dx = malloc(sizeof(int));
    int *ip = malloc(sizeof(int));
    int *zf = malloc(sizeof(int));
    int *sf = malloc(sizeof(int));
    int *sp = malloc(sizeof(int));
    int *bp = malloc(sizeof(int));

    if (ax == NULL || bx == NULL || cx == NULL || dx == NULL || ip == NULL || zf == NULL || sf == NULL || sp == NULL ||
        bp == NULL) {
        printf("Erreur : échec de l'allocation de mémoire pour les registres.\n");
        // Libérer la mémoire allouée pour les registres
        if (ax != NULL)
            free(ax);
        if (bx != NULL)
            free(bx);
        if (cx != NULL)
            free(cx);
        if (dx != NULL)
            free(dx);
        if (ip != NULL)
            free(ip);
        if (zf != NULL)
            free(zf);
        if (sf != NULL)
            free(sf);
        if (sp != NULL)
            free(sp);
        if (bp != NULL)
            free(bp);
        // Libérer la mémoire allouée pour le memory_handler
        memory_destroy(cpu->memory_handler);
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    // Initialiser les registres à 0
    *ax = 0;
    *bx = 0;
    *cx = 0;
    *dx = 0;
    *ip = 0;
    *zf = 0;
    *sf = 0;
    *sp = 0;
    *bp = 0;

    // Initialiser la table de hachage pour le contexte
    cpu->context = hashmap_create(BASIC_MALLOC);
    if (cpu->context == NULL) {
        printf("Erreur : échec de la création de la table de hachage.\n");
        // Libérer la mémoire allouée pour les registres
        free(ax);
        free(bx);
        free(cx);
        free(dx);
        free(ip);
        free(zf);
        free(sf);
        free(sp);
        free(bp);
        // Libérer la mémoire allouée pour le memory_handler
        memory_destroy(cpu->memory_handler);
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    // Insérer les registres dans la table de hachage
    if (hashmap_insert(cpu->context, "AX", ax) == -1 || hashmap_insert(cpu->context, "BX", bx) == -1 ||
        hashmap_insert(cpu->context, "CX", cx) == -1 || hashmap_insert(cpu->context, "DX", dx) == -1 ||
        hashmap_insert(cpu->context, "IP", ip) == -1 || hashmap_insert(cpu->context, "ZF", zf) == -1 ||
        hashmap_insert(cpu->context, "SF", sf) == -1 || hashmap_insert(cpu->context, "SP", sp) == -1 ||
        hashmap_insert(cpu->context, "BP", bp) == -1) {
        printf("Erreur : échec de l'insertion des registres dans la table de hachage.\n");
        // Libérer la mémoire allouée pour les registres
        free(ax);
        free(bx);
        free(cx);
        free(dx);
        free(ip);
        free(zf);
        free(sf);
        free(sp);
        free(bp);
        // Libérer la mémoire allouée pour le memory_handler
        memory_destroy(cpu->memory_handler);
        // Libérer la mémoire allouée pour le contexte
        hashmap_destroy(cpu->context);
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    // Initialiser la table de hachage pour le pool constant
    cpu->constant_pool = hashmap_create(BASIC_MALLOC);
    if (cpu->constant_pool == NULL) {
        printf("Erreur : échec de la création de la table de hachage pour le pool constant.\n");
        // Libérer la mémoire allouée pour les registres
        free(ax);
        free(bx);
        free(cx);
        free(dx);
        free(ip);
        free(zf);
        free(sf);
        free(sp);
        free(bp);
        // Libérer la mémoire allouée pour le memory_handler
        memory_destroy(cpu->memory_handler);
        // Libérer la mémoire allouée pour le contexte
        hashmap_destroy(cpu->context);
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    // Initialise le Stack Segment
    //@todo comment le placer sous le segment data si DS n'est pas initialisé ici ?
    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    int start = 0;
    int res_ss = -1;
    if (ds != NULL) {
        start = ds->start + ds->size;
        return NULL;
    } else if ((start = getSegFreePos(cpu->memory_handler, 128)) != -1) {
        res_ss = create_segment(cpu->memory_handler, "SS", start, 128);
    }
    if ((res_ss != 1) || (start == -1)) {
        printf("Erreur : échec de la création du segment de pile.\n");
        // Libérer la mémoire allouée pour les registres
        free(ax);
        free(bx);
        free(cx);
        free(dx);
        free(ip);
        free(zf);
        free(sf);
        free(sp);
        free(bp);
        // Libérer la mémoire allouée pour le memory_handler
        memory_destroy(cpu->memory_handler);
        // Libérer la mémoire allouée pour le contexte
        hashmap_destroy(cpu->context);
        // Libérer la mémoire allouée pour le pool constant
        hashmap_destroy(cpu->constant_pool);
        // Libérer la mémoire allouée pour le CPU
        free(cpu);
        return NULL;
    }

    *sp = ds->start + ds->size - 1;
    *bp = ds->start + ds->size - 1;

    return cpu;
}

void cpu_destroy(CPU *cpu) {
    if (cpu == NULL)
        return;

    if (cpu->memory_handler != NULL) {
        memory_destroy(cpu->memory_handler);
    }
    if (cpu->context != NULL) {
        int *ax = (int *)hashmap_get(cpu->context, "AX");
        int *bx = (int *)hashmap_get(cpu->context, "BX");
        int *cx = (int *)hashmap_get(cpu->context, "CX");
        int *dx = (int *)hashmap_get(cpu->context, "DX");
        int *ip = (int *)hashmap_get(cpu->context, "IP");
        int *zf = (int *)hashmap_get(cpu->context, "ZF");
        int *sf = (int *)hashmap_get(cpu->context, "SF");
        int *sp = (int *)hashmap_get(cpu->context, "SP");
        int *bp = (int *)hashmap_get(cpu->context, "BP");
        if (ax != NULL)
            free(ax);
        if (bx != NULL)
            free(bx);
        if (cx != NULL)
            free(cx);
        if (dx != NULL)
            free(dx);
        if (ip != NULL)
            free(ip);
        if (zf != NULL)
            free(zf);
        if (sf != NULL)
            free(sf);
        if (sp != NULL)
            free(sp);
        if (bp != NULL)
            free(bp);
        hashmap_destroy(cpu->context);
    }
    if (cpu->constant_pool != NULL) {
        hashmap_destroy(cpu->constant_pool);
    }
    free(cpu);
}

void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data) {
    if (segment_name == NULL || handler == NULL || data == NULL) {
        printf("Erreur : argument invalide (segment_name, handler ou data est "
               "NULL).\n");
        return NULL;
    }

    Segment *s = hashmap_get(handler->allocated, segment_name);
    if (s == NULL) {
        printf("Erreur : segment \"%s\" introuvable dans la mémoire allouée.\n", segment_name);
        return NULL;
    }

    if (pos < 0 || pos >= s->size) {
        printf("Erreur : position %d est hors des bornes du segment \"%s\" "
               "(taille "
               "%d).\n",
               pos, segment_name, s->size);
        return NULL;
    }

    handler->memory[s->start + pos] = data;
    return handler->memory[s->start + pos];
}

void *load(MemoryHandler *handler, const char *segment_name, int pos) {
    if (segment_name == NULL || handler == NULL) {
        printf("Erreur : argument invalide (segment_name ou handler est NULL).\n");
        return NULL;
    }
    Segment *s = hashmap_get(handler->allocated, segment_name);
    if (s == NULL) {
        printf("Erreur : segment \"%s\" introuvable dans la mémoire allouée.\n", segment_name);
        return NULL;
    }
    if (pos < 0 || pos >= s->size) {
        printf("Erreur : position %d est hors des bornes du segment \"%s\" "
               "(taille "
               "%d).\n",
               pos, segment_name, s->size);
        return NULL;
    }

    return handler->memory[s->start + pos];
}

int parserString(const char *str, int **tab) {
    if (str == NULL)
        return -1;

    char *copy = strdup(str);
    if (!copy)
        return -1;

    int count = 1;
    for (int i = 0; copy[i]; i++) {
        if (copy[i] == ',')
            count++;
    }

    *tab = malloc(sizeof(int) * count);
    if (!*tab) {
        free(copy);
        return -1;
    }

    char *token = strtok(copy, ",");
    int i = 0;
    while (token != NULL && i < count) {
        (*tab)[i++] = atoi(token);
        token = strtok(NULL, ",");
    }

    free(copy);
    return count;
}

void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count) {
    if (cpu == NULL || data_instructions == NULL || *data_instructions == NULL || data_count <= 0) {
        printf("Erreur : argument invalide.\n");
        return;
    }

    static int pos = 0;
    int total_values = 0;

    for (int i = 0; i < data_count; i++) {
        int *temp_tab;
        int k = parserString(data_instructions[i]->operand2, &temp_tab);
        if (k > 0) {
            total_values += k;
            free(temp_tab);
        }
    }

    int r = create_segment(cpu->memory_handler, "DS", pos, total_values);
    if (r <= 0)
        return;

    int current_pos = pos;

    // Счётчик для отслеживания уже выделенных значений
    // Compteur pour suivre le nombre de valeurs déjà allouées
    int values_written = 0;
    int **allocated_vals = malloc(sizeof(int *) * total_values);

    for (int i = 0; i < data_count; i++) {
        int *tab;
        int k = parserString(data_instructions[i]->operand2, &tab);
        if (k <= 0 || tab == NULL)
            continue;

        for (int j = 0; j < k; j++) {
            int *val = malloc(sizeof(int));
            if (val == NULL) {
                perror("Erreur d'allocation pour val");

                // Очистка ранее выделенных значений
                // Libération des valeurs déjà allouées
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
    if (cpu == NULL)
        return;

    Segment *s = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (s == NULL) {
        printf("Pas de segment 'DS' dans la mémoire.\n");
        return;
    }

    printf("Contenu du segment de données 'DS' :\n");
    for (int i = 0; i < s->size; i++) {
        int index = s->start + i;
        int *valeur = (int *)(cpu->memory_handler->memory[index]);

        if (valeur != NULL) {
            printf("DS[%d] = %d\n", i, *valeur);
        } else {
            printf("DS[%d] = (vide)\n", i);
        }
    }
}

char *trim(char *str) {
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')
        str++;

    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
    return str;
}

int search_and_replace(char **str, HashMap *values) {
    if (!str || !*str || !values)
        return 0;

    int replaced = 0;
    char *input = *str;

    // Iterate through all keys in the hashmap
    for (int i = 0; i < values->size; i++) {
        if (values->table[i].key && values->table[i].key != (void *)-1) {
            char *key = values->table[i].key;
            int value = (int)(long)values->table[i].value;

            // Find potential substring match
            char *substr = strstr(input, key);
            if (substr) {
                // Construct replacement buffer
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

                // Calculate lengths
                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                int remain_len = strlen(substr + key_len);

                // Create new string
                char *new_str = (char *)malloc(strlen(input) - key_len + repl_len + 1);
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                // Free and update original string
                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Trim the final string
    if (replaced) {
        char *trimmed = trim(input);
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
        Instruction *instr = result->data_instructions[i];
        if (instr->operand1)
            search_and_replace(&(instr->operand1), result->memory_locations);
        if (instr->operand2)
            search_and_replace(&(instr->operand2), result->memory_locations);
    }

    for (int j = 0; j < result->code_count; j++) {
        Instruction *instr = result->code_instructions[j];
        if (instr->operand1)
            search_and_replace(&(instr->operand1), result->labels);
        if (instr->operand2)
            search_and_replace(&(instr->operand2), result->labels);
    }

    return 0;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count) {
    if (cpu == NULL || code_instructions == NULL || code_count < 0) {
        printf("Erreur : argument invalide (cpu ou code_instructions est "
               "NULL).\n");
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

    int *IP = hashmap_get(cpu->context, "IP");
    if (IP != NULL) {
        *IP = pos;
    } else {
        printf("Erreur : registre IP introuvable\n");
    }
}

void handle_MOV(CPU *cpu, void *src, void *dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    *(int *)dest = *(int *)src;
}

void handle_ADD(CPU *cpu, void *src, void *dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    *(int *)dest = (*(int *)src) + (*(int *)dest);
}

void handle_CMP(CPU *cpu, void *src, void *dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    int res = (*(int *)dest) - (*(int *)src);

    if (res == 0) {
        int *ZF = (int *)hashmap_get(cpu->context, "ZF");
        *ZF = 1;
    } else if (res < 0) {
        int *SF = (int *)hashmap_get(cpu->context, "SF");
        *SF = 1;
    }
}

void handle_JMP(CPU *cpu, void *adress) {
    if (cpu == NULL || adress == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    intptr_t *IP = (intptr_t *)hashmap_get(cpu->context, "IP");
    *IP = *(intptr_t *)adress;
}

void handle_JZ(CPU *cpu, void *adress) {
    if (cpu == NULL || adress == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    int *ZF = (int *)hashmap_get(cpu->context, "ZF");
    intptr_t *IP = (intptr_t *)hashmap_get(cpu->context, "IP");
    if (*ZF == 1) {
        *IP = *(intptr_t *)adress;
    }
}

void handle_JNZ(CPU *cpu, void *adress) {
    if (cpu == NULL || adress == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }
    int *ZF = (int *)hashmap_get(cpu->context, "ZF");
    intptr_t *IP = (intptr_t *)hashmap_get(cpu->context, "IP");
    if (*ZF == 0) {
        *IP = *(intptr_t *)adress;
    }
}

void handle_HALT(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return;
    }

    int *IP = (int *)hashmap_get(cpu->context, "IP");
    Segment *CS = (Segment *)hashmap_get(cpu->memory_handler->allocated, "CS");
    if (CS == NULL || IP == NULL) {
        printf("Erreur : segment de codes ou IP n'est pas initialisé.\n");
        return;
    }

    *IP = CS->start + CS->size;
}

void handle_HALT(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return;
    }

    intptr_t *IP = (intptr_t *)hashmap_get(cpu->context, "IP");
    Segment *CS = (Segment *)hashmap_get(cpu->memory_handler->allocated, "CS");
    if (CS == NULL || IP == NULL) {
        printf("Erreur : segment de codes ou IP n'est pas initialisé.\n");
        return;
    }

    *IP = (intptr_t)((char *)(CS->start) + CS->size);
}

void handle_PUSH(CPU *cpu, void *src) {

    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    // Si la source n'est pas précisée, on utilise le registre ax
    if (src == NULL) {
        src = hashmap_get(cpu->context, "AX");
        if (src == NULL) {
            printf("Erreur : registre AX introuvable.\n");
            return;
        }
    }

    int res = push_value(cpu->memory_handler, *(int *)src);
    if (res == -1) {
        printf("Erreur : échec de l'empilement de la valeur.\n");
        return;
    }
}

void handle_POP(CPU *cpu, void *dest) {

    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    // Si la destination n'est pas précisée, on utilise le registre ax
    if (dest == NULL) {
        dest = hashmap_get(cpu->context, "AX");
        if (dest == NULL) {
            printf("Erreur : registre AX introuvable.\n");
            return;
        }
    }

    int res = push_value(cpu->memory_handler, *(int *)dest);
    if (res == -1) {
        printf("Erreur : échec de l'empilement de la valeur.\n");
        return;
    }
}

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest) {
    if (cpu == NULL || instr == NULL) {
        printf("Erreur : argument invalide.\n");
        return -1;
    }

    if (strncmp(instr->mnemonic, "MOV", 3) == 0) {
        if (src == NULL || dest == NULL) {
            printf("Erreur : MOV nécessite deux arguments.\n");
            return -1;
        }
        handle_MOV(cpu, src, dest);

    } else if (strncmp(instr->mnemonic, "ADD", 3) == 0) {
        if (src == NULL || dest == NULL) {
            printf("Erreur : ADD nécessite deux arguments.\n");
            return -1;
        }
        handle_ADD(cpu, src, dest);

    } else if (strncmp(instr->mnemonic, "CMP", 3) == 0) {
        if (src == NULL || dest == NULL) {
            printf("Erreur : CMP nécessite deux arguments.\n");
            return -1;
        }
        handle_CMP(cpu, src, dest);

    } else if (strncmp(instr->mnemonic, "JMP", 3) == 0) {
        if (src == NULL) {
            printf("Erreur : JMP nécessite une adresse (src).\n");
            return -1;
        }
        handle_JMP(cpu, src);

    } else if (strncmp(instr->mnemonic, "JZ", 2) == 0) {
        if (src == NULL) {
            printf("Erreur : JZ nécessite une adresse (src).\n");
            return -1;
        }
        handle_JZ(cpu, src);

    } else if (strncmp(instr->mnemonic, "JNZ", 3) == 0) {
        if (src == NULL) {
            printf("Erreur : JNZ nécessite une adresse (src).\n");
            return -1;
        }
        handle_JNZ(cpu, src);

    } else if (strncmp(instr->mnemonic, "HALT", 4) == 0) {
        handle_HALT(cpu);

    } else if (strncmp(instr->mnemonic, "PUSH", 4) == 0) {
        // Ici, src (operand 1) est la source à empiler
        // dest (operand 2) n'est pas utilisé
        handle_PUSH(cpu, src);

    } else if (strncmp(instr->mnemonic, "POP", 3) == 0) {
        // Ici, src (operand 1) est la destination où la valeur sera dépilée
        // dest (operand 2) n'est pas utilisé
        handle_POP(cpu, src);

    } else {
        printf("Erreur : instruction \"%s\" non reconnue.\n", instr->mnemonic);
        return -1;
    }

    return 0;
}

int execute_instruction(CPU *cpu, Instruction *instr) {
    if (cpu == NULL || instr == NULL) {
        printf("Erreur : argument invalide.\n");
        return -1;
    }

    void *src = NULL;
    void *dest = NULL;

    if (instr->operand1 != NULL) {
        src = resolve_addressing(cpu, instr->operand1);
    }

    if (instr->operand2 != NULL) {
        dest = resolve_addressing(cpu, instr->operand2);
    }

    return handle_instruction(cpu, instr, src, dest);
}

Instruction *fetch_next_instruction(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (CPU est NULL).\n");
        return NULL;
    }

    int *IP = (int *)hashmap_get(cpu->context, "IP");
    Segment *CS = (Segment *)hashmap_get(cpu->context, "CS");

    if (IP == NULL || CS == NULL) {
        printf("Erreur : IP ou CS non initialisé.\n");
        return NULL;
    }

    int start = (int)((intptr_t)(CS->start));
    int end = start + CS->size;

    if (*IP < start || *IP >= end) {
        printf("Erreur : IP (%d) hors des limites valides [%d, %d).\n", *IP, start, end);
        return NULL;
    }

    Instruction *instr = (Instruction *)(cpu->memory_handler->memory[*IP]);
    (*IP)++;

    return instr;
}

int run_program(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (CPU est NULL).\n");
        return -1;
    }

    printf("=== ÉTAT INITIAL ===\n");
    print_data_segment(cpu);
    print_registres_et_drapeaux(cpu);

    while (1) {
        Instruction *inst = fetch_next_instruction(cpu);
        if (inst == NULL) {
            printf("Fin du programme ou erreur de récupération d’instruction.\n");
            break;
        }

        printf("\nInstruction à exécuter : %s %s %s\n", inst->mnemonic, inst->operand1 ? inst->operand1 : "",
               inst->operand2 ? inst->operand2 : "");

        printf("Appuyez sur Entrée pour exécuter l’instruction ou tapez 'q' pour quitter : ");

        char input[10];
        fgets(input, sizeof(input), stdin);

        if (strncmp(input, "q", 1) == 0) {
            printf("Exécution interrompue par l’utilisateur.\n");
            break;
        }

        if (execute_instruction(cpu, inst) == -1) {
            printf("Erreur lors de l’exécution de l’instruction.\n");
            break;
        }

        print_registres_et_drapeaux(cpu);
    }

    printf("\n=== ÉTAT FINAL ===\n");
    print_data_segment(cpu);
    print_registres_et_drapeaux(cpu);

    return 0;
}

void print_registres_et_drapeaux(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (CPU est NULL).\n");
        return;
    }

    int *AX = (int *)hashmap_get(cpu->context, "AX");
    printf("Registre AX: %s%d\n", AX ? "" : "vide ", AX ? *AX : 0);

    int *BX = (int *)hashmap_get(cpu->context, "BX");
    printf("Registre BX: %s%d\n", BX ? "" : "vide ", BX ? *BX : 0);

    int *CX = (int *)hashmap_get(cpu->context, "CX");
    printf("Registre CX: %s%d\n", CX ? "" : "vide ", CX ? *CX : 0);

    int *DX = (int *)hashmap_get(cpu->context, "DX");
    printf("Registre DX: %s%d\n", DX ? "" : "vide ", DX ? *DX : 0);

    // Affichage des drapeaux
    int *IP = (int *)hashmap_get(cpu->context, "IP");
    printf("Drapeau IP: %s%d\n", IP ? "" : "non initialisé ", IP ? *IP : 0);

    int *ZF = (int *)hashmap_get(cpu->context, "ZF");
    printf("Drapeau ZF: %s%d\n", ZF ? "" : "non initialisé ", ZF ? *ZF : 0);

    int *SF = (int *)hashmap_get(cpu->context, "SF");
    printf("Drapeau SF: %s%d\n", SF ? "" : "non initialisé ", SF ? *SF : 0);
}

int matches(const char *pattern, const char *string) {
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

void *immediate_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^-?[0-9]+$", operand)) {
        int *valeur = (int *)hashmap_get(cpu->constant_pool, operand);
        if (valeur == NULL) {
            valeur = (int *)malloc(sizeof(int));
            if (valeur == NULL) {
                perror("Erreur d'allocation pour valeur");
                return NULL;
            }
            char *endptr;
            *valeur = (int)strtol(operand, &endptr, 10);
            if (*endptr != '\0') {
                printf("Erreur : operand n'est pas un entier valide : %s\n", operand);
                free(valeur);
                return NULL;
            }
            hashmap_insert(cpu->constant_pool, operand, valeur);
        }
        return valeur;
    } else {
        printf("Cette commande n'est pas un adressage immédiat : %s\n", operand);
        return NULL;
    }
    return NULL;
}

void *register_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^(A|B|C|D)X$", operand)) {
        int *valeur = (int *)hashmap_get(cpu->context, operand);
        if (valeur == NULL) {
            printf("Registre %s est vide ou non initialisé.\n", operand);
            return NULL;
        }
        return valeur;
    } else {
        printf("Cette commande n'est pas un adressage par registre : %s\n", operand);
        return NULL;
    }
}

void *memory_direct_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^\\[-?[0-9]+\\]$", operand)) {
        int address;
        if (sscanf(operand, "[%d]", &address) != 1) {
            printf("Erreur : extraction de l'adresse a échoué : %s\n", operand);
            return NULL;
        }

        int cell_count = cpu->memory_handler->total_size / sizeof(void *);
        if (address < 0 || address >= cell_count) {
            printf("Hors limites mémoire ! Adresse : %d (max : %d)\n", address, cell_count - 1);
            return NULL;
        }

        void *valeur = cpu->memory_handler->memory[address];
        if (valeur == NULL) {
            printf("Erreur : aucune donnée à l'adresse mémoire %d.\n", address);
            return NULL;
        }

        return valeur;
    } else {
        printf("Cette commande n'est pas un adressage par registre : %s\n", operand);
        return NULL;
    }
}

void *register_indirect_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^\\[(A|B|C|D)X\\]$", operand)) {
        char registre[10];
        size_t len = strlen(operand);
        strncpy(registre, operand + 1, len - 2);
        registre[len - 2] = '\0';
        return register_addressing(cpu, registre);
    } else {
        printf("Cette commande n'est pas un adressage indirect par registre : %s\n", operand);
        return NULL;
    }
}

void *resolve_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return NULL;
    }
    if (operand == NULL) {
        printf("Erreur : argument invalide (operand est NULL).\n");
        return NULL;
    }

    void *result;

    // On essaye d'abord l'adressage immédiat
    result = immediate_addressing(cpu, operand);
    if (result != NULL) {
        return result;
    }

    // Ensuite, on essaye l'adressage par registre
    result = register_addressing(cpu, operand);
    if (result != NULL) {
        return result;
    }

    // Adressage direct mémoire
    result = memory_direct_addressing(cpu, operand);
    if (result != NULL) {
        return result;
    }

    // Adressage indirect par registre
    result = register_indirect_addressing(cpu, operand);
    if (result != NULL) {
        return result;
    }

    // Aucun adressage n'a fonctionné
    printf("Erreur : aucun mode d'adressage valide trouvé pour l'opérande : %s\n", operand);
    return NULL;
}

int push_value(CPU *cpu, int value) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return -1;
    }

    Segment *ss = hashmap_get(cpu->memory_handler->allocated, "SS");
    if (ss == NULL) {
        printf("Erreur : segment de pile introuvable.\n");
        return -1;
    }

    int *sp = (int *)hashmap_get(cpu->context, "SP");
    if (sp == NULL) {
        printf("Erreur : registre SP introuvable.\n");
        return -1;
    }

    if (*sp - 1 < ss->start) {
        printf("Erreur : stack overflow\n");
        return -1;
    }

    // On cherche la valeur dans constant pool
    int *valeur = (int *)hashmap_get(cpu->constant_pool, "value");
    if (valeur == NULL) {
        // Si la valeur n'est pas trouvée, on l'alloue
        valeur = (int *)malloc(sizeof(int));
        if (valeur == NULL) {
            perror("Erreur d'allocation pour valeur");
            return -1;
        }
        *valeur = value;

        // Convertir la valeur en chaîne de caractères
        char snum[20];
        itoa(value, snum, 10);

        int res_insert = hashmap_insert(cpu->constant_pool, snum, valeur);
        if (res_insert != 1) {
            printf("Erreur : échec de l'insertion dans le pool constant.\n");
            free(valeur);
            return -1;
        }
    }

    *valeur = value;

    //@todo peut etre pas bonne méthode et pas besoint de malloc ? on verra à la fin
    // on insère la valeur dans la mémoire du cpu
    *sp -= 1;
    if (store(cpu->memory_handler, "SS", *sp - ss->start, valeur) == NULL) {
        printf("Erreur : échec de l'insertion de la valeur dans le segment de pile.\n");
        *sp += 1;
        return -1;
    }

    return 0;
}

int pop_value(CPU *cpu, int *dest) {

    static int stack_is_empty = 1;

    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return -1;
    }

    Segment *ss = hashmap_get(cpu->memory_handler->allocated, "SS");
    if (ss == NULL) {
        printf("Erreur : segment de pile introuvable.\n");
        return -1;
    }

    int *sp = (int *)hashmap_get(cpu->context, "SP");
    if (sp == NULL) {
        printf("Erreur : registre SP introuvable.\n");
        return -1;
    }

    // On vérifie si la pile est vide
    if (stack_is_empty) {
        printf("Erreur : stack underflow\n");
        return -1;
    }

    // On cherche la valeur dans constant pool
    int *valeur = (int *)load(cpu->memory_handler, "SS", *sp - ss->start);
    if (valeur == NULL) {
        printf("Erreur : échec de la récupération de la valeur dans le segment de pile.\n");
        return -1;
    }

    // On la copie dans le registre de destination
    // Si la destination est NULL, on prend le registre ax par défaut
    if (dest == NULL) {
        dest = (int *)hashmap_get(cpu->context, "AX");
        if (dest == NULL) {
            printf("Erreur : registre AX introuvable.\n");
            return -1;
        }
    }

    *(int *)dest = *valeur;

    // On vérifie si la liste est vide
    if (*sp == ss->start) {
        stack_is_empty = 1;
    } else {
        stack_is_empty = 0;
        *sp += 1;
    }

    return 0;
}