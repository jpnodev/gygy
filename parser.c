#include "parser.h"

ParserResult *init() {
    ParserResult *pr = (ParserResult *)malloc(sizeof(ParserResult));
    if (pr == NULL) {
        perror("Erreur d'allocation de la mémoire pour ParserResult");
        return NULL;
    }
    pr->data_instructions = NULL;
    pr->data_count = 0;
    pr->code_instructions = NULL;
    pr->code_count = 0;
    pr->labels = hashmap_create(SIMPLE);
    pr->memory_locations = hashmap_create(SIMPLE);

    return pr;
}

Instruction *parse_data_instruction(const char *line, HashMap *memory_locations) {
    if (line == NULL || memory_locations == NULL) {
        perror("Instruction ne peut pas être lue");
        return NULL;
    }

    static int taille = 0;

    Instruction *instruction = (Instruction *)malloc(sizeof(Instruction));
    if (instruction == NULL) {
        perror("Erreur d'allocation de la mémoire pour data_instructions");
        return NULL;
    }

    char inst[20], op1[20], op2[100];
    if (sscanf(line, "%19s %19s %[^\n]", inst, op1, op2) < 2) {
        perror("Format d'instruction invalide");
        free(instruction);
        return NULL;
    }

    instruction->mnemonic = strdup(inst);
    instruction->operand1 = strdup(op1);
    instruction->operand2 = strdup(op2);

    if (!instruction->mnemonic || !instruction->operand1 || !instruction->operand2) {
        perror("Erreur d'allocation de la mémoire pour les champs de l'instruction");
        free(instruction->mnemonic);
        free(instruction->operand1);
        free(instruction->operand2);
        free(instruction);
        return NULL;
    }

    int element_count = 1;
    if (strchr(op2, ',')) {
        char *copy = strdup(op2);
        if (!copy) {
            perror("Erreur d'allocation de mémoire pour la copie de op2");
            free(instruction->mnemonic);
            free(instruction->operand1);
            free(instruction->operand2);
            free(instruction);
            return NULL;
        }

        char *token = strtok(copy, ",");
        element_count = 0;
        while (token != NULL) {
            element_count++;
            token = strtok(NULL, ",");
        }
        free(copy);
    }

    hashmap_insert(memory_locations, instruction->mnemonic, (void *)(long)taille);
    taille += element_count;

    return instruction;
}

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count) {
    if (line == NULL || labels == NULL) {
        perror("Instruction ne peut pas être lue");
        return NULL;
    }

    Instruction *instruction = (Instruction *)malloc(sizeof(Instruction));
    if (instruction == NULL) {
        perror("Erreur d'allocation de la mémoire pour data_instructions");
        return NULL;
    }

    char label[20] = "", inst[20] = "", op1[20] = "", op2[20] = "";

    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        perror("Erreur d'allocation de la mémoire pour la copie de la ligne");
        free(instruction);
        return NULL;
    }

    // Supprimer le caractère de nouvelle ligne à la fin
    size_t len = strlen(line_copy);
    if (len > 0 && line_copy[len - 1] == '\n') {
        line_copy[len - 1] = '\0';
    }

    if (strchr(line_copy, ':')) { // Si une étiquette est présente
        char *token = strtok(line_copy, ":");
        strcpy(label, token);

        //@todo on devrait plutot insérer dans label
        hashmap_insert(labels, label, (void *)(long)code_count);

        token = strtok(NULL, " ");
        if (token)
            strcpy(inst, token);

        token = strtok(NULL, ",");
        if (token)
            strcpy(op1, token);

        token = strtok(NULL, " ");
        if (token)
            strcpy(op2, token);

    } else { // Pas d'étiquette, lecture normale
        sscanf(line_copy, "%19s %19[^,], %19s", inst, op1, op2);
    }

    instruction->mnemonic = strdup(inst);
    instruction->operand1 = strdup(op1);
    instruction->operand2 = strdup(op2);

    if (!instruction->mnemonic || !instruction->operand1 || !instruction->operand2) {
        perror("Erreur d'allocation de la mémoire pour les champs de l'instruction");
        free(instruction->mnemonic);
        free(instruction->operand1);
        free(instruction->operand2);
        free(instruction);
        free(line_copy);
        return NULL;
    }

    free(line_copy);

    return instruction;
}

ParserResult *parse(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return NULL;
    }

    ParserResult *result = init();
    if (result == NULL) {
        fclose(file);
        return NULL;
    }

    int data_capacity = 10;
    int code_capacity = 10;
    result->data_instructions = malloc(data_capacity * sizeof(Instruction *));
    result->code_instructions = malloc(code_capacity * sizeof(Instruction *));

    if (result->data_instructions == NULL || result->code_instructions == NULL) {
        perror("Erreur d'allocation de mémoire pour les instructions");
        fclose(file);
        return NULL;
    }

    char line[256];
    int is_data_section = 0;
    int is_code_section = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '\0' || line[0] == ';')
            continue;

        if (strncmp(line, ".DATA", 5) == 0) {
            is_data_section = 1;
            is_code_section = 0;
            continue;
        }

        if (strncmp(line, ".CODE", 5) == 0) {
            is_data_section = 0;
            is_code_section = 1;
            continue;
        }

        if (is_data_section) {
            Instruction *instruction = parse_data_instruction(line, result->memory_locations);
            if (instruction == NULL)
                continue;

            if (result->data_count >= data_capacity) {
                data_capacity *= 2;
                result->data_instructions = realloc(result->data_instructions, data_capacity * sizeof(Instruction *));
                if (result->data_instructions == NULL) {
                    perror("Erreur de réallocation de mémoire pour data_instructions");
                    fclose(file);
                    return NULL;
                }
            }

            result->data_instructions[result->data_count++] = instruction;
        }

        if (is_code_section) {
            Instruction *instruction = parse_code_instruction(line, result->labels, result->code_count);
            if (instruction == NULL)
                continue;

            if (result->code_count >= code_capacity) {
                code_capacity *= 2;
                result->code_instructions = realloc(result->code_instructions, code_capacity * sizeof(Instruction *));
                if (result->code_instructions == NULL) {
                    perror("Erreur de réallocation de mémoire pour code_instructions");
                    fclose(file);
                    return NULL;
                }
            }

            result->code_instructions[result->code_count++] = instruction;
        }
    }

    fclose(file);
    return result;
}

void liberer_instruction(Instruction *i) {
    if (i == NULL)
        return;
    free(i->mnemonic);
    free(i->operand1);
    free(i->operand2);
    free(i);
}

void free_parser_result(ParserResult *result) {
    if (result == NULL)
        return;

    for (int i = 0; i < result->data_count; i++) {
        liberer_instruction(result->data_instructions[i]);
    }
    free(result->data_instructions);

    for (int i = 0; i < result->code_count; i++) {
        liberer_instruction(result->code_instructions[i]);
    }
    free(result->code_instructions);

    // printf("libération labels\n");
    hashmap_destroy(result->labels);

    // printf("libération memory_locations\n");
    hashmap_destroy(result->memory_locations);

    free(result);
}
