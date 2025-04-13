#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

int main(void) {
    // 1. Инициализация CPU
    printf("=== INITIALISATION DU CPU ===\n");
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    printf("parse\n");
    // 2. Разбор файла
    ParserResult *pr = parse("filename.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== RESULTAT DU PARSER: INSTRUCTIONS ===\n");
    afficher_instructions(pr->data_instructions, pr->data_count);
    printf("=== PARSER RESULT : MEMORY LOCATION ===\n");
    display_hashmap(pr->memory_locations);

    printf("allocate_variables\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    printf("=== APRES ALLOCATE VARIABLES: INSTRUCTIONS ===\n");
    afficher_instructions(pr->data_instructions, pr->data_count);
    printf("=== AFFICHAGE DE CPU MEMORY ===\n");
    print_data_segment(cpu);

    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    // // 5. Напечатать содержимое сегмента данных
    // printf("\n=== SEGMENT DE DONNÉES INIT ===\n");
    // print_data_segment(cpu);

    // // 6. Напечатать ассемблер .CODE
    // printf("\n=== INSTRUCTIONS DE CODE ===\n");
    // afficher_instructions(pr->code_instructions, pr->code_count);

    // // 7. Распределить и загрузить .CODE
    // allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    // // 8. Запустить программу
    // run_program(cpu);

    // // 9. Освобождение ресурсов
    // free_parser_result(pr);
    // cpu_destroy(cpu);

    return 0;
}