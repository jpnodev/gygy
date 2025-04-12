// TEST POUR L'EXERCICE 5
#include "cpu.h"
#include "debug.h"
#include "hachage.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>

#define MEMORY_SIZE 1024

void print_free_segments(MemoryHandler *handler) {
    Segment *s = handler->free_list;
    printf("Free segments:\n");
    while (s != NULL) {
        printf("  start = %d, size = %d\n", s->start, s->size);
        s = s->next;
    }
}

CPU *setup_test_environment() {

    // Initialiser le CPU
    CPU *cpu = cpu_init(MEMORY_SIZE);
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    // Initialiser les registres avec des valeurs spécifiques
    int *ax = (int *)hashmap_get(cpu->context, "AX");
    int *bx = (int *)hashmap_get(cpu->context, "BX");
    int *cx = (int *)hashmap_get(cpu->context, "CX");
    int *dx = (int *)hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    print_free_segments(cpu->memory_handler);

    // Creer et initialiser le segment de donnees
    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", 0, 20);
        // Initialiser le segment de donn es avec des valeurs de test
        for (int i = 0; i < 10; i++) {
            int *value = (int *)malloc(sizeof(int));
            *value = i * 10 + 5; // Valeurs 5, 15, 25, 35...
            store(cpu->memory_handler, "DS", i, value);
        }
    }
    printf("Test environment initialized.\n");
    return cpu;
}

void *get_safe_ds_cell(CPU *cpu, int index) {
    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (ds == NULL) {
        printf("Erreur : segment DS introuvable.\n");
        return NULL;
    }
    if (index < 0 || index >= ds->size) {
        printf("Erreur : index %d hors des bornes du segment DS (taille = %d).\n", index, ds->size);
        return NULL;
    }
    return cpu->memory_handler->memory[ds->start + index];
}

int main(void) {
    CPU *cpu = setup_test_environment();
    if (!cpu) {
        printf("Erreur : échec de l'initialisation de l'environnement de test.\n");
        return -1;
    }

    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (ds == NULL) {
        printf("Erreur : Segment DS introuvable.\n");
        return -1;
    }

    void *dest = NULL;
    print_data_segment(cpu);

    for (int i = 0; i < 4; i++) {
        dest = get_safe_ds_cell(cpu, i);
        if (dest != NULL) {
            if (i == 0) {
                // TEST Adressage immédiat:
                void *val_src = immediate_addressing(cpu, "AX"); // donne message d'erreur de format;
                printf("\n");
                val_src = immediate_addressing(cpu, "42");
                handle_MOV(cpu, val_src, dest);
                print_data_segment(cpu);
            }
            if (i == 1) {
                // TEST Adressage indirect par registre:
                void *val_src = register_indirect_addressing(cpu, "AX"); // donne message d'erreur de format;
                val_src = register_indirect_addressing(cpu, "[AX]");
                handle_MOV(cpu, val_src, dest);
                print_data_segment(cpu);
            }
            if (i == 2) {
                // TEST Adressage direct:
                void *val_src = memory_direct_addressing(cpu, "[AX]"); // donne message d'erreur de format;
                val_src = memory_direct_addressing(cpu, "[5]");
                handle_MOV(cpu, val_src, dest);
                print_data_segment(cpu);
            }
            if (i == 3) {
                // TEST Adressage par registre:
                void *val_src = register_addressing(cpu, "[CX]"); // donne message d'erreur de format;
                val_src = register_addressing(cpu, "CX");

                handle_MOV(cpu, val_src, dest);
                print_data_segment(cpu);
            }
        }
    }

    void *val_src = resolve_addressing(cpu, "888888");
    dest = get_safe_ds_cell(cpu, 8);
    if (dest != NULL) {
        handle_MOV(cpu, val_src, dest);
        print_data_segment(cpu);
    }

    display_memory(cpu->memory_handler);
    print_free_segments(cpu->memory_handler);

    cpu_destroy(cpu);

    return 0;
}
