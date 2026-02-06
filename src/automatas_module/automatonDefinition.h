// ----------------- LIBRARIES -----------------
#ifndef AUTOMATON_DEFINITION_H
#define AUTOMATON_DEFINITION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ----------------- STRUCTS -----------------

typedef enum {      // Enumeración que tendrá cada token reconocido por el Scanner
    CAT_NUMBER,
    CAT_IDENTIFIER,
    CAT_KEYWORD,
    CAT_LITERAL,
    CAT_OPERATOR,
    CAT_SPECIALCHAR,
    CAT_NONRECOGNIZED
} category_t;



//NOTA: LA PRINCIIPAL DIFERÉNCIA ENTRE 1 OPCIÓN Y LA OTRA, ES SIMPLEMENTE MÁS O MENOS LEGIBLE A LA HORA DE CODEAR, PERO TMBIÉN MÁS MENOS JODIDO A LA HORA DE CODEAR CON MEMÓRIA

// OPCIÓN 1: Definición del autómata usando solo chars (OPCIÓN MEDIO LOKA AL USAR TANTOS PUNTEROS - COMO HAYA UN BUGG DE MEMÓRIA VAMOS A FLIPAR XD)
//typedef struct {
//    char* alphabet;             // Alfabeto del autómata (conjunto de caracteres válidos)
//    char** states;              // Lista de TODOS los estados del autómata
//    char* initial_state;        // Estado inicial
//    char** accepting_states;    // Estados de aceptación
//    char*** transition_matrix;  // Matriz de transición: cada fila representa un estado, cada columna representa un símbolo del alfabeto, y el valor es el estado al que se transita
//    category_t* category;       // Categoría del autómata q se asignará al token una vez llegue a algún accepting state (End-State)
//} automaton_t;



// OPCIÓN 2: Definición del autómata usando índices (OPCIÓN MÁS CLÁSICA Y SENCILLA DE IMPLEMENTAR, PERO MENOS LEGIBLE)
typedef struct {
    char* alphabet;             // Alfabeto del autómata (conjunto de caracteres válidos)
    int alphabet_size;          // Tamaño del alfabeto
    int* states;                // Lista de TODOS los estados del autómata
    int num_states;             // Número total de estados
    int initial_state;          // Índice del estado inicial
    int* accepting_states;      // Índices de los estados de aceptación
    int num_accepting_states;   // Número de estados de aceptación
    int** transition_matrix;    // Matriz de transición: [estado][símbolo] = estado_destino
    category_t category;        // Categoría del autómata (todos los tokens de este autómata = misma categoría)
} automaton2_t;


// ============ FUNCIONES DE INICIALIZACIÓN Y CONFIGURACIÓN ============

// 1. Crear un autómata vacío (sin memoria asignada)
automaton2_t* create_empty_automaton();

// 2. Asignar alfabeto al autómata
void set_alphabet(automaton2_t* automaton, const char* alphabet);

// 3. Asignar número de estados (esto aloca memoria para la matriz de transición)
void set_num_states(automaton2_t* automaton, int num_states, int num_accepting);

// 4. Asignar estado inicial
void set_initial_state(automaton2_t* automaton, int initial);

// 5. Asignar estados de aceptación
void set_accepting_states(automaton2_t* automaton, const int* accepting_states, int num);

// 6. Asignar matriz de transición
void set_transition_matrix(automaton2_t* automaton, int** matrix);

// 7. Asignar categoría del token
void set_category(automaton2_t* automaton, category_t category);

// 8. Validar que el autómata esté bien formado
int validate_automaton(automaton2_t* automaton);

// 9. Liberar memoria del autómata
void free_automaton(automaton2_t* automaton);



// ============ FUNCIONES COMUNES DE USO ============

// Buscar el índice de un símbolo en el alfabeto (-1 si no existe)
int find_symbol_index(automaton2_t* automaton, char symbol);

// Verificar si un estado es de aceptación
int is_accepting_state(automaton2_t* automaton, int state);

// Mostrar información del autómata (para debugging)
void print_automaton_info(automaton2_t* automaton, const char* name);


#endif // AUTOMATON_DEFINITION_H