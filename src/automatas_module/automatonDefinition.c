// ----------------- LIBRERÍAS -----------------
#include "automatonDefinition.h"
#include <stdlib.h>
#include <string.h>


// ============ FUNCIONES DE INICIALIZACIÓN Y CONFIGURACIÓN ============
automaton2_t* create_empty_automaton() {
    automaton2_t* automaton = (automaton2_t*)malloc(sizeof(automaton2_t));
    if (!automaton) return NULL;

    // Inicializar todo a NULL/0
    automaton->alphabet = NULL;
    automaton->alphabet_size = 0;
    automaton->states = NULL;
    automaton->num_states = 0;
    automaton->initial_state = -1;
    automaton->accepting_states = NULL;
    automaton->num_accepting_states = 0;
    automaton->transition_matrix = NULL;
    automaton->category = CAT_NONRECOGNIZED;
    automaton->category_name = "CAT_NONRECOGNIZED";
    return automaton;
}

void set_category_name(automaton2_t* automaton, char* category_name) {
    if (!automaton || !category_name) return;
    if (automaton->category_name) {
        free(automaton->category_name);
    }
    int len = strlen(category_name);
    automaton->category_name = (char*)malloc((len + 1) * sizeof(char));
    if (automaton->category_name) {
        strcpy(automaton->category_name, category_name);
    }
}

void set_alphabet(automaton2_t* automaton, const char* alphabet) {
    if (!automaton || !alphabet) return;

    // Liberar alfabeto anterior si existe
    if (automaton->alphabet) {
        free(automaton->alphabet);
    }

    // Copiar nuevo alfabeto
    automaton->alphabet_size = strlen(alphabet);
    automaton->alphabet = (char*)malloc((automaton->alphabet_size + 1) * sizeof(char));
    if (automaton->alphabet) {
        strcpy(automaton->alphabet, alphabet);
    }
}

void set_num_states(automaton2_t* automaton, int num_states, int num_accepting) {
    if (!automaton || num_states <= 0) return;

    // Liberar memoria anterior si existe
    if (automaton->states) {
        free(automaton->states);
    }
    if (automaton->transition_matrix) {
        for (int i = 0; i < automaton->num_states; i++) {
            if (automaton->transition_matrix[i]) {
                free(automaton->transition_matrix[i]);
            }
        }
        free(automaton->transition_matrix);
    }

    // Asignar nuevos valores
    automaton->num_states = num_states;
    automaton->num_accepting_states = num_accepting;

    // Alocar memoria para estados
    automaton->states = (int*)malloc(num_states * sizeof(int));
    if (automaton->states) {
        for (int i = 0; i < num_states; i++) {
            automaton->states[i] = i;  // Estados numerados 0, 1, 2, ...
        }
    }

    // Alocar memoria para matriz de transición
    // Matriz de tamaño [num_states][alphabet_size]
    if (automaton->alphabet_size > 0) {
        automaton->transition_matrix = (int**)malloc(num_states * sizeof(int*));
        if (automaton->transition_matrix) {
            for (int i = 0; i < num_states; i++) {
                automaton->transition_matrix[i] = (int*)malloc(automaton->alphabet_size * sizeof(int));
                if (automaton->transition_matrix[i]) {
                    // Inicializar a -1 (estado error)
                    for (int j = 0; j < automaton->alphabet_size; j++) {
                        automaton->transition_matrix[i][j] = -1;
                    }
                }
            }
        }
    }
}

void set_initial_state(automaton2_t* automaton, int initial) {
    if (!automaton) return;
    if (initial >= 0 && initial < automaton->num_states) {
        automaton->initial_state = initial;
    }
}

void set_accepting_states(automaton2_t* automaton, const int* accepting_states, int num) {
    if (!automaton || !accepting_states || num <= 0) return;

    // Liberar aceptación anterior si existe
    if (automaton->accepting_states) {
        free(automaton->accepting_states);
    }

    // Copiar nuevos estados de aceptación
    automaton->num_accepting_states = num;
    automaton->accepting_states = (int*)malloc(num * sizeof(int));
    if (automaton->accepting_states) {
        memcpy(automaton->accepting_states, accepting_states, num * sizeof(int));
    }
}

