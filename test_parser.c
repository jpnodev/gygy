#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

int main(void) {
    ParserResult *pr = parse("filename.txt");
    if (pr == NULL) {
        perror("Erreur!\n");
        return -1;
    }

    // Afficher les instructions de la section .DATA et .CODE
    printf("Instructions de la section .DATA :\n");
    display_instructions(pr->data_instructions, pr->data_count);
    printf("Instructions de la section .CODE :\n");
    display_instructions(pr->code_instructions, pr->code_count);

    printf("Table des labels :\n");
    display_hashmap(pr->labels);
    printf("\nTable des emplacements mémoire :\n");
    display_hashmap(pr->memory_locations);

    // Initialiser le CPU
    printf("\nInitialisation du CPU:\n");
    CPU *cpu = cpu_init(MEMORY_SIZE);

    if (cpu == NULL) {
        perror("Erreur d'initialisation du CPU\n");
        free_parser_result(pr);
        return -1;
    }

    // Afficher la mémoire du CPU
    printf("\nAllocation de la mémoire Data Segment\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    print_data_segment(cpu);
    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}