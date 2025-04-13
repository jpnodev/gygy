#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

int main(void) {
    printf("=== INITIALISATION DU CPU ===\n");
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    printf("=== LECTURE FICHIER ===\n");
    ParserResult *pr = parse("test_stack.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== ALLOCATION DATA SEGMENT ===\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);

    printf("=== ETAT INITIAL ===\n");
    print_data_segment(cpu);

    //printf("=== TABLE HACHAGE DES INDEX ===\n");
    //display_hashmap(pr->memory_locations);

    printf("=== ALLOCATION CODE SEGMENT ===\n");
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    printf("=== INSTRUCTIONS INTIAL ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("=== RESOLVE CONSTANTS ===\n");
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== INSTRUCTIONS AFTER RESOLVE ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("=== ALLOCATION STACK SEGMENT ===\n");
    allocate_stack_segment(cpu);

    printf("=== ETAT INITIAL STACK SEGMENT ===\n");
    print_stack_segment(cpu);


    printf("=== RUN PROGRAMME ===\n");
    run_program(cpu);



    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}