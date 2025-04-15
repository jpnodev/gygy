#include "memory.h"
#include "hachage.h"
#include "debug.h"
#include "cpu.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>  

#define MEMORY_SIZE 1024

int main(void) {
    printf("=== INITIALISATION DU CPU ===\n");
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    printf("\n\n");

    printf("=== TEST 1: RECUPERATION ET STOCKAGE DES DONNEES D'UN SEGMENT ===\n");
    printf("\n");
    print_free_list(cpu->memory_handler);

    int pos = getSegFreePos(cpu->memory_handler, 1);
    if (pos < 0 || pos > cpu->memory_handler->total_size) {
        printf("Erreur lors de la recherche d'une position\n");
        cpu_destroy(cpu);
        return 0;
    }
    printf("Premiere position libre trouvée: %d\n", pos);

    int s = create_segment(cpu->memory_handler, "TEST_SEG", pos, 5);
    if (s == -1 || s == 0) {
        printf("Erreur lors de la création du segment\n");
        cpu_destroy(cpu);
        return 0;
    }

    int *val = malloc(sizeof(int));
    if (val == NULL) {
        perror("Erreur malloc pour val");
        cpu_destroy(cpu);
        return 1;
    }
    *val = 5;

    val = store(cpu->memory_handler, "TEST_SEG", pos, val);
    if (val == NULL) {
        printf("Erreur lors du stockage des données dans le segment\n");
        cpu_destroy(cpu);
        return 0;
    }
    printf("Chargement des données réussi\n");


    val = load(cpu->memory_handler, "TEST_SEG", pos);
    if (val == NULL) {
        printf("Erreur lors de la récupération des données du segment\n");
        cpu_destroy(cpu);
        return 0;
    }

    printf("Valeur récupérée: %d\n", *val);


    remove_segment(cpu->memory_handler, "TEST_SEG");
    printf("\n");
    print_free_list(cpu->memory_handler);
    cpu_destroy(cpu);

    printf("\n\n");

    printf("=== TEST 2: LECTURE DU CODE SIMPLE (QUE DATA ET CODE SEGMENT) ===\n");

    cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    printf("\n=== lecture du ficher ===\n");
    ParserResult *pr = parse("test_cpu.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("\n=== allocation data segment ===\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);

    printf("\n=== etat initial ===\n");
    print_data_segment(cpu);

    printf("\n=== table hachage des index ===\n");
    display_hashmap(pr->memory_locations);

    printf("\n=== allocation du code segment ===\n");
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    printf("\n=== instructions initials ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("\n=== resolve constants ===\n");
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    printf("\n=== instructions après resolve ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("\n=== run programme ===\n");
    run_program(cpu);
    free_parser_result(pr);
    cpu_destroy(cpu);

    printf("\n\n");

    printf("=== TEST 3: AJOUT DU STACK SEGMENT ===\n");

    cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    reset_parse_offset();
    printf("\n=== lecture du ficher ===\n");
    pr = parse("test_stack.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("\n=== allocation data segment ===\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);

    printf("\n=== etat initial ===\n");
    print_data_segment(cpu);

    printf("\n=== allocation stack segment ===\n");
    allocate_stack_segment(cpu);

    printf("\n=== etat initial du stack segment ===\n");
    display_stack_segment(cpu);

    printf("\n=== allocation code segment ===\n");
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    printf("\n=== instructions initial ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("\n=== resolve constats ===\n");
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    printf("\n=== instruction after resolve ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("=== run programme ===\n");
    run_program(cpu);
    free_parser_result(pr);

    cpu_destroy(cpu);

    printf("\n\n");

    printf("\n=== TEST 4: AJOUT DU EXTRA ===\n");
    
    return 0;
}
