// TEST POUR LES EXERCICES 3 ET 4
#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

int main(void) {
    ParserResult *pr = parse("test_parser.txt");
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

    // Test store et load manuels
    printf("\nTest de la fonction store et load\n");

    int *val = malloc(sizeof(int));
    *val = 764;

    if (store(cpu->memory_handler, "DS", 0, val) == NULL) {
        perror("Erreur lors du stockage de la valeur dans le segment DS\n");
        free(val);
        cpu_destroy(cpu);
        free_parser_result(pr);
        return -1;
    }

    print_data_segment(cpu);

    int *loaded_val = (int *)load(cpu->memory_handler, "DS", 0);
    if (loaded_val == NULL) {
        perror("Erreur lors du chargement de la valeur depuis le segment DS\n");
        free(val);
        cpu_destroy(cpu);
        free_parser_result(pr);
        return -1;
    }

    printf("Valeur chargée depuis le segment DS : %d\n", *loaded_val);
    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}