#ifndef memorydef
#define memorydef

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hachage.h"

// Cette structure représente un segment de mémoire ou un intervalle
// numérique et est utilisée pour créer une liste chaînée de segments.
// Exemple: Segment seg1 = {100, 20, NULL};
typedef struct segment {
    int start; 
    int size; 
    struct segment *next; 
} Segment;

// Structure pour la gestion de la mémoire
typedef struct memoryHandler {
    void **memory;       // Pointeur vers un tableau de pointeurs représentant la mémoire principale
    int total_size;      // Taille totale de la mémoire en octets
    Segment* free_list;  // Liste chaînée des segments de mémoire libres
    HashMap* allocated;  // Table de hachage des segments alloués (clé - nom du segment, valeur - pointeur vers le segment)
} MemoryHandler;

// Cette fonction initialise la structure MemoryHandler. 
// Le pointeur memory est initialisé à NULL, et la taille est définie selon l'argument de la fonction.
// L'ensemble de la mémoire est ajouté à free_list, et une table de hachage est créée avec la fonction hashmap_create.
// Retourne NULL en cas d'erreur, sinon un pointeur vers la structure initialisée.
MemoryHandler *memory_init(int size);

// Cette fonction recherche dans free_list un segment disponible pour stocker des données 
// de taille size à partir de l'adresse start. 
// Elle conserve également dans prev le pointeur vers le segment précédent. 
// Retourne NULL en cas d'erreur, sinon un pointeur vers la structure trouvée.
Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev);

// Cette fonction stocke des données dans un segment trouvé à l'aide de la fonction find_free_segment.  
// Elle gère différents cas, réalloue correctement la mémoire, et met à jour la table de hachage allocated  
// ainsi que free_list en ajoutant de nouveaux segments si nécessaire.  
// Renvoie -1 en cas d'erreur, 0 si aucun segment n'est trouvé, et 1 si l'opération s'est bien déroulée.
int create_segment(MemoryHandler *handler, const char *name, int start, int size); 

// Cette fonction recherche un segment dans la table de hachage en utilisant name (avec hashmap_get)  
// et réorganise la mémoire en supprimant ce segment.  
// Si nécessaire, elle fusionne les segments adjacents pour optimiser l'espace libre.  
// Renvoie -1 en cas d'erreur et 1 en cas de succès.
int remove_segment(MemoryHandler *handler, const char *name);

// Cette fonction libère toute la mémoire associée au gestionnaire de mémoire (MemoryHandler).
void memory_destroy(MemoryHandler* handler);

int getSegFreePos(MemoryHandler* handler, int size);
#endif