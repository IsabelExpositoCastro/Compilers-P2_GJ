#include "special_characters.h"
#include <stdlib.h>
#include <string.h>

// ============ DEFINICIÓN DEL AUTÓMATA DE CARACTERES ESPECIALES ============

#define SPECIAL_CHARS "();{}[],"
#define SPECIAL_CHARS_COUNT 8

// Estados
#define STATE_INITIAL 0
#define STATE_ACCEPT 1

automaton2_t* create_special_char_automaton() {
    automaton2_t* automaton = create_empty_automaton();
    if (!automaton) return NULL;

    // 1. ASIGNAR ALFABETO
    set_alphabet(automaton, SPECIAL_CHARS);

    // 2. ASIGNAR NÚMERO DE ESTADOS (2 estados: inicial y aceptación)
    //    num_accepting = 1 (solo q_accept es de aceptación)
    set_num_states(automaton, 2, 1);

    // 3. ASIGNAR ESTADO INICIAL
    set_initial_state(automaton, STATE_INITIAL);

    // 4. ASIGNAR ESTADOS DE ACEPTACIÓN
    int accepting_states[] = {STATE_ACCEPT};
    set_accepting_states(automaton, accepting_states, 1);

    // 5. ASIGNAR MATRIZ DE TRANSICIÓN
    //    Matriz: [2 estados][8 símbolos]
    //    q0 → todos los símbolos van a q_accept
    //    q1 → no tiene transiciones válidas (error)
    
    // Llenar la matriz que ya está asignada en set_num_states
    for (int i = 0; i < SPECIAL_CHARS_COUNT; i++) {
        automaton->transition_matrix[STATE_INITIAL][i] = STATE_ACCEPT;
        automaton->transition_matrix[STATE_ACCEPT][i] = -1;
    }

    // 6. ASIGNAR CATEGORÍA
    set_category(automaton, CAT_SPECIALCHAR);

    // 7. VALIDAR
    validate_automaton(automaton);

    return automaton;
}
