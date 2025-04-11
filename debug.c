#include "debug.h"


void display_hashentry(HashEntry* entry) {
    if (entry == NULL) {
        printf("L'entrée de hachage est NULL\n");
        return;
    } else if (entry->key == TOMBSTONE) {
        printf("TOMBSTONE\n");
        return;
    }
    printf("Clé: %s, Valeur: %p\n", entry->key, entry->value);
}

void display_hashmap(HashMap* map) {
    if (map == NULL) {
        printf("La table de hachage est NULL\n");
        return;
    }
    int empty = 1;
    for (int i = 0; i < map->size; i++) {
        if (map->table[i].key != NULL){
            empty = 0;
            break;
        }
    }
    if (empty) {
        printf("La table de hachage est vide\n");
        return;
    }
    printf("Table de hachage:\n");
    for (int i = 0; i < map->size; i++) {
        if (map->table[i].key != NULL){
            printf("Index %d: ", i);
            display_hashentry(&map->table[i]);
        }
    }
}

void display_segment(Segment* seg) {
    if (seg == NULL) {
        printf("Le segment est NULL\n");
        return;
    }
    printf("Segment: start = %d, end = %d, size = %d\n", seg->start, ((seg->start + seg->size) - 1), seg->size);
}

void display_segments(Segment* seg) {
    if (seg == NULL) {
        printf("La liste de segments est vide\n");
        return;
    }
    Segment* current = seg;
    while (current != NULL) {
        display_segment(current);
        current = current->next;
    }
}

void display_memory(MemoryHandler* mem) {
    if (mem == NULL) {
        printf("La structure MemoryHandler est NULL\n");
        return;
    }
    printf("Taille totale de la mémoire: %d cases\n", mem->total_size);
    
    if (mem->free_list == NULL) {
        printf("La mémoire est pleine\n");
        return;
    }

    if (mem->free_list->size == mem->total_size) {
        printf("La mémoire est vide\n");
        return;
    }
    
    printf("Segments libres:\n");
    display_segments(mem->free_list);
    // printf("Segments alloués:\n");
}