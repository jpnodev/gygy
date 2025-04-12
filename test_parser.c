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

    afficher_instructions(pr->data_instructions, pr->data_count);
    // afficher_instructions(pr->code_instructions, pr->code_count);
    // display_hashmap(pr -> labels);
    // display_hashmap(pr -> memory_locations);
    // CPU *cpu = init_CPU(MEMORY_SIZE);
    // allocate_variables(cpu, pr -> data_instructions, pr -> data_count);
    // print_data_segment(cpu);
    // free_parser_result(pr);
    // cpu_destroy(cpu);
    // return 0;
}