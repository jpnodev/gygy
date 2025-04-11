#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "hachage.h"

void test_init() {
    printf("Testing init()...\n");
    ParserResult *pr = init();
    if (pr == NULL) {
        printf("init() failed: returned NULL\n");
        return;
    }
    if (pr->data_instructions == NULL && pr->code_instructions == NULL &&
        pr->data_count == 0 && pr->code_count == 0 &&
        pr->labels != NULL && pr->memory_locations != NULL) {
        printf("init() passed\n");
    } else {
        printf("init() failed\n");
    }
    hashmap_destroy(pr->labels);
    hashmap_destroy(pr->memory_locations);
    free(pr);
}

void test_parse_data_instruction() {
    printf("Testing parse_data_instruction()...\n");
    HashMap *memory_locations = hashmap_create();
    const char *line = "var1 INT 10";
    Instruction *instruction = parse_data_instruction(line, memory_locations);

    if (instruction == NULL) {
        printf("parse_data_instruction() failed: returned NULL\n");
        hashmap_destroy(memory_locations);
        return;
    }

    if (strcmp(instruction->mnemonic, "var1") == 0 &&
        strcmp(instruction->operand1, "INT") == 0 &&
        strcmp(instruction->operand2, "10") == 0) {
        printf("parse_data_instruction() passed\n");
    } else {
        printf("parse_data_instruction() failed\n");
    }

    free(instruction->mnemonic);
    free(instruction->operand1);
    free(instruction->operand2);
    free(instruction);
    hashmap_destroy(memory_locations);
}

void test_parse_code_instruction() {
    printf("Testing parse_code_instruction()...\n");
    HashMap *memory_locations = hashmap_create();
    const char *line = "label1: ADD R1, R2";
    Instruction *instruction = parse_code_instruction(line, memory_locations, 0);

    if (instruction == NULL) {
        printf("parse_code_instruction() failed: returned NULL\n");
        hashmap_destroy(memory_locations);
        return;
    }

    if (strcmp(instruction->mnemonic, "ADD") == 0 &&
        strcmp(instruction->operand1, "R1") == 0 &&
        strcmp(instruction->operand2, "R2") == 0) {
        printf("parse_code_instruction() passed\n");
    } else {
        printf("parse_code_instruction() failed\n");
    }

    free(instruction->mnemonic);
    free(instruction->operand1);
    free(instruction->operand2);
    free(instruction);
    hashmap_destroy(memory_locations);
}

int main() {
    test_init();
    test_parse_data_instruction();
    test_parse_code_instruction();
    return 0;
}