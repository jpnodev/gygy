#include "memory.h"
#include "hachage.h"
#include "debug.h"
#include <stdio.h>

int main(void) {
    MemoryHandler *handler = memory_init(128);
    if (!handler) {
        printf("Failed to initialize memory.\n");
        return 1;
    }

    display_memory(handler);

    // Try finding a free segment
    Segment *prev = NULL;
    // Segment *seg = find_free_segment(handler, 0, 10, &prev);
    // if (seg) {
    //     printf("Found a free segment starting at %d of size %d.\n", seg->start, seg->size);
    // } else {
    //     printf("No suitable free segment found.\n");
    // }

    // Test the hashmap functionality
    HashMap *map = hashmap_create();
    if (!map) {
        printf("Failed to create hashmap.\n");
        return 1;
    }

    // Insert a key-value pair
    if (hashmap_insert(map, "hello", "world") == 1) {
        printf("Inserted ('hello','world') into hashmap.\n");
    }

    // display_hashmap(map);
    display_memory(handler);

    // Retrieve a value
    char *val = (char *) hashmap_get(map, "hello");
    if (val) {
        printf("Retrieved value: %s\n", val);
    } else {
        printf("Failed to retrieve value.\n");
    }

    // Clean up
    hashmap_destroy(map);
    // Normally, free full memory structures here

    

    return 0;
}