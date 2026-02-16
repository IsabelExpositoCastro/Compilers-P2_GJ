// --------------- LIBRARIES ---------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../automatas_module/automatonDefinition.h"

#define MAX_LENGTH 256


// ============ ESTRUCTURA PARA ESTADO DE AUTÓMATA ============
typedef struct {
    automaton* automaton;   // Puntero al autómata correspondiente
    int current_state;      // Estado actual en el autómata
    int is_active;          // ¿Sigue activo?
} automaton_state_t;


// ============ ESTRUCTURA PARA CANDIDATO DE TOKEN ============
typedef struct {
    char buffer[MAX_LENGTH];        // Buffer para almacenar el lexema del token
    int length;                     // Longitud del lexema
    int category;                   // Categoría del token
    int is_valid;                  // ¿Es un token válido?
} token_candidate_t;


// ============ ESTRUCTURA DE CONTEXTO DEL SCANNER ============
typedef struct {
    FILE* input_file;       // Archivo de entrada
    FILE* output_file;      // Archivo de salida
    int current_char;       // Carácter actual leído
    int lookahead_char;     // Carácter de lookahead (si se ha leído, sino LOOKAHEAD_NOT_READ)
    int line_num;           // Número de línea actual (inicia en 1)
    int col_num;            // Número de columna actual (inicia en 0)
} scanner_context_t;


// --------------- FUNCTION PROTOTYPES ---------------
void printCasual();
void StartScanner(FILE* InputFile, FILE* OutputFile, FILE* Autfile,char* input_filename);

// Funciones internas del scanner
int peek_char(scanner_context_t* ctx);
int read_char(scanner_context_t* ctx);
int can_any_automaton_continue(automaton_state_t* states, int num_automata, char next_char);
void initialize_automaton_states(automaton_state_t* states, automaton* automata, int num_automata);
void reset_automaton_states(automaton_state_t* states, int num_automata);
