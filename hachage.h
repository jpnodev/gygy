#ifndef hachage
#define hachage

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#define TABLE_SIZE 128
#define TOMBSTONE ((void *)-1)

// Cette structure représente un élément de la table de hachage.  
// Elle contient une clé (`key`), générée à partir des données de l’élément, permettant de 
// le rechercher dans le tableau,  
// ainsi qu’une valeur (`value`), qui stocke l’élément de n'importe quel type.
typedef struct hashentry {
    char *key ;
    void *value ;
} HashEntry;


// Ce structure contient un pointer de notre tableux d'hachage et sa size
typedef struct hashmap {
    int size;
    HashEntry *table;
} HashMap;


// Cette fonction implémente une fonction de hachage basée sur une chaîne de caractères passée en argument.  
// Elle calcule la somme des valeurs ASCII des lettres de cette chaîne pour générer une clé de hachage.
// Renvoi 0 si str est vide ou valeur de hachage
unsigned long simple_hash(const char *str);

// Cette fonction initialise la structure HashMap avec 0 comme des element dans tab avec taille TABLE_SIZE 
// Renvoi NULL en cas d'erreur ou pointer vers structure crée 
HashMap *hashmap_create();

// La fonction 'hashmap_insert' ajoute une nouvelle paire ('key', 'value') dans la table de hachage,  
// en utilisant le probing linéaire pour gérer les collisions.  
// Elle génère la valeur de hachage de la clé, recherche un emplacement disponible (vide ou marqué `TOMBSTONE`)  
// et insère le nouvel élément dans la table.
// Renvoi -1 en cas d'erreur ou 1 si élement est bien inserer
int hashmap_insert(HashMap *map, const char *key, void *value);

// Cette fonction effectue une recherche d’un élément dans une table de hachage  
// en explorant le tableau et en vérifiant les entrées en cas de collision.  
// Elle retourne `NULL` si l’élément n’est pas trouvé, en cas d’erreur,  
// ou bien la valeur associée si la clé est présente.
void *hashmap_get(HashMap *map, const char *key);

// Cette fonction libere toutes les données de tableux hachage
void hashmap_destroy(HashMap *map);

// Cette fonction supprime un élément de la table de hachage en utilisant une clé de hachage.  
// Retourne -1 en cas d'erreur, 0 si l'élément n'a pas été trouvé, et 1 si la suppression a été réussie.
int hashmap_remove(HashMap *map, const char *key);
#endif