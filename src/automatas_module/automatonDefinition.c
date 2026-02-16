#include "automatonDefinition.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../counter_module/counter.h"
#include "../preprocesor_variables.h"
#include "../error_handler_module/error_handler.h"
#define MAX_LINE 256
#define NO_NULL_STATE -1

// ============ FUNCIONES COMUNES DE USO ============

int find_symbol_index(automaton* automaton, char symbol) {
    if (!automaton || !automaton->alphabet || automaton->alphabet_size <= 0) return -1;

    for (int i = 0; i < automaton->alphabet_size; i++) {
        COUNT_COMP_N(1);
        if (automaton->alphabet[i] == symbol) {
            COUNT_COMP_N(1);
            return i;
        }
    }
    return -1;
}

int is_accepting_state(automaton* automaton, int state) {
    if (!automaton || !automaton->accepting_states || automaton->num_accepting_states <= 0) return 0;

    for (int i = 0; i < automaton->num_accepting_states; i++) {
        COUNT_COMP_N(1);
        if (automaton->accepting_states[i] == state) {
            COUNT_COMP_N(1);
            return 1;
        }
    }
    return 0;
}

void print_automaton_info(automaton* automaton, const char* name) {
    if (!automaton) {
        printf("[AUTOMATON] %s: NULL\n", name);
        return;
    }

    printf("\n+---- [AUTOMATON] %s ----+\n", name);

    // Category
    printf("| Category: %s (ID: %d)\n",
           automaton->category_name ? automaton->category_name : "NULL",
           automaton->category);

    // Alphabet
    printf("| Alphabet: '%s' (size: %d)\n",
           automaton->alphabet ? automaton->alphabet : "NULL",
           automaton->alphabet_size);

    // States
    printf("| States (%d): ", automaton->num_states);
    for (int i = 0; i < automaton->num_states; i++) {
        printf("%d", automaton->states[i]);
        if (i < automaton->num_states - 1) printf(", ");
    }
    printf("\n");

    // Initial state
    printf("| Initial state: %d\n", automaton->initial_state);

    // Accepting states
    printf("| Accepting states (%d): ", automaton->num_accepting_states);
    for (int i = 0; i < automaton->num_accepting_states; i++) {
        printf("%d", automaton->accepting_states[i]);
        if (i < automaton->num_accepting_states - 1) printf(", ");
    }
    printf("\n");

    // Null state
    printf("| Null state (error): %d\n", automaton->null_state);

    // Transition matrix header
    printf("| Transition Matrix:\n");
    printf("|   ");
    for (int j = 0; j < automaton->alphabet_size; j++) {
        printf("%c ", automaton->alphabet[j]);
    }
    printf("\n");

    // Transition matrix data
    for (int i = 0; i < automaton->num_states; i++) {
        printf("| %d:", automaton->states[i]);
        for (int j = 0; j < automaton->alphabet_size; j++) {
            printf(" %d", automaton->transition_matrix[i][j]);
        }
        printf("\n");
    }

    printf("+----------------------------------------------------+\n");
}



