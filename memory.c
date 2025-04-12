#include "memory.h"

MemoryHandler *memory_init(int size) {
    MemoryHandler *mem = (MemoryHandler *)malloc(sizeof(MemoryHandler));
    if (mem == NULL) {
        perror("Erreur d'allocation de la mémoire pour MemoryHandler");
        return NULL;
    }

    mem->memory = malloc(sizeof(void *) * size);
    if (mem->memory == NULL) {
        perror("Erreur d'allocation de la mémoire pour memory");
        free(mem);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        mem->memory[i] = NULL;
    }

    mem->total_size = size;

    mem->free_list = (Segment *)malloc(sizeof(Segment));
    if (mem->free_list == NULL) {
        perror("Erreur d'allocation de la mémoire pour free_list");
        free(mem->memory);
        free(mem);
        return NULL;
    }

    mem->free_list->start = 0;
    mem->free_list->size = size;
    mem->free_list->next = NULL;

    mem->allocated = hashmap_create(SEGMENT);
    if (mem->allocated == NULL) {
        perror("Erreur d'allocation de la mémoire pour allocated");
        free(mem->free_list);
        free(mem->memory);
        free(mem);
        return NULL;
    }

    return mem;
}

Segment *find_free_segment(MemoryHandler *handler, int start, int size, Segment **prev) {
    if (handler == NULL || handler->free_list == NULL) {
        perror("Erreur. Gestionnaire de la mémoire est vide \n");
        return NULL;
    }

    *prev = NULL;
    Segment *temp = handler->free_list;

    while (temp) {
        if (temp->start <= start && temp->start + temp->size >= start + size) {
            return temp;
        }
        *prev = temp;
        temp = temp->next;
    }

    return NULL;
}

int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    if (handler == NULL || name == NULL) {
        perror("Erreur: gestionnaire de mémoire ou nom invalide\n");
        return -1;
    }

    Segment *prev = NULL;
    Segment *seg = find_free_segment(handler, start, size, &prev);

    if (seg == NULL) {
        printf("Erreur: segment libre non trouvé\n");
        return 0;
    }

    Segment *s = (Segment *)malloc(sizeof(Segment));
    if (s == NULL) {
        perror("Erreur d'allocation mémoire pour Segment\n");
        return -1;
    }
    s->start = start;
    s->size = size;
    s->next = NULL;

    int seg_start = seg->start;
    int seg_size = seg->size;

    if (seg_start == start) {
        if (seg_size == size) {
            if (prev) {
                prev->next = seg->next;
            } else {
                handler->free_list = seg->next;
            }
            free(seg);
        } else {
            seg->start = seg_start + size;
            seg->size = seg_size - size;
        }
    } else if (seg_start < start && start + size < seg_start + seg_size) {
        Segment *new_free = (Segment *)malloc(sizeof(Segment));
        if (new_free == NULL) {
            perror("Erreur d'allocation mémoire pour Segment\n");
            free(s);
            return -1;
        }
        new_free->start = start + size;
        new_free->size = (seg_start + seg_size) - (start + size);
        new_free->next = seg->next;

        seg->size = start - seg_start;

        seg->next = new_free;
    } else if (seg_start < start) {
        seg->size = start - seg_start;
    }

    if (hashmap_insert(handler->allocated, name, s) == -1) {
        free(s);
        return -1;
    }

    return 1;
}

int remove_segment(MemoryHandler *handler, const char *name) {
    if (handler == NULL || name == NULL) {
        perror("Erreur: gestionnaire de mémoire ou nom invalide\n");
        return -1;
    }

    Segment *s = (Segment *)hashmap_get(handler->allocated, name);
    if (s == NULL) {
        printf("Erreur: segment non trouvé\n");
        return -1;
    }
    if (hashmap_remove(handler->allocated, name) != 1) {
        printf("Erreur\n");
        return -1;
    }

    Segment *prev = NULL;
    Segment *current = handler->free_list;

    while (current != NULL && current->start < s->start) {
        prev = current;
        current = current->next;
    }

    s->next = current;
    if (prev == NULL) {
        handler->free_list = s;
    } else {
        prev->next = s;
    }

    if (s->next != NULL && s->start + s->size == s->next->start) {
        s->size += s->next->size;
        Segment *temp = s->next;
        s->next = temp->next;
        free(temp);
    }

    if (prev != NULL && prev->start + prev->size == s->start) {
        prev->size += s->size;
        prev->next = s->next;
        free(s);
    }

    return 1;
}

void memory_destroy(MemoryHandler *handler) {
    if (handler == NULL) {
        return;
    }

    Segment *temp = handler->free_list;
    while (temp != NULL) {
        Segment *next = temp->next;
        free(temp);
        temp = next;
    }

    hashmap_destroy(handler->allocated);
    free(handler->memory);
    free(handler);
}

int getSegFreePos(MemoryHandler *handler, int size) {
    if (handler == NULL || size <= 0) {
        perror("Erreur: gestionnaire de mémoire ou taille invalide\n");
        return -1;
    }
    Segment *cour = handler->free_list;
    while (cour != NULL) {
        if (cour->size >= size) {
            return cour->start;
        }
        cour = cour->next;
    }
    return -1;
}
