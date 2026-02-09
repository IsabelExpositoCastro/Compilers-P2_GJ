// --------------- LIBRARIES ---------------
#include "scanner.h"
#include "../automatas_module/special_characters.h"


// --------------- DEFINITIONS ---------------
#define CHAR_NULL '\0'
#define MAX_AUTOMATA 10
#define TOKEN_BUFFER_SIZE 256
#define DEFAULT_TOKEN {0}


// ============ VARIABLE GLOBAL: TODOS LOS AUTÓMATAS ============
static automaton2_t* all_automata[MAX_AUTOMATA];
static automaton2_t* lookahead_automata[MAX_AUTOMATA];
static int num_automata = 0;


// ============ INICIALIZACIÓN DE AUTÓMATAS ============
void initialize_all_automata() {
    // Crear autómatas específicos
    all_automata[0] = create_special_char_automaton();
    lookahead_automata[0] = create_special_char_automaton();
    // TODO: Agregar más autómatas (keywords, identifiers, numbers, etc.)
    
    num_automata = 1;  // Por ahora solo tenemos SPECIAL_CHARACTERS

    //all_automata[1] = create_literal_automaton();
    //lookahead_automata[1] = create_literal_automaton();
    //num_automata++;

    
    printf("[SCANNER] Initialized %d automata\n", num_automata);
}

void free_all_automata() {
    for (int i = 0; i < num_automata; i++) {
        if (all_automata[i]) {
            free_automaton(all_automata[i]);
            all_automata[i] = NULL;
        }
        if (lookahead_automata[i]) {
            free_automaton(lookahead_automata[i]);
            lookahead_automata[i] = NULL;
        }
    }
    num_automata = 0;
}


// ============ FUNCIONES DE MANEJO DE ENTRADA ============

int peek_char(scanner_context_t* ctx) {
    if (ctx->lookahead_char == -2) {  // No leído aún
        ctx->lookahead_char = fgetc(ctx->input_file);
    }
    return ctx->lookahead_char;
}

int read_char(scanner_context_t* ctx) {
    int c;
    if (ctx->lookahead_char != -2) {  // Ya hay lookahead
        c = ctx->lookahead_char;
        ctx->lookahead_char = -2;
    } else {
        c = fgetc(ctx->input_file);
    }
    
    if (c == '\n') {
        ctx->line_num++;
        ctx->col_num = 0;
    } else if (c != EOF) {
        ctx->col_num++;
    }
    
    return c;
}


// ============ INICIALIZACIÓN DE ESTADOS DE AUTÓMATAS ============

void initialize_automaton_states(automaton_state_t* states, automaton2_t** automata, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        states[i].automaton = automata[i];
        states[i].current_state = automata[i]->initial_state;
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
        if (symbol_idx == -1) {
            // Símbolo no en alfabeto
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        if (next_state != -1) {
            // Transición válida
            return 1;
        }
    }
    return 0;  // Ninguno puede continuar
}


// ============ PROCESAMIENTO DE TRANSICIONES ============

void process_automata_transition(automaton_state_t* states, int num_auto, char c) {
    int any_active = 0;
    
    for (int i = 0; i < num_auto; i++) {
        if (!states[i].is_active) continue;
        
        // Buscar símbolo en alfabeto
        int symbol_idx = find_symbol_index(states[i].automaton, c);
        
        if (symbol_idx == -1) {
            // Símbolo NO en alfabeto → muere
            states[i].is_active = 0;
            continue;
        }
        
        // Intentar transición
        int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
        
        if (next_state == -1) {
            // Transición inválida → muere
            states[i].is_active = 0;
            continue;
        }
        
        // Transición válida
        states[i].current_state = next_state;
        any_active = 1;
    }
}


// ============ BÚSQUEDA DE AUTÓMATA ACEPTOR ============

int find_accepting_automaton(automaton_state_t* states, int num_auto) {
    for (int i = 0; i < num_auto; i++) {
        if (states[i].is_active && is_accepting_state(states[i].automaton, states[i].current_state)) {
            return i;
        }
    }
    return -1;  // Ninguno
}


// ============ FUNCIÓN PRINCIPAL DEL SCANNER ============

