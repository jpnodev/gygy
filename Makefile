# ============================
# Paramètres du compilateur
# ============================

CC = gcc                          # Le compilateur
CFLAGS = -Wall -Wextra -g        # Options de compilation (avertissements + debug)

# ============================
# Définition des fichiers sources et objets
# ============================

SRC = $(wildcard *.c)            # Tous les fichiers .c du dossier
OBJ = $(SRC:.c=.o)               # Tous les fichiers objets correspondants

# ============================
# Exécutables
# ============================

MAIN_EXEC = main                 # Exécutable principal
TEST_HACHAGE_EXEC = test_hachage
TEST_MEMORY_EXEC = test_memory
TEST_PARSER_EXEC = test_parser
TEST_CPU_EXEC = test_cpu
TEST_CPU_ADRESSING_EXEC = test_cpu_adressing

# ============================
# Règle par défaut (appelée par "make")
# ============================

all: $(MAIN_EXEC)
run_parser: $(TEST_PARSER_EXEC)
	./$(TEST_PARSER_EXEC)

run_cpu: $(TEST_CPU_EXEC)
	./$(TEST_CPU_EXEC)

run_cpu_adressing: $(TEST_CPU_ADRESSING_EXEC)
	./$(TEST_CPU_ADRESSING_EXEC)

# ============================
# Compilation de l'exécutable principal
# ============================

$(MAIN_EXEC): debug.o hachage.o memory.o parser.o cpu.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

# ============================
# Compilation des exécutables de test
# ============================

$(TEST_HACHAGE_EXEC): debug.o hachage.o memory.o parser.o test_hachage.o
	$(CC) -o $@ $^

$(TEST_MEMORY_EXEC): debug.o hachage.o memory.o parser.o test_memory.o
	$(CC) -o $@ $^

$(TEST_PARSER_EXEC): debug.o hachage.o memory.o parser.o cpu.o test_parser.o
	$(CC) -o $@ $^

$(TEST_CPU_EXEC): debug.o hachage.o memory.o cpu.o test_cpu.o parser.o
	$(CC) -o $@ $^

$(TEST_CPU_ADRESSING_EXEC): debug.o hachage.o memory.o cpu.o test_cpu_adressing.o 
	$(CC) -o $@ $^

# ============================
# Règle de compilation générique pour tous les fichiers .c
# ============================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ============================
# Nettoyage des fichiers objets et exécutables
# ============================

clean:
	rm -f *.o $(MAIN_EXEC) $(TEST_HACHAGE_EXEC) $(TEST_MEMORY_EXEC) $(TEST_PARSER_EXEC) $(TEST_CPU_EXEC) $(TEST_CPU_ADRESSING_EXEC)

# Nettoyage complet (fichiers temporaires en plus)
mrproper: clean
	rm -f *~ .depend

# ============================
# Génération des dépendances automatiques
# ============================

depend:
	$(CC) -MM $(SRC) > .depend

-include .depend

# ============================
# Raccourci pour exécuter le programme principal
# ============================

run: $(MAIN_EXEC)
	./$(MAIN_EXEC)
