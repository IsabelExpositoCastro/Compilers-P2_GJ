// --------------- LIBRARIES ---------------
#include "scanner.h"
#include "../automatas_module/automatonDefinition.h"
#include "../counter_module/counter.h"
#include "../preprocesor_variables.h"
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
        COUNT_COMP_N(1);
        token_buffer[i] = CHAR_NULL;
        COUNT_GEN_N(1);
    }
    *token_pos = TOKEN_RESET_POS;
    COUNT_GEN_N(1);
}

static void emit_unrecognized_if_pending(FILE* output_file, char* token_buffer, int* token_pos) {
    COUNT_COMP_N(1);
    if (*token_pos > 0) {
        fprintf(output_file, "<%s, %s> ", token_buffer, UNREC);
        COUNT_IO_N(1);
        clear_token_buffer(token_buffer, token_pos);
    }
}




// ============ FUNCIONES DE MANEJO DE ENTRADA ============

int peek_char(scanner_context_t* ctx) {
    COUNT_COMP_N(1);
    if (ctx->lookahead_char == LOOKAHEAD_NOT_READ) {  // No leído aún
        ctx->lookahead_char = fgetc(ctx->input_file);
        COUNT_IO_N(1);
        COUNT_GEN_N(1);
    }
    return ctx->lookahead_char;
}

int read_char(scanner_context_t* ctx) {
    int c;
    COUNT_COMP_N(1);
    if (ctx->lookahead_char != LOOKAHEAD_NOT_READ) {  // Ya hay lookahead
        c = ctx->lookahead_char;
        ctx->lookahead_char = LOOKAHEAD_NOT_READ;
        COUNT_GEN_N(2);
    } else {
        c = fgetc(ctx->input_file);
        COUNT_IO_N(1);
        COUNT_GEN_N(1);
    }
    
    COUNT_COMP_N(2);
    if (c == CHAR_NEWLINE) {
        ctx->line_num++;
        ctx->col_num = FIRST_COLUMN_NUMBER;
        COUNT_GEN_N(2);
        COUNT_IO_N(1);

    } else if (c != EOF) {
        ctx->col_num++;
        COUNT_GEN_N(1);
    }
    
    return c;
}


// ============ INICIALIZACIÓN DE ESTADOS DE AUTÓMATAS ============

void initialize_automaton_states(automaton_state_t* states, automaton* automata, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);
        states[i].automaton = &automata[i];
        states[i].current_state = automata[i].initial_state;
        states[i].is_active = 1;
        COUNT_GEN_N(3);
    }
}

void reset_automaton_states(automaton_state_t* states, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);
        states[i].current_state = states[i].automaton->initial_state;
        states[i].is_active = 1;
        COUNT_GEN_N(2);
    }
}


// ============ LÓGICA DE CONTINUACIÓN DE AUTÓMATAS ============

int can_any_automaton_continue(automaton_state_t* states, int num_auto, char next_char) {
    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);
        if (!states[i].is_active) continue;
        
        // Buscar índice del símbolo
        int symbol_idx = find_symbol_index(states[i].automaton, next_char);
        COUNT_GEN_N(1);
        if (symbol_idx == INVALID_STATE) {
            // Símbolo no en alfabeto
            COUNT_COMP_N(1);
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        COUNT_GEN_N(1);
        if (next_state != INVALID_STATE) {
            // Transición válida
            COUNT_COMP_N(1);
            return 1;
        }
    }
    return 0;  // Ninguno puede continuar
}

int can_any_automaton_accept_with_char(automaton_state_t* states, int num_auto, char next_char) {
    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);
        if (!states[i].is_active) continue;

        int symbol_idx = find_symbol_index(states[i].automaton, next_char);
        COUNT_GEN_N(1);
        if (symbol_idx == INVALID_STATE) {
            COUNT_COMP_N(1);
            continue;
        }

        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        COUNT_GEN_N(1);
        if (next_state != INVALID_STATE && is_accepting_state(states[i].automaton, next_state)) {
            COUNT_COMP_N(1);
            return 1;
        }
    }
    return 0;
}

int can_selected_automaton_continue_with_char(automaton_state_t* states, int automaton_index, char next_char) {
    COUNT_COMP_N(1);
    if (automaton_index == INVALID_AUTOMATON_INDEX) return 0;
    COUNT_COMP_N(1);
    if (!states[automaton_index].is_active) return 0;

    int symbol_idx = find_symbol_index(states[automaton_index].automaton, next_char);
    COUNT_GEN_N(1);
    COUNT_COMP_N(1);
    if (symbol_idx == INVALID_STATE) return 0;

    int next_state = states[automaton_index].automaton->transition_matrix[states[automaton_index].current_state][symbol_idx];
    COUNT_GEN_N(1);
    COUNT_COMP_N(1);
    return (next_state != INVALID_STATE);
}


// ============ PROCESAMIENTO DE TRANSICIONES ============

int process_automata_transition(automaton_state_t* states, int num_auto, char c) {
    int any_active = 0;
    COUNT_GEN_N(1);
    
    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);        // COunt del four
        COUNT_COMP_N(1);        // Count del if --> se q se puede hacer una sola llamada sumando 2, pèro asi es más visual jajja
        if (!states[i].is_active) continue;
        
        // Buscar símbolo en alfabeto
        int symbol_idx = find_symbol_index(states[i].automaton, c);
        COUNT_GEN_N(1);
        
        COUNT_COMP_N(1);
        if (symbol_idx == INVALID_STATE) {
            // Símbolo NO en alfabeto → muere
            states[i].is_active = 0;
            COUNT_GEN_N(1);
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        COUNT_GEN_N(1);
        
        COUNT_COMP_N(1);
        if (next_state == INVALID_STATE) {
            // Transición inválida → muere
            states[i].is_active = 0;
            COUNT_GEN_N(1);
            continue;
        }
        
        // Transición válida
        states[i].current_state = next_state;
        any_active = 1;
        COUNT_GEN_N(2);
    }
    return any_active;
}


