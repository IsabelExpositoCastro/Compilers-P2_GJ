// ----------------- LIBRARIES -----------------
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
    int* states;                // Lista de TODOS los estados del autómata
    int initial_state;          // Índice del estado inicial
    int* accepting_states;      // Índices de los estados de aceptación
    int** transition_matrix;    // Matriz de transición: cada fila representa un estado, cada columna representa un símbolo del alfabeto, y el valor es el índice del estado al que se transita
    category_t* category;       // Categoría del autómata q se asignará al token una vez llegue a algún accepting state (End-State)
} automaton2_t;


// ----------------- FUNCTION PROTOTYPES -----------------
void initialize_automaton();