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
    ParserResult *pr = parse("filename.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    printf("=== ALLOCATION DATA SEGMENT ===\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);

    printf("=== ETAT INITIAL ===\n");
    print_data_segment(cpu);

    printf("=== TABLE HACHAGE DES INDEX ===\n");
    display_hashmap(pr->memory_locations);

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

    printf("=== RUN PROGRAMME ===\n");
    run_program(cpu);



    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}

/*int main(void) {
    // 1. Инициализация CPU
    printf("=== INITIALISATION DU CPU ===\n");
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    // 2. Разбор файла
    printf("parse\n");
    ParserResult *pr = parse("filename.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    // 3. Вывод результатов парсера
    printf("=== RESULTAT DU PARSER: INSTRUCTIONS ===\n");
    afficher_instructions(pr->data_instructions, pr->data_count);
    printf("=== PARSER RESULT : MEMORY LOCATION ===\n");
    display_hashmap(pr->memory_locations);

    // 4. Выделение переменных
    printf("allocate_variables\n");
    allocate_variables(cpu, pr->data_instructions, pr->data_count, pr);
    printf("=== APRES ALLOCATE VARIABLES: INSTRUCTIONS ===\n");
    afficher_instructions(pr->data_instructions, pr->data_count);
    printf("=== AFFICHAGE DE CPU MEMORY ===\n");
    print_data_segment(cpu);

    // 5. Замена переменных на значения
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    // 6. Проверка, все ли операнды заменены на индексы
    printf("\n=== VERIFICATION DES CONSTANTES ===\n");
    for (int i = 0; i < pr->code_count; i++) {
        Instruction *instr = pr->code_instructions[i];
        if (instr->operand1 && !matches("^-?[0-9]+$", instr->operand1)) {
            printf("⚠️  operand1 НЕ заменён: '%s'\n", instr->operand1);
        }
        if (instr->operand2 && !matches("^-?[0-9]+$", instr->operand2)) {
            printf("⚠️  operand2 НЕ заменён: '%s'\n", instr->operand2);
        }
    }

    // 7. Вывод финального кода
    printf("\n=== INSTRUCTIONS DE CODE APRES RESOLVE_CONSTANTS ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    // 8. Распределение .CODE
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    // 9. Запуск программы (опционально)
    // run_program(cpu);

    // 10. Освобождение ресурсов
    free_parser_result(pr);
    cpu_destroy(cpu);

    return 0;
}*/