#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "debug.h"

int main() {
    MemoryHandler* memory = memory_init(1024);
    if (memory == NULL) {
        printf("Erreur d'initialisation de la mémoire\n");
        return -1;
    }

    if (create_segment(memory, "new_segment_1", 0, 52) != 1) {
        printf("erreur d\'allocation du segment 1\n");
    }

    if (create_segment(memory, "new_segment_2", 4, 10) != 1) {
        printf("erreur d\'allocation du segment 2\n");
    }

    if (create_segment(memory, "new_segment_3", 96, 72) != 1) {
        printf("erreur d\'allocation du segment 3\n");
    }

    display_memory(memory);
    display_hashmap(memory->allocated);
    display_segments(memory->free_list);

    if ((Segment*)hashmap_get(memory->allocated, "new_segment_1") == NULL) {
        printf("erreur d\'allocation du segment 1\n");
    } else {
        remove_segment(memory, "new_segment_1");
        printf("segment 1 supprimé\n");
    }
    if ((Segment*)hashmap_get(memory->allocated, "new_segment_2") == NULL) {
        printf("erreur d\'allocation du segment 2\n");
    } else {
        remove_segment(memory, "new_segment_2");
        printf("segment 2 supprimé\n");
    }
    if ((Segment*)hashmap_get(memory->allocated, "new_segment_3") == NULL) {
        printf("erreur d\'allocation du segment 3\n");
    } else {
        remove_segment(memory, "new_segment_3");
        printf("segment 3 supprimé\n");
    }

    memory_destroy(memory);

    return 0;
}