#include "memory.h"
#include "hachage.h"
#include "debug.h"
#include "parser.h"
#include "cpu.h"
#include "addressing.h"

#include <stdio.h>

CPU* setup_test_environment() {
   
    CPU* cpu = cpu_init(1024);
    if (!cpu) {
        printf("Erreur : échec de l'initialisation du CPU.\n");
        return NULL;
    }

    int* ax = hashmap_insert(cpu->context, "AX", ax);
    int* bx = hashmap_insert(cpu->context, "BX", bx);
    int* cx = hashmap_insert(cpu->context, "CX", cx);
    int* dx = hashmap_insert(cpu->context, "DX", dx);

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", 0, 20);

      
        for (int i = 0; i < 10; i++) {
            int* value = (int*) malloc(sizeof(int));
            *value = i * 10 + 5;
            store(cpu->memory_handler, "DS", i, value);
        }
    }

    printf("Environnement de test initialisé.\n");
    return cpu;
}

void* get_safe_ds_cell(CPU* cpu, int index) {
    Segment* ds = hashmap_get(cpu->memory_handler->allocated, "DS");
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

    CPU* cpu = cpu_init(1024);
    if (!setup_test_environment(cpu)){
        return -1;
    }
    cpu_destroy(cpu);
    // print_data_segment(cpu);

    // Segment* ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    // if (ds == NULL) {
    //     printf("Erreur : Segment DS introuvable.\n");
    //     return -1;
    // }

    // void* dest = NULL;

    // for (int i = 0; i < 4; i++) {
    //     dest = get_safe_ds_cell(cpu, i);
    //     if (dest != NULL) {
    //         if (i == 0) {
    //              // TEST Adressage immédiat:
    //             void* val_src = immediate_addressing(cpu, "AX"); //donne message d'erreur de format;
    //             printf("\n");
    //             val_src = immediate_addressing(cpu, "42");
    //             handle_MOV(cpu, val_src, dest);
    //             print_data_segment(cpu);
    //         }
    //         if (i == 1) {
    //             // TEST Adressage indirect par registre:
    //             void* val_src = register_indirect_addressing(cpu, "AX"); //donne message d'erreur de format;
    //             val_src = register_indirect_addressing(cpu, "[AX]");
    //             handle_MOV(cpu, val_src, dest);
    //             print_data_segment(cpu);
    //         }
    //         if (i == 2) {
    //              // TEST Adressage direct: 
    //             void* val_src = memory_direct_addressing(cpu, "[AX]"); //donne message d'erreur de format;
    //             val_src = memory_direct_addressing(cpu, "[5]");
    //             handle_MOV(cpu, val_src, dest);
    //             print_data_segment(cpu);
    //         }
    //         if (i == 3) {
    //               // TEST Adressage par registre: 
    //             void* val_src = register_addressing(cpu, "[CX]"); //donne message d'erreur de format;
    //             val_src = register_addressing(cpu, "CX");
            
    //             handle_MOV(cpu, val_src, dest);
    //             print_data_segment(cpu);
    //         }
    //     }
    // }

    // void* val_src = resolve_addressing(cpu, "888888");
    // dest = get_safe_ds_cell(cpu, 8);
    // if (dest != NULL) {
    //     handle_MOV(cpu, val_src, dest);
    //     print_data_segment(cpu);
    // }
    return 0;

}
