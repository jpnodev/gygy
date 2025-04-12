#include "debug.h"
#include "hachage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    HashMap *map = hashmap_create(SIMPLE);
    if (map == NULL) {
        printf("Erreur de creation de hashmap\n");
        return -1;
    }
    printf("hashmap créée\n");

    hashmap_insert(map, "key1", "value1");
    hashmap_insert(map, "key2", "value2");
    hashmap_insert(map, "key3", "value3");

    printf("hashmap inséréée\n");

    display_hashmap(map);

    char *str = hashmap_get(map, "key1");

    printf("Valeur de key1: %s\n", str);
    hashmap_remove(map, "key1");

    printf("hashmap après suppression de key1\n");

    display_hashmap(map);

    hashmap_remove(map, "key2");
    printf("hashmap après suppression de key2\n");
    display_hashmap(map);

    // hashmap_remove(map, "key3");
    hashmap_destroy(map);
    return 0;
}