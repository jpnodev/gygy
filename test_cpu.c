#include "cpu.h"
#include "debug.h"

#define MEMORY_SIZE 1024
int main() {
  int memory_size = MEMORY_SIZE;
  CPU* cpu = cpu_init(memory_size);

  if (cpu == NULL) {
    printf("Failed to initialize CPU.\n");
    return 1;
  }

  printf("CPU initialized successfully.\n");

  display_memory(cpu->memory_handler);
  display_hashmap(cpu->context);

  // Nettoyage memoire
  free(cpu->memory_handler);
  free(cpu->context);
  free(cpu);

  return 0;
}