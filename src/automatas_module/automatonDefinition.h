#ifndef AUTOMATON_DEFINITION_H
#define AUTOMATON_DEFINITION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CAT_NONRECOGNIZED -1 // Categoria reservada para tokens no reconocidos

// Automata definition structure.
typedef struct {
    char* alphabet;             // Alfabeto del autómata (conjunto de caracteres válidos)
    int alphabet_size;          // Tamaño del alfabeto
    int* states;                // Lista de TODOS los estados del autómata
    int num_states;             // Número total de estados
    int initial_state;          // Índice del estado inicial
    int* accepting_states;      // Índices de los estados de aceptación
    int num_accepting_states;   // Número de estados de aceptación
    int** transition_matrix;    // Matriz de transición: [estado][símbolo] = estado_destino
    char* category_name;        // Nombre de la categoría (ej: IDENTIFIER, KEYWORD, etc.)
    int category;               // Categoría del autómata (todos los tokens de este autómata = misma categoría)
    bool need_lookahead;        // Indica si el autómata requiere lookahead para decidir su aceptación
} automaton;


// ============ FUNCIONES COMUNES DE USO ============

// Buscar el índice de un símbolo en el alfabeto (-1 si no existe)
int find_symbol_index(automaton* automaton, char symbol);

// Verificar si un estado es de aceptación
int is_accepting_state(automaton* automaton, int state);

// Mostrar información del autómata (para debugging)
void print_automaton_info(automaton* automaton, const char* name);



void read_automatas(FILE* file, automaton* a);
automaton* generate_automatas(FILE* file, int* num_automatas);
void free_automaton(automaton* a);
void free_automatas(automaton* automatas, int num_automatas);

#endif // AUTOMATON_DEFINITION_H