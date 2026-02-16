// --------------- LIBRARIES ---------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../automatas_module/automatonDefinition.h"


// ============ ESTRUCTURA PARA ESTADO DE AUTÓMATA ============
typedef struct {
    automaton* automaton;
    int current_state;      // Estado actual en el autómata
    int is_active;          // ¿Sigue activo?
} automaton_state_t;


// ============ ESTRUCTURA PARA CANDIDATO DE TOKEN ============
typedef struct {
    char buffer[256]; // <- Tremendo magic number.
    int length;
    int category;
    int is_valid;
} token_candidate_t;


// ============ ESTRUCTURA DE CONTEXTO DEL SCANNER ============
typedef struct {
    FILE* input_file;
    FILE* output_file;
    int current_char;
    int lookahead_char;
    int line_num;
    int col_num;
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
