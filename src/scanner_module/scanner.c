// --------------- LIBRARIES ---------------
#include "scanner.h"
#include "../automatas_module/automatonDefinition.h"
#include "../counter_module/counter.h"
#include <ctype.h>


// --------------- DEFINITIONS ---------------
#define CHAR_NULL '\0'
#define CHAR_NEWLINE '\n'
#define CHAR_SPACE ' '
#define CHAR_TAB '\t'
#define CHAR_CARRIAGE_RETURN '\r'
#define INVALID_STATE -1
#define INVALID_AUTOMATON_INDEX -1
#define LOOKAHEAD_NOT_READ -2
#define INITIAL_CHAR_VALUE -1
#define FIRST_LINE_NUMBER 1
#define FIRST_COLUMN_NUMBER 0
#define TOKEN_RESET_POS 0
#define MAX_AUTOMATA 20     // Limite de autómatas a cargar
#define TOKEN_BUFFER_SIZE 2560
#define TOKEN_BUFFER_LAST_INDEX (TOKEN_BUFFER_SIZE - 1)
#define DEFAULT_TOKEN {0}
#define UNREC "CAT_UNRECOGNIZED"


// ============ VARIABLE GLOBAL: TODOS LOS AUTÓMATAS ============
static automaton* all_automata = NULL;
static automaton* lookahead_automata[MAX_AUTOMATA]; // ?
static int num_automata = 0;


static void clear_token_buffer(char* token_buffer, int* token_pos) {
    for (int i = 0; i < *token_pos; i++) {
        token_buffer[i] = CHAR_NULL;
    }
    *token_pos = TOKEN_RESET_POS;
}




// ============ FUNCIONES DE MANEJO DE ENTRADA ============

int peek_char(scanner_context_t* ctx) {
    if (ctx->lookahead_char == LOOKAHEAD_NOT_READ) {  // No leído aún
        ctx->lookahead_char = fgetc(ctx->input_file);
    }
    return ctx->lookahead_char;
}

int read_char(scanner_context_t* ctx) {
    int c;
    if (ctx->lookahead_char != LOOKAHEAD_NOT_READ) {  // Ya hay lookahead
        c = ctx->lookahead_char;
        ctx->lookahead_char = LOOKAHEAD_NOT_READ;
    } else {
        c = fgetc(ctx->input_file);
    }
    
    if (c == CHAR_NEWLINE) {
        ctx->line_num++;
        ctx->col_num = FIRST_COLUMN_NUMBER;
    } else if (c != EOF) {
        ctx->col_num++;
    }
    
    return c;
}


// ============ INICIALIZACIÓN DE ESTADOS DE AUTÓMATAS ============

void initialize_automaton_states(automaton_state_t* states, automaton* automata, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        states[i].automaton = &automata[i];
        states[i].current_state = automata[i].initial_state;
        states[i].is_active = 1;
    }
}

void reset_automaton_states(automaton_state_t* states, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        states[i].current_state = states[i].automaton->initial_state;
        states[i].is_active = 1;
    }
}


// ============ LÓGICA DE CONTINUACIÓN DE AUTÓMATAS ============

