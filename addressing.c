#include "addressing.h"

int matches(const char* pattern , const char* string) {
    regex_t regex;
    int result = regcomp(&regex , pattern , REG_EXTENDED);
    if (result) {
        fprintf(stderr , "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}


void* immediate_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^-?[0-9]+$", operand)) {
        int* valeur = (int*)hashmap_get(cpu->constant_pool, operand);
        if (valeur == NULL) {
            valeur = (int*)malloc(sizeof(int));
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


void* register_addressing(CPU *cpu, const char *operand) {
    if (cpu == NULL || operand == NULL) {
        printf("Erreur : argument invalide (cpu ou operand est NULL).\n");
        return NULL;
    }

    if (matches("^(A|B|C|D)X$", operand)){
        int* valeur = (int*)hashmap_get(cpu->context, operand);
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

        int cell_count = cpu->memory_handler->total_size / sizeof(void*);
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

void* register_indirect_addressing(CPU *cpu, const char *operand) {
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

void handle_MOV(CPU* cpu, void* src, void* dest) {
    if (cpu == NULL || src == NULL || dest == NULL) {
        printf("Erreur : argument invalide (cpu, src ou dest est NULL).\n");
        return;
    }

    *(int*)dest = *(int*)src;
}

void* resolve_addressing(CPU* cpu, const char* operand) {
    if (cpu == NULL) {
        printf("Erreur : argument invalide (cpu est NULL).\n");
        return NULL;
    }
    if (operand == NULL) {
        printf("Erreur : argument invalide (operand est NULL).\n");
        return NULL;
    }

    void* result;

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