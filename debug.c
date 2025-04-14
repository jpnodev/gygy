#include "cpu.h"

void display_hashentry(HashEntry *entry) {
    if (entry == NULL) {
        printf("L'entrée de hachage est NULL\n");
        return;
    } else if (entry->key == TOMBSTONE) {
        printf("TOMBSTONE\n");
        return;
    }
    printf("Clé: %s, Valeur: %p\n", entry->key, entry->value);
}

void display_hashmap(HashMap *map) {
    if (map == NULL) {
        printf("La table de hachage est NULL\n");
        return;
    }

    switch (map->type) {
    case SIMPLE:
        printf("Type de données: simple\n");
        break;
    case BASIC_MALLOC:
        printf("Type de données: simple allouées avec malloc\n");
        break;
    case SEGMENT:
        printf("Type de données: structure SEGMENT\n");
        break;
    default:
        printf("Type de données inconnu\n");
        break;
    }

    int empty = 1;
    for (int i = 0; i < map->size; i++) {
        if (map->table[i].key != NULL) {
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
        if (map->table[i].key != NULL) {
            printf("Index %d: ", i);
            display_hashentry(&map->table[i]);
        }
    }
}

void display_segment(Segment *seg) {
    if (seg == NULL) {
        printf("Le segment est NULL\n");
        return;
    }
    printf("Segment: start = %d, end = %d, size = %d\n", seg->start, ((seg->start + seg->size) - 1), seg->size);
}

void display_segments(Segment *seg) {
    if (seg == NULL) {
        printf("La liste de segments est vide\n");
        return;
    }
    Segment *current = seg;
    while (current != NULL) {
        display_segment(current);
        current = current->next;
    }
}

void display_memory(MemoryHandler *mem) {
    if (mem == NULL) {
        printf("La structure MemoryHandler est NULL\n");
        return;
    }
    printf("Taille totale de la mémoire: %d cases\n", mem->total_size);

    if (mem->free_list == NULL) {
        printf("La mémoire est pleine\n");
        display_hashmap(mem->allocated);
        return;
    }

    if (mem->free_list->size == mem->total_size) {
        printf("La mémoire est vide\n");
        display_segments(mem->free_list);
        return;
    }

    printf("Segments libres:\n");
    display_segments(mem->free_list);
    display_hashmap(mem->allocated);
}

void display_instructions(Instruction **liste, int count) {
    if (liste == NULL || count == 0) {
        printf("Aucune instruction à afficher.\n");
        return;
    }

    printf("Instructions :\n");
    for (int i = 0; i < count; i++) {
        if (liste[i] != NULL) {
            printf("\"%s\"  \"%s\"  \"%s\" \n", liste[i]->mnemonic, liste[i]->operand1,
                   liste[i]->operand2 ? liste[i]->operand2 : "");
        }
    }
    printf("\n");
}

void display_stack_segment(CPU *cpu) {
    if (cpu == NULL) {
        printf("Erreur : CPU est NULL.\n");
        return;
    }

    Segment *ss = hashmap_get(cpu->memory_handler->allocated, "SS");
    if (ss == NULL) {
        printf("Erreur : segment de pile 'SS' introuvable.\n");
        return;
    }

    int *sp = (int *)hashmap_get(cpu->context, "SP");
    int *bp = (int *)hashmap_get(cpu->context, "BP");

    if (sp == NULL || bp == NULL) {
        printf("Erreur : registres SP ou BP introuvables.\n");
        return;
    }

    printf("Contenu du segment de pile 'SS' (de %d à %d) :\n", ss->start, ss->start + ss->size - 1);
    int count = 0;
    for (int i = 0; i < ss->size; i++) {
        int addr = ss->start + i;
        int *val = (int *)(cpu->memory_handler->memory[addr]);

        if (val != NULL) {
            printf("SS[%d] = %d", i, *val);

            if (addr == *sp) {
                printf(" <-- SP");
            }
            if (addr == *bp) {
                printf(" <-- BP");
            }

            printf("\n");
            count++;
        }
    }

    if (count == 0) {
        printf("Pile vide (aucune case utilisée).\n");
    }
}