void set_transition_matrix(automaton2_t* automaton, int** matrix) {
    if (!automaton || !matrix) return;

    // Copiar matriz de transición
    if (automaton->transition_matrix) {
        for (int i = 0; i < automaton->num_states; i++) {
            if (automaton->transition_matrix[i]) {
                memcpy(automaton->transition_matrix[i], matrix[i], 
                       automaton->alphabet_size * sizeof(int));
            }
        }
    }
}

void set_category(automaton2_t* automaton, category_t category) {
    if (!automaton) return;
    automaton->category = category;
}

int validate_automaton(automaton2_t* automaton) {
    if (!automaton) return 0;

    // Validar que todos los campos necesarios estén configurados
    if (!automaton->alphabet || automaton->alphabet_size <= 0) {
        fprintf(stderr, "[VALIDATE] Error: Alfabeto no configurado\n");
        return 0;
    }

    if (!automaton->states || automaton->num_states <= 0) {
        fprintf(stderr, "[VALIDATE] Error: Estados no configurados\n");
        return 0;
    }

    if (automaton->initial_state < 0 || automaton->initial_state >= automaton->num_states) {
        fprintf(stderr, "[VALIDATE] Error: Estado inicial inválido\n");
        return 0;
    }

    if (!automaton->accepting_states || automaton->num_accepting_states <= 0) {
        fprintf(stderr, "[VALIDATE] Error: Estados de aceptación no configurados\n");
        return 0;
    }

    if (!automaton->transition_matrix) {
        fprintf(stderr, "[VALIDATE] Error: Matriz de transición no configurada\n");
        return 0;
    }

    fprintf(stderr, "[VALIDATE] Autómata válido\n");
    return 1;
}

void free_automaton(automaton2_t* automaton) {
    if (!automaton) return;

    if (automaton->alphabet) {
        free(automaton->alphabet);
    }

    if (automaton->states) {
        free(automaton->states);
    }

    if (automaton->accepting_states) {
        free(automaton->accepting_states);
    }

    if (automaton->transition_matrix) {
        for (int i = 0; i < automaton->num_states; i++) {
            if (automaton->transition_matrix[i]) {
                free(automaton->transition_matrix[i]);
            }
        }
        free(automaton->transition_matrix);
    }

    free(automaton);
}


// ============ FUNCIONES COMUNES DE USO ============

int find_symbol_index(automaton2_t* automaton, char symbol) {
    if (!automaton || !automaton->alphabet) return -1;

    for (int i = 0; i < automaton->alphabet_size; i++) {
        if (automaton->alphabet[i] == symbol) {
            return i;
        }
    }
    return -1;
}

int is_accepting_state(automaton2_t* automaton, int state) {
    if (!automaton || !automaton->accepting_states) return 0;

    for (int i = 0; i < automaton->num_accepting_states; i++) {
        if (automaton->accepting_states[i] == state) {
            return 1;
        }
    }
    return 0;
}

void print_automaton_info(automaton2_t* automaton, const char* name) {
    if (!automaton) {
        printf("[AUTOMATON] %s: NULL\n", name);
        return;
    }

    printf("\n[AUTOMATON] %s\n", name);
    printf("  Alphabet: '%s' (size: %d)\n", automaton->alphabet ? automaton->alphabet : "NULL", automaton->alphabet_size);
    printf("  States: %d\n", automaton->num_states);
    printf("  Initial state: %d\n", automaton->initial_state);
    printf("  Accepting states: ");
    for (int i = 0; i < automaton->num_accepting_states; i++) {
        printf("%d ", automaton->accepting_states[i]);
    }
    printf("(count: %d)\n", automaton->num_accepting_states);
    printf("  Category: %d\n\n", automaton->category);
}
