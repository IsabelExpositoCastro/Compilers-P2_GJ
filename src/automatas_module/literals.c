#include "literals.h"
#include <string.h>   // for strlen if you want
#include <stdlib.h>

// Alphabet: all allowed characters inside literals + the quote itself (needed for start/end)
#define LITERAL_ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\t"

// States
#define STATE_START   0   // before opening quote
#define STATE_IN      1   // inside literal
#define STATE_ACCEPT  2   // after closing quote

automaton2_t* create_literal_automaton(void) {
    automaton2_t* automaton = create_empty_automaton();
    if (!automaton) return NULL;

    // 1) Alphabet
    set_alphabet(automaton, LITERAL_ALPHABET);

    // 2) States: 3 total, 1 accepting
    set_num_states(automaton, 3, 1);

    // 3) Initial
    set_initial_state(automaton, STATE_START);

    // 4) Accepting
    int accepting_states[] = { STATE_ACCEPT };
    set_accepting_states(automaton, accepting_states, 1);

    // 5) Transitions (matrix already exists and is initialized to -1)
    int quote_idx = find_symbol_index(automaton, '"');
    if (quote_idx == -1) {
        // Should not happen because LITERAL_ALPHABET includes "
        return automaton; // or free_automaton(automaton); return NULL;
    }

    // Start: must read opening quote
    automaton->transition_matrix[STATE_START][quote_idx] = STATE_IN;

    // Inside:
    // - if we see a quote -> accept (closing quote)
    automaton->transition_matrix[STATE_IN][quote_idx] = STATE_ACCEPT;

    // - any other valid symbol stays inside
    for (int i = 0; i < automaton->alphabet_size; i++) {
        if (i != quote_idx) {
            automaton->transition_matrix[STATE_IN][i] = STATE_IN;
        }
    }

    // Accept state: no transitions (stay -1)

    // 6) Category
    set_category(automaton, CAT_LITERAL);

    // 7) Validate
    validate_automaton(automaton);

    return automaton;
}
