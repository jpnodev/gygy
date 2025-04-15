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

    printf("=== LECTURE DU FICHIER test_extra.txt ===\n");
    ParserResult *pr = parse("test_extra.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== ALLOCATION DATA SEGMENT (DS) ===\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    print_data_segment(cpu);

    printf("=== ALLOCATION STACK SEGMENT (SS) ===\n");
    allocate_stack_segment(cpu);
    display_stack_segment(cpu);

    printf("=== ALLOCATION CODE SEGMENT (CS) ===\n");
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    printf("=== INSTRUCTIONS INITIALES ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("=== RESOLVE CONSTANTS (.DATA/.CODE) ===\n");
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== INSTRUCTIONS APRÈS REMPLACEMENT ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    printf("=== TEST ALLOCATION SEGMENT EXTRA (ES) ===\n");

    int *AX = hashmap_get(cpu->context, "AX");
    int *BX = hashmap_get(cpu->context, "BX");
    int *ZF = hashmap_get(cpu->context, "ZF");

    if (!AX || !BX || !ZF) {
        printf("Erreur : registres AX, BX ou ZF non disponibles.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    *AX = 5;
    *BX = 1;
    *ZF = 0;

    if (alloc_es_segment(cpu) == 0) {
        printf("Segment ES alloué avec succès.\n");
    }
    print_extra_segment(cpu);

    printf("=== EXÉCUTION DU PROGRAMME ===\n");
    run_program(cpu);

    printf("=== LIBÉRATION DU SEGMENT EXTRA (ES) ===\n");
    if (free_es_segment(cpu) == 0) {
        printf("Segment ES libéré avec succès.\n");
    }
    print_extra_segment(cpu);

    printf("=== FIN ===\n");
    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}
