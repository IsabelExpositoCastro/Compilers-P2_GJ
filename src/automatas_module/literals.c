


#include "automatonDefinition.h"  // For automaton functions and types
#include <stdlib.h>
#include <string.h>



// ----------------- LITERAL AUTOMATON -----------------

#define LITERAL_ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\t"  // Alphabet for literal characters

// Function to create the literal automaton (DFA)
automaton2_t* create_literal_automaton() {
    automaton2_t* automaton = create_empty_automaton();
    if (!automaton) return NULL;

    // Set the alphabet: All characters inside the literal except the closing quote
    set_alphabet(automaton, LITERAL_ALPHABET);

    //set_category_name(automaton, "LITERAL");

    // Set up the number of states: 3 states (0 - initial, 1 - inside literal, 2 - accepting)
    set_num_states(automaton, 3, 1);  // 3 states and 1 accepting state
    set_initial_state(automaton, 0);  // The initial state is 0

    // The accepting state is state 2
    int accepting_states[] = {2};  // Only state 2 is the accepting state
    set_accepting_states(automaton, accepting_states, 1);

    // Set up the transition matrix
    int** matrix = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        matrix[i] = (int*)malloc(automaton->alphabet_size * sizeof(int));
        for (int j = 0; j < automaton->alphabet_size; j++) {
            matrix[i][j] = -1;  // Set all transitions to invalid (-1) by default
        }
    }

    // Transitions for State 0 (Initial state)
    // From state 0, upon encountering the opening quote ("), go to state 1
    matrix[0][find_symbol_index(automaton, '"')] = 1;  // State 0 -> State 1 (start literal)

    // Transitions for State 1 (Inside literal)
    // From state 1, upon encountering the closing quote ("), go to state 2 (accepting state)
    matrix[1][find_symbol_index(automaton, '"')] = 2;  // State 1 -> State 2 (end literal)
    
    // From state 1, any valid character inside the literal should stay in state 1
    for (int i = 0; i < automaton->alphabet_size; i++) {
        if (i != find_symbol_index(automaton, '"')) {
            matrix[1][i] = 1;  // Stay in state 1 for all characters except closing quote
        }
    }

    // State 2: No transitions from the accepting state (it’s the final state)
    // This state is the accepting state once the closing quote is encountered.
    // No transitions needed from this state.

    // Set the transition matrix
    set_transition_matrix(automaton, matrix);

    // Free temporary transition matrix
    for (int i = 0; i < 3; i++) {
        free(matrix[i]);
    }
    free(matrix);

    // Set category for this automaton (literal)
    set_category(automaton, CAT_LITERAL);

    validate_automaton(automaton);

    return automaton;
}

// ----------------- PROCESS LITERAL FUNCTION -----------------

// Function to process literals using the automaton
// void process_literal(scanner_context_t* ctx) {
//     // Initialize the literals automaton
//     automaton2_t* literal_automaton = create_literal_automaton();
    
//     // Initialize automaton states (the states are set up when creating the automaton)
//     automaton_state_t states[MAX_AUTOMATA];
//     initialize_automaton_states(states, &literal_automaton, 1);

//     char c = read_char(ctx); // First character after the opening quote
//     while (c != EOF && c != '\n' && c != '"') {
//         // Continue processing the characters inside the literal
//         process_automata_transition(states, 1, c);  // Consume the character in the automaton
//         c = read_char(ctx);  // Read next character
//     }

//     // Check if we found the closing quote
//     if (c == '"') {
//         // Successfully found the closing quote, accept token
//         emit_token(CAT_LITERAL);
//     } else {
//         // Error: unclosed literal or invalid character inside
//         emit_error("Unclosed literal or invalid character");
//     }
// }
