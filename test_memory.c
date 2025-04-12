#include "debug.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024
int main() {
    // Initialisation de la mémoire
    MemoryHandler *memory = memory_init(MEMORY_SIZE);
    if (memory == NULL) {
        printf("Erreur d'initialisation de la mémoire\n");
        return -1;
    }

    // Affichage de la mémoire initiale
    printf("Affichage de la mémoire initiale:\n");
    display_memory(memory);

    // Création de segments
    printf("\nCréation de segments:\n");
    int res = create_segment(memory, "segment_1", 1, 5);
    if (res == -1) {
        printf("Erreur d'allocation du segment 1\n");
    } else if (res == 0) {
        printf("Aucun segment trouvé\n");
    } else {
        printf("Segment 1 alloué avec succès\n");
    }

    res = create_segment(memory, "segment_2", 52, 20);
    if (res == -1) {
        printf("Erreur d'allocation du segment 2\n");
    } else if (res == 0) {
        printf("Aucun segment trouvé\n");
    } else {
        printf("Segment 2 alloué avec succès\n");
    }

    res = create_segment(memory, "segment_3", 72, 20);
    if (res == -1) {
        printf("Erreur d'allocation du segment 3\n");
    } else if (res == 0) {
        printf("Aucun segment trouvé\n");
    } else {
        printf("Segment 3 alloué avec succès\n");
    }

    // Affichage de la mémoire après allocation
    printf("\n> Affichage de la mémoire après allocation:\n");
    display_memory(memory);

    // Suppression de segments
    if ((Segment *)hashmap_get(memory->allocated, "segment_1") == NULL) {
        printf("segment 1 non trouvé\n");
    } else {
        res = remove_segment(memory, "segment_1");
        if (res == -1) {
            printf("Erreur de suppression du segment 1\n");
        } else {
            printf("segment 1 supprimé avec succès\n");
        }
    }

    if ((Segment *)hashmap_get(memory->allocated, "segment_2") == NULL) {
        printf("segment 2 non trouvé\n");
    } else {
        res = remove_segment(memory, "segment_2");
        if (res == -1) {
            printf("Erreur de suppression du segment 2\n");
        } else {
            printf("segment 2 supprimé avec succès\n");
        }
    }

    if ((Segment *)hashmap_get(memory->allocated, "segment_3") == NULL) {
        printf("segment 3 non trouvé\n");
    } else {
        res = remove_segment(memory, "segment_3");
        if (res == -1) {
            printf("Erreur de suppression du segment 3\n");
        } else {
            printf("segment 3 supprimé avec succès\n");
        }
    }

    // Affichage de la mémoire après suppression
    printf("\n> Affichage de la mémoire après suppression:\n");
    display_memory(memory);

    // Libération de la mémoire
    memory_destroy(memory);
    printf("\nMémoire libérée avec succès\n");

    // Affichage de la mémoire après libération
    // printf("Affichage de la mémoire après libération:\n");
    // display_memory(memory);
    // printf("Affichage de la liste des segments libres:\n");
    // display_segments(memory->free_list);
    // printf("Affichage de la table de hachage des segments alloués:\n");
    // display_hashmap(memory->allocated);

    return 0;
}