int can_any_automaton_continue(automaton_state_t* states, int num_auto, char next_char) {
    for (int i = 0; i < num_auto; i++) {
        if (!states[i].is_active) continue;
        
        // Buscar índice del símbolo
        int symbol_idx = find_symbol_index(states[i].automaton, next_char);
        if (symbol_idx == INVALID_STATE) {
            // Símbolo no en alfabeto
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        if (next_state != INVALID_STATE) {
            // Transición válida
            return 1;
        }
    }
    return 0;  // Ninguno puede continuar
}

int can_any_automaton_accept_with_char(automaton_state_t* states, int num_auto, char next_char) {
    for (int i = 0; i < num_auto; i++) {
        if (!states[i].is_active) continue;

        int symbol_idx = find_symbol_index(states[i].automaton, next_char);
        if (symbol_idx == INVALID_STATE) {
            continue;
        }

        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        if (next_state != INVALID_STATE && is_accepting_state(states[i].automaton, next_state)) {
            return 1;
        }
    }
    return 0;
}


// ============ PROCESAMIENTO DE TRANSICIONES ============

int process_automata_transition(automaton_state_t* states, int num_auto, char c) {
    int any_active = 0;
    
    for (int i = 0; i < num_auto; i++) {
        if (!states[i].is_active) continue;
        
        // Buscar símbolo en alfabeto
        int symbol_idx = find_symbol_index(states[i].automaton, c);
        
        if (symbol_idx == INVALID_STATE) {
            // Símbolo NO en alfabeto → muere
            states[i].is_active = 0;
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        
        if (next_state == INVALID_STATE) {
            // Transición inválida → muere
            states[i].is_active = 0;
            continue;
        }
        
        // Transición válida
        states[i].current_state = next_state;
        any_active = 1;
    }
    return any_active;
}


// ============ BÚSQUEDA DE AUTÓMATA ACEPTOR ============

int find_accepting_automaton(automaton_state_t* states, int num_auto) {
    int best_index = INVALID_AUTOMATON_INDEX;
    int best_alphabet_size = 0;

    for (int i = 0; i < num_auto; i++) {
        if (!states[i].is_active) continue;
        if (!is_accepting_state(states[i].automaton, states[i].current_state)) continue;

        int current_alphabet_size = states[i].automaton->alphabet_size;
        if (best_index == INVALID_AUTOMATON_INDEX || current_alphabet_size < best_alphabet_size) {
            best_index = i;
            best_alphabet_size = current_alphabet_size;
        }
    }

    return best_index;  // Ninguno si sigue en INVALID_AUTOMATON_INDEX
}


// ============ FUNCIÓN PRINCIPAL DEL SCANNER ============

void StartScanner(FILE* InputFile, FILE* OutputFile, FILE* Automatafile, char* input_filename) {
    if (!InputFile || !OutputFile || !Automatafile) {
        // ADD ERROR
        return;
    }
    all_automata = generate_automatas(Automatafile, &num_automata);
    if (!all_automata || num_automata <= 0) {
        // ADD ERROR
        return;
    }
    if (num_automata > MAX_AUTOMATA) {
        // ADD ERROR
        free_automatas(all_automata, num_automata);
        return;
    }

    // Contexto del scanner
    scanner_context_t ctx = {
        .input_file = InputFile,
        .output_file = OutputFile,
        .current_char = INITIAL_CHAR_VALUE,
        .lookahead_char = LOOKAHEAD_NOT_READ,
        .line_num = FIRST_LINE_NUMBER,
        .col_num = FIRST_COLUMN_NUMBER
    };

    automaton_state_t states[MAX_AUTOMATA];

    initialize_automaton_states(states, all_automata, num_automata);
    // ADD ERROR
    
    char token_buffer[TOKEN_BUFFER_SIZE] = DEFAULT_TOKEN;
    int token_pos = TOKEN_RESET_POS;
    int c;
    int category;
    char* category_name;
    int next_char;
    int counter = 0;

    while ((c = read_char(&ctx)) != EOF) {
        if (c == CHAR_NEWLINE) {
            fprintf(OutputFile, "\n");
            ctx.line_num++;

            reset_automaton_states(states, num_automata);
            clear_token_buffer(token_buffer, &token_pos);
            counter++;
        }else if (c == CHAR_SPACE || c == CHAR_TAB || c == CHAR_CARRIAGE_RETURN) {
            reset_automaton_states(states, num_automata);
            clear_token_buffer(token_buffer, &token_pos);
        }else{
            int alivedfm = process_automata_transition(states, num_automata, c);
            if (token_pos < TOKEN_BUFFER_LAST_INDEX) {
                token_buffer[token_pos] = c;
                token_pos++;
                token_buffer[token_pos] = CHAR_NULL;
            }
            //printf("\n%i,%s  %c, %i",counter, token_buffer, c, alivedfm);
            if (alivedfm != 0)
            {
                int alivekw = find_accepting_automaton(states, num_automata);
                //printf("\n%i,  %c, %i, %i",counter, c, alivedfm, alivekw);
                if (alivekw != INVALID_AUTOMATON_INDEX){
                    next_char = peek_char(&ctx);
                    int alivedfmwithla = can_any_automaton_accept_with_char(states, num_automata, next_char);
                    if(alivedfmwithla != 0 && next_char !=EOF){
                        //printf("\n%i,  %c, %i, %i, %i",counter, c, alivedfm, alivekw, alivedfmwithla);
                        continue;
                    }else{
                        fprintf(OutputFile, "<%s, %s> ", token_buffer, states[alivekw].automaton->category_name );
                        clear_token_buffer(token_buffer, &token_pos);
                        reset_automaton_states(states, num_automata);
                    }
                }
            }else{
                next_char = peek_char(&ctx);
                reset_automaton_states(states, num_automata);
                int alivekw = process_automata_transition(states, num_automata, next_char);
                if (alivekw != 0)
                {
                    fprintf(OutputFile, "<%s, %s> ", token_buffer, UNREC);
                    clear_token_buffer(token_buffer, &token_pos);
                }
                reset_automaton_states(states, num_automata);
            }
        }
        
    }
    free_automatas(all_automata, num_automata);
}
