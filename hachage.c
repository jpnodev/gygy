#include "debug.h"
#include "hachage.h"
#include "memory.h"

unsigned long simple_hash(const char *str) {
    if (str == NULL)
        return 0;

    unsigned long res = 0;
    int i = 0;

    while (str[i] != '\0') {
        res += str[i];
        i++;
    }

    return res;
}

HashMap *hashmap_create(hashmap_value_t type) {
    if (type != SIMPLE && type != BASIC_MALLOC && type != SEGMENT) {
        perror("Erreur: type de hachage non valide\n");
        return NULL;
    }

    HashMap *map = malloc(sizeof(HashMap));
    if (map == NULL) {
        perror("Erreur d'allocation de la mémoire\n");
        return NULL;
    }

    map->type = type;
    map->size = TABLE_SIZE;
    map->table = calloc(map->size, sizeof(HashEntry));

    if (!map->table) {
        perror("Erreur d'allocation de la mémoire\n");
        free(map);
        return NULL;
    }

    for (int i = 0; i < map->size; i++) {
        map->table[i].key = NULL;
        map->table[i].value = NULL;
    }
    return map;
}

int hashmap_insert(HashMap *map, const char *key, void *value) {
    if (map == NULL || key == NULL) {
        perror("Erreur: la table de hachage est NULL\n");
        return -1;
    }

    unsigned long i = simple_hash(key) % map->size;

    for (int j = 0; j < map->size; j++) {
        unsigned long index = (i + j) % map->size;

        if (map->table[index].key == NULL || map->table[index].key == TOMBSTONE) {
            if (map->table[index].key == TOMBSTONE) {
                if (map->table[index].value != NULL) {
                    free(map->table[index].value);
                }
            }

            map->table[index].key = strdup(key);
            map->table[index].value = value;
            return 1;
        }
    }

    return -1;
}

void *hashmap_get(HashMap *map, const char *key) {
    if (map == NULL || key == NULL) {
        perror("Erreur: la table de hachage est NULL\n");
        return NULL;
    }

    unsigned long i = simple_hash(key) % map->size;

    for (int j = 0; j < map->size; j++) {
        unsigned long index = (i + j) % map->size;

        if (map->table[index].key == NULL) {
            return NULL;
        }

        if (map->table[index].key != TOMBSTONE && strcmp(map->table[index].key, key) == 0) {
            return map->table[index].value;
        }
    }

    return NULL;
}

int hashmap_remove(HashMap *map, const char *key) {
    if (map == NULL || key == NULL) {
        perror("Erreur: la table de hachage est NULL\n");
        return -1;
    }

    unsigned long i = simple_hash(key) % map->size;

    for (int j = 0; j < map->size; j++) {
        unsigned long index = (i + j) % map->size;

        if (map->table[index].key == NULL) {
            return 0;
        }

        if (map->table[index].key != TOMBSTONE && strcmp(map->table[index].key, key) == 0) {
            free(map->table[index].key);

            // Suppression dèla valeur selon son type
            switch (map->type) {
            case SIMPLE:
                break;
            case BASIC_MALLOC:
                if (map->table[index].value != NULL) {
                    free(map->table[index].value);
                }
                break;
            case SEGMENT:
                // Le segment est déjà libéré dans la fonction remove_segment
                // Il ne faut pas appeler hashmap_remove seule sur un segment
                break;
            default:
                perror("Erreur: type de donnée hachée non valide\n");
                break;
            }

            map->table[index].key = TOMBSTONE;
            map->table[index].value = NULL;
            return 1;
        }
    }
    return 0;
}

void hashmap_destroy(HashMap *map) {
    if (map == NULL) {
        perror("Erreur: la table de hachage est NULL\n");
        return;
    }
    for (int i = 0; i < map->size; i++) {
        if (map->table[i].key != NULL && map->table[i].key != TOMBSTONE) {
            if (map->table[i].key != NULL)
                free(map->table[i].key);

            // Suppression de la valeur selon son type
            switch (map->type) {
            case SIMPLE:
                break;
            case BASIC_MALLOC:
                if (map->table[i].value != NULL) {
                    free(map->table[i].value);
                }
                break;
            case SEGMENT:
                // printf("libération des segments en hashmap
                if (map->table[i].value != NULL) {
                    Segment *s = (Segment *)map->table[i].value;
                    while (s != NULL) {
                        Segment *next = s->next;
                        free(s);
                        s = next;
                    }
                }
                break;
            default:
                perror("Erreur: type de donnée hachée non valide\n");
                break;
            }
            map->table[i].value = NULL;
        }
    }

    if (map->table != NULL)
        free(map->table);
    if (map != NULL)
        free(map);
}