automaton* generate_automatas(FILE* file,int* num_automatas){
    fscanf(file, "%d", num_automatas);
    COUNT_IO_N(1);
    // Consumir el newline después del número
    char dummy[MAX_LINE];
    fgets(dummy, MAX_LINE, file);
    COUNT_IO_N(strlen(dummy));

    automaton* automatas = malloc((*num_automatas) * sizeof(automaton));
    COUNT_GEN_N(1);
    for (int k = 0; k < (*num_automatas); k++) {
        COUNT_COMP_N(1);
        read_automatas(file, &automatas[k]);
        automatas[k].category = k; //Category -1 is reserved to CAT_NONRECONIZED
        COUNT_GEN_N(1);
    }
    return automatas;
}
// listautomatas[returnedvalue].category_name = "IDENTIFIER" por ejemplo, y asi con las demas categorias.
/*
Number of automatas: 1

alphabet: abc
num_states: 3
states: 0 1 2
initial: 0
num_accepting_states: 1
accepting states: 1
category: CAT_CATEGORY
transitions: states x symbols
        a b c
state0: 1 2 0
state1: 2 0 1
state2: 0 1 2
null state: 2
*/
void read_automatas(FILE* file, automaton* a, FILE* output) {
    char line[MAX_LINE];
    
    // Consumir línea vacía demarcador
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    // ---------- ALPHABET ---------- (The alphabet must be without spaces " " to work well)
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    char alphabet_str[MAX_LINE];
    sscanf(line, "%s", alphabet_str);
    COUNT_GEN_N(1);
    
    a->alphabet_size = strlen(alphabet_str);
    if (a->alphabet_size == 0) {
        error_report(SCANNER_ERR_INVALID_ALPHABET, STEP_SCANNER, __FILE__, __LINE__, output);
        return;
    }
    COUNT_GEN_N(1);
    a->alphabet = malloc(a->alphabet_size + 1);
    COUNT_GEN_N(1);
    strcpy(a->alphabet, alphabet_str);
    COUNT_GEN_N(1);

    // ---------- NUM STATES ----------
    fgets(line, MAX_LINE, file); // num_states: 3
    COUNT_IO_N(strlen(line));
    char* token = strtok(line, " \n");
    COUNT_GEN_N(1);
    a->num_states = atoi(token);
    COUNT_GEN_N(1);

    //----------- STATES --------------
    a->states = malloc(a->num_states * sizeof(int));
    COUNT_GEN_N(1);  
    fgets(line, MAX_LINE, file); // states: 0 1 2
    COUNT_IO_N(strlen(line));
    token = strtok(line, " \n");
    COUNT_GEN_N(1);
    for(int i = 0; i < a->num_states;i++) {
        COUNT_COMP_N(1);
        a->states[i] = atoi(token);
        COUNT_GEN_N(1);
        token = strtok(NULL, " \n");
        COUNT_GEN_N(1);
    }

    // ---------- INITIAL STATE ---------- 
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    sscanf(line, "%d", &a->initial_state);
    COUNT_GEN_N(1);

    // ---------- NUMBER ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    sscanf(line, "%d", &a->num_accepting_states);
    COUNT_GEN_N(1);
    
    // ---------- ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file); // accepting: 2
    COUNT_IO_N(strlen(line));
    token = strtok(line, " \n");
    COUNT_GEN_N(1);
    a->accepting_states = malloc(a->num_accepting_states * sizeof(int));
    COUNT_GEN_N(1);

    for(int i = 0; i< a->num_accepting_states; i++) {
        COUNT_COMP_N(1);
        a->accepting_states[i] = atoi(token);
        COUNT_GEN_N(1);
        token = strtok(NULL, " \n");
        COUNT_GEN_N(1);
    }

    // ---------- CATEGORY ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    char cat[MAX_LINE];
    sscanf(line, "%s", cat);
    COUNT_IO_N(strlen(line));
    a->category_name = malloc(strlen(cat) + 1);
    COUNT_GEN_N(1);
    strcpy(a->category_name, cat);
    COUNT_GEN_N(1);

    // ---------- TRANSITIONS ----------
    a->transition_matrix = malloc(a->num_states * sizeof(int*));
    COUNT_GEN_N(1);
    for (int i = 0; i < a->num_states; i++) {
        COUNT_COMP_N(1);
        a->transition_matrix[i] = malloc(a->alphabet_size * sizeof(int));
        COUNT_GEN_N(1);
    }

    for (int i = 0; i < a->num_states; i++) {
        COUNT_COMP_N(1);
        fgets(line, MAX_LINE, file);
        COUNT_IO_N(strlen(line));
        token = strtok(line, " \n");
        COUNT_GEN_N(1);
        for (int j = 0; j < a->alphabet_size; j++) {
            COUNT_COMP_N(1);
            if (token != NULL) {
                COUNT_COMP_N(1);
                a->transition_matrix[i][j] = atoi(token);
                COUNT_GEN_N(1);
                token = strtok(NULL, " \n");
                COUNT_GEN_N(1);
            }
        }
    }
    
    // ---------- NULL STATE ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO_N(strlen(line));
    if (strcmp(line, "null\n") == 0) {
        COUNT_COMP_N(1);
        a->null_state = NO_NULL_STATE; // If it is the case that there is no null state.
    } else {
        sscanf(line, "%d", &a->null_state);
        COUNT_IO_N(strlen(line));
    }
}


void free_automaton(automaton* a) {
    if (!a) return;

    // Alphabet
    if (a->alphabet) {
        COUNT_COMP_N(1);
        free(a->alphabet);
        a->alphabet = NULL;
    }

    // States
    if (a->states) {
        COUNT_COMP_N(1);
        free(a->states);
        a->states = NULL;
    }

    // Accepting states
    if (a->accepting_states) {
        COUNT_COMP_N(1);
        free(a->accepting_states);
        a->accepting_states = NULL;
    }

    // Transition matrix
    if (a->transition_matrix) {
        COUNT_COMP_N(1);
        for (int i = 0; i < a->num_states; i++) {
            COUNT_COMP_N(1);
            if (a->transition_matrix[i]) {
                COUNT_COMP_N(1);
                free(a->transition_matrix[i]);
            }
        }
        free(a->transition_matrix);
        a->transition_matrix = NULL;
    }

    // Category name
    if (a->category_name) {
        COUNT_COMP_N(1);
        free(a->category_name);
        a->category_name = NULL;
    }
}

void free_automatas(automaton* automatas, int num_automatas) {
    if (!automatas) return;

    for (int i = 0; i < num_automatas; i++) {
        COUNT_COMP_N(1);
        free_automaton(&automatas[i]);
    }

    free(automatas);
}