void StartScanner(FILE* InputFile, FILE* OutputFile) {
    if (!InputFile || !OutputFile) {
        fprintf(stderr, "[ERROR] Scanner: Invalid input or output file\n");
        return;
    }

    // Inicializar autómatas
    initialize_all_automata();
    
    // Contexto del scanner
    scanner_context_t ctx = {
        .input_file = InputFile,
        .output_file = OutputFile,
        .current_char = -1,
        .lookahead_char = -2,
        .line_num = 1,
        .col_num = 0
    };

    // Estados de autómatas
    automaton_state_t states[MAX_AUTOMATA];
    automaton_state_t lookahead_states[MAX_AUTOMATA];
    
    initialize_automaton_states(states, all_automata, num_automata);
    initialize_automaton_states(lookahead_states, lookahead_automata, num_automata);
    
    char token_buffer[TOKEN_BUFFER_SIZE] = DEFAULT_TOKEN;
    int token_pos = 0;
    
    token_candidate_t last_valid = {
        .buffer = DEFAULT_TOKEN,
        .length = 0,
        .category = CAT_NONRECOGNIZED,
        .is_valid = 1
    };
    int c;

    while ((c = read_char(&ctx)) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if(last_valid.length > 0) {
                fprintf(OutputFile, "<%s, %d> ", token_buffer, last_valid.category);
            }
            for (int i = 0; i < last_valid.length; i++) {
                token_buffer[i] = '\0';
                last_valid.buffer[i] = '\0';
            }
            token_pos = 0;
            last_valid.length = 0;
            last_valid.category = CAT_NONRECOGNIZED;
            last_valid.is_valid = 0;
            reset_automaton_states(states, num_automata);
            if (c == '\n') {
                fprintf(OutputFile, "\n");
                ctx.line_num++;
            }
        }else {
            int next_char = peek_char(&ctx);

            if(can_any_automaton_continue(states, num_automata, c)) {
                if(!last_valid.is_valid){
                    if(last_valid.length > 1) {
                        fprintf(OutputFile, "<%s, %d> ", last_valid.buffer, last_valid.category);
                    }
                    for (int i = 0; i < last_valid.length; i++) {
                        token_buffer[i] = '\0';
                        last_valid.buffer[i] = '\0';
                    }
                    token_pos = 0;
                    last_valid.length = 0;
                    last_valid.category = CAT_NONRECOGNIZED;
                    last_valid.is_valid = 0;
                }
                process_automata_transition(states, num_automata, (char)c);
                token_buffer[token_pos] = (char)c;
                token_pos++;
                last_valid.buffer[token_pos] = '\0';
                last_valid.length = token_pos;
                last_valid.is_valid = 1;
                int accepting_idx = find_accepting_automaton(states, num_automata);
                if (accepting_idx != -1) {
                    strncpy(last_valid.buffer, token_buffer, token_pos);
                    last_valid.buffer[token_pos] = '\0';
                    last_valid.category = states[accepting_idx].automaton->category;
                    last_valid.is_valid = 1;
                    fprintf(OutputFile, "<%s, %d> ", last_valid.buffer, last_valid.category);
                    for (int i = 0; i < last_valid.length; i++) {
                        token_buffer[i] = '\0';
                        last_valid.buffer[i] = '\0';
                    }
                    token_pos = 0;
                    last_valid.length = 0;
                    last_valid.category = CAT_NONRECOGNIZED;
                    last_valid.is_valid = 0;
                    reset_automaton_states(states, num_automata);
                }
            }else{ //aqui ampliar
                token_buffer[token_pos] = c;
                token_pos++;
                last_valid.buffer[token_pos] = '\0';
                last_valid.length = token_pos;
                last_valid.category = CAT_NONRECOGNIZED;
                last_valid.is_valid = 0;
                //if(can_any_automaton_continue(lookahead_automata, num_automata, next_char));
            }
        }
    }
    free_all_automata();
}


// ============ FUNCIÓN DE PRUEBA ============
void printCasual() {
    printf("=== SCANNER TEST ===\n");
    printf("Testing SPECIAL CHARACTERS automaton:\n\n");
    
    // Inicializar un autómata de prueba
    automaton2_t* special = create_special_char_automaton();
    
    if (special) {
        print_automaton_info(special, "SPECIAL_CHARACTERS");
        
        // Probar algunos caracteres
        const char* test_chars = "();{}[],abc";
        printf("Testing characters:\n");
        for (int i = 0; test_chars[i] != '\0'; i++) {
            char c = test_chars[i];
            int symbol_idx = find_symbol_index(special, c);
            printf("  '%c' -> ", c);
            if (symbol_idx != -1) {
                int next_state = special->transition_matrix[special->initial_state][symbol_idx];
                printf("Symbol found (idx=%d) -> Next state: %d", symbol_idx, next_state);
                if (is_accepting_state(special, next_state)) {
                    printf(" [ACCEPTING]\n");
                } else {
                    printf("\n");
                }
            } else {
                printf("NOT IN ALPHABET\n");
            }
        }
        
        free_automaton(special);
    }
}