// ============ BÚSQUEDA DE AUTÓMATA ACEPTOR ============

int find_accepting_automaton(automaton_state_t* states, int num_auto) {
    int best_index = INVALID_AUTOMATON_INDEX;
    int best_alphabet_size = 0;
    COUNT_GEN_N(2);

    for (int i = 0; i < num_auto; i++) {
        COUNT_COMP_N(1);
        COUNT_COMP_N(1);
        if (!states[i].is_active) continue;
        COUNT_COMP_N(1);
        if (!is_accepting_state(states[i].automaton, states[i].current_state)) continue;

        int current_alphabet_size = states[i].automaton->alphabet_size;
        COUNT_GEN_N(1);
        COUNT_COMP_N(2);        // Son 2 comparaciones pq es o un caso o el otro (OR case)
        if (best_index == INVALID_AUTOMATON_INDEX || current_alphabet_size < best_alphabet_size) {
            best_index = i;
            best_alphabet_size = current_alphabet_size;
            COUNT_GEN_N(2);
        }
    }

    return best_index;  // Ninguno si sigue en INVALID_AUTOMATON_INDEX
}


// ============ FUNCIÓN PRINCIPAL DEL SCANNER ============

void StartScanner(FILE* InputFile, FILE* OutputFile, FILE* Automatafile, char* input_filename) {
    COUNT_COMP_N(3);
    if (!InputFile || !OutputFile || !Automatafile) {
        // ADD ERROR
        return;
    }
    all_automata = generate_automatas(Automatafile, &num_automata);
    COUNT_GEN_N(1);
    COUNT_IO_N(1);
    COUNT_COMP_N(2);
    if (!all_automata || num_automata <= 0) {
        // ADD ERROR
        return;
    }
    COUNT_COMP_N(1);
    if (num_automata > MAX_AUTOMATA) {
        // ADD ERROR
        free_automatas(all_automata, num_automata);
        COUNT_GEN_N(1);
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
    COUNT_GEN_N(1);
    // ADD ERROR
    
    char token_buffer[TOKEN_BUFFER_SIZE] = DEFAULT_TOKEN;
    int token_pos = TOKEN_RESET_POS;
    int c;
    int category;
    char* category_name;
    int next_char;
    int counter = 0;
    COUNT_GEN_N(7);

    
    while ((c = read_char(&ctx)) != EOF) {
        COUNT_IO_N(1);
        COUNT_COMP_N(2);
        if (c == CHAR_NEWLINE) {
            emit_unrecognized_if_pending(OutputFile, token_buffer, &token_pos);
            fprintf(OutputFile, "\n");
            COUNT_IO_N(1);
            ctx.line_num++;
            COUNT_GEN_N(1);

            reset_automaton_states(states, num_automata);
            clear_token_buffer(token_buffer, &token_pos);
            counter++;
        
            COUNT_GEN_N(1);

        
        }else if (c == CHAR_SPACE || c == CHAR_TAB || c == CHAR_CARRIAGE_RETURN) {
            emit_unrecognized_if_pending(OutputFile, token_buffer, &token_pos);
            reset_automaton_states(states, num_automata);
            clear_token_buffer(token_buffer, &token_pos);
            COUNT_COMP_N(3);

        }else{
            int alivedfm = process_automata_transition(states, num_automata, c);
            COUNT_GEN_N(1);
            COUNT_COMP_N(1);
            if (token_pos < TOKEN_BUFFER_LAST_INDEX) {
                token_buffer[token_pos] = c;
                token_pos++;
                token_buffer[token_pos] = CHAR_NULL;
                COUNT_GEN_N(3);
            }
            //printf("\n%i,%s  %c, %i",counter, token_buffer, c, alivedfm);
            COUNT_COMP_N(1);
            if (alivedfm != 0)
            {
                int alivekw = find_accepting_automaton(states, num_automata);
                COUNT_GEN_N(1);
                COUNT_COMP_N(1);
                //printf("\n%i,  %c, %i, %i",counter, c, alivedfm, alivekw);
                if (alivekw != INVALID_AUTOMATON_INDEX){
                    next_char = peek_char(&ctx);
                    int selected_can_continue = can_selected_automaton_continue_with_char(states, alivekw, next_char);
                    COUNT_GEN_N(2);
                    COUNT_COMP_N(1);
                    if(selected_can_continue != 0 && next_char !=EOF){
                        //printf("\n%i,  %c, %i, %i, %i",counter, c, alivedfm, alivekw, alivedfmwithla);
                        continue;
                    }else{
                        fprintf(OutputFile, "<%s, %s> ", token_buffer, states[alivekw].automaton->category_name );
                        COUNT_IO_N(1);
                        clear_token_buffer(token_buffer, &token_pos);
                        reset_automaton_states(states, num_automata);
                    }
                }
            }else{
                emit_unrecognized_if_pending(OutputFile, token_buffer, &token_pos);
                reset_automaton_states(states, num_automata);
            }
        }
        
    }
    emit_unrecognized_if_pending(OutputFile, token_buffer, &token_pos);
    write_counters(OutputFile, input_filename);
    COUNT_IO_N(1);
    free_automatas(all_automata, num_automata);
    COUNT_GEN_N(1);
}
