#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

int main(void) {
    // 1. Инициализация CPU
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return -1;
    }

    // 2. Разбор файла
    ParserResult* pr = parse("filename.txt");
    if (pr == NULL) {
        cpu_destroy(cpu);
        return -1;
    }

    // 3. Разрешить константы
    if (resolve_constants(pr) == -1) {
        printf("Erreur : échec de la résolution des constantes.\n");
        free_parser_result(pr);
        cpu_destroy(cpu);
        return -1;
    }

    // 4. Выделить и инициализировать .DATA
    allocate_variables(cpu, pr->data_instructions, pr->data_count);

    // 5. Напечатать содержимое сегмента данных
    printf("\n=== SEGMENT DE DONNÉES INIT ===\n");
    print_data_segment(cpu);

    // 6. Напечатать ассемблер .CODE
    printf("\n=== INSTRUCTIONS DE CODE ===\n");
    afficher_instructions(pr->code_instructions, pr->code_count);

    // 7. Распределить и загрузить .CODE
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);

    // 8. Запустить программу
    run_program(cpu);

    // 9. Освобождение ресурсов
    free_parser_result(pr);
    cpu_destroy(cpu);

    return 0;
}

