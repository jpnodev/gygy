#ifndef parser
#define parser

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hachage.h"


// Cette structure est utilisée pour enregistrer une commande exécutée en pseudo-assembleur.  
// Elle se compose de trois parties : l'instruction elle-même, le premier et le deuxième opérande,  
// qui peuvent varier en fonction de la fonction à réaliser.
typedef struct {
    char *mnemonic; // Instruction mnemonic (ou nom de variable pour .DATA)
    char *operand1; // Premier operande (ou type pour .DATA)
    char *operand2; // Second operande (ou initialisation pour .DATA)
} Instruction;


// Cette structure est utilisée pour stocker le résultat de l'analyse du pseudo-assembleur.  
// Elle contient les instructions des sections .DATA et .CODE sous forme de tableaux  
// de pointeurs vers la structure Instruction, ainsi que le nombre d'instructions.  
// Les champs labels et memory_locations sont des tables de hachage contenant  
// respectivement les informations sur les étiquettes et les variables.
typedef struct {
    Instruction ** data_instructions; // Tableau d'instructions de la section .DATA
    int data_count; // Nombre d'instructions dans la section .DATA
    Instruction ** code_instructions; // Tableau d'instructions de la section .CODE
    int code_count; // Nombre d'instructions dans la section .CODE
    HashMap* labels; // Association des étiquettes avec leurs indices dans code_instructions
    HashMap* memory_locations; // Association des noms de variables avec leurs adresses mémoire
} ParserResult;

// Cette fonction initialise la structure ParserResult, deux structures data_instructions et code_instructions.
// Elle les met à NULL, data_count et code_count à 0 (parce qu'on n'a aucune commande à l'initialisation).
// Après, on initialise labels et memory_locations avec la fonction hashmap_create().
// Renvoie un pointeur vers la structure initialisée.
ParserResult *init();
// Cette fonction analyse une ligne de la section .DATA et retourne une structure Instruction.
// Elle alloue de la mémoire pour une nouvelle instruction, après lit la ligne et 
// extrait les trois champs : mnemonic, operand1, operand2.
// Si operand2 contient plusieurs éléments séparés par des virgules, elle les compte pour connaître
// combien de valeurs seront nécessaires en mémoire. (mais garde dans mémoire comme un tab)
// Elle assigne une adresse mémoire de départ (taille) à la variable,
// puis met à jour la taille totale nécessaire pour le segment.
// Enfin, elle insère le nom de la variable et son adresse dans la table de hachage memory_locations.
// Renvoie un pointeur vers la structure Instruction créée, ou NULL en cas d'erreur.
Instruction *parse_data_instruction(const char *line, HashMap *memory_locations);
// Cette fonction analyse une ligne de la section .CODE et retourne une structure Instruction.
// Elle vérifie si la ligne contient une étiquette (séparée par ':'), et si oui,
// elle l'associe à une adresse courante (code_count) dans la table de hachage memory_locations.
// Ensuite, elle extrait le mnémonique (instruction) et jusqu'à deux opérandes (operand1, operand2),
// en utilisant strtok ou sscanf selon la présence d'une étiquette.
// En cas d'erreur d'allocation mémoire retourne NULL.
// Renvoie un pointeur vers la structure Instruction construite.
Instruction *parse_code_instruction(const char *line, HashMap *memory_locations, int code_count);
// Cette fonction ouvre un fichier assembleur et analyse son contenu ligne par ligne.
// Elle commence par créer un objet FILE pour lire le fichier et alloue une structure ParserResult à l’aide de la fonction init().
// Initialement, elle alloue de la mémoire pour 10 instructions dans les sections .DATA et .CODE.
// Ensuite, elle lit chaque ligne du fichier :
//  - Si la ligne commence par ".DATA", elle active le mode de lecture de la section DATA.
//  - Si la ligne commence par ".CODE", elle active le mode de lecture de la section CODE.
// Pour chaque ligne non vide ou non commentée :
//  - Si on est dans la section .DATA, elle appelle parse_data_instruction pour analyser la ligne,
//    stocke l’instruction dans le tableau data_instructions, et double la capacité du tableau si nécessaire.
//  - Si on est dans la section .CODE, elle appelle parse_code_instruction, stocke le résultat dans code_instructions,
//    et double la capacité du tableau si besoin.
// En cas d'erreur d'allocation mémoire ou d'ouverture de fichier, elle libère les ressources et retourne NULL.
// Sinon, elle retourne un pointeur vers la structure ParserResult remplie.
ParserResult *parse(const char *filename);
// Cette fonction affiche une liste d'instructions assembleur.
// Elle prend en paramètre un tableau de pointeurs vers des structures Instruction, ainsi que leur nombre.
// Si la liste est vide ou NULL, elle affiche un message d'avertissement.
// Sinon, elle parcourt chaque instruction et affiche son mnémonique, son opérande 1,
// et son opérande 2 (si ce dernier existe), en les encadrant de guillemets.
void afficher_instructions(Instruction **liste, int count);
// Cette fonction libère la mémoire allouée pour une instruction.
// Elle prend en paramètre un pointeur vers une structure Instruction.
// Si le pointeur est NULL, elle ne fait rien.
// Sinon, elle libère la mémoire allouée dynamiquement pour les champs mnemonic, operand1, operand2,
// puis libère la mémoire de la structure elle-même.
void free_parser_result(ParserResult *result);
// Cette fonction libère toute la mémoire associée à une structure ParserResult.
// Elle prend en paramètre un pointeur vers ParserResult.
// Si le pointeur est NULL, elle ne fait rien.
// Sinon, elle libère d'abord chaque instruction de la section .DATA à l'aide de liberer_instruction,
// puis le tableau data_instructions lui-même.
// Ensuite, elle fait de même pour la section .CODE : libère chaque instruction,
// puis libère le tableau code_instructions.
// Elle détruit également les deux tables de hachage : labels et memory_locations.
// Enfin, elle libère la structure ParserResult elle-même.
void liberer_instruction(Instruction* i);

#endif