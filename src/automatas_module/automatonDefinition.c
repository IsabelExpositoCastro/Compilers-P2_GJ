// ----------------- LIBRERÍAS -----------------
#include "automatonDefinition.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

// ============ FUNCIONES COMUNES DE USO ============

int find_symbol_index(automaton* automaton, char symbol) {
    if (!automaton || !automaton->alphabet || automaton->alphabet_size <= 0) return -1;

    for (int i = 0; i < automaton->alphabet_size; i++) {
        if (automaton->alphabet[i] == symbol) {
            return i;
        }
    }
    return -1;
}

int is_accepting_state(automaton* automaton, int state) {
    if (!automaton || !automaton->accepting_states || automaton->num_accepting_states <= 0) return 0;

    for (int i = 0; i < automaton->num_accepting_states; i++) {
        if (automaton->accepting_states[i] == state) {
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

    printf("\n[AUTOMATON] %s\n", name);

    // Alphabet
    printf("  Alphabet: '%s' (size: %d)\n",
           automaton->alphabet ? automaton->alphabet : "NULL",
           automaton->alphabet_size);

    // States
    printf("  States (%d): ", automaton->num_states);
    for (int i = 0; i < automaton->num_states; i++) {
        printf("%d ", automaton->states[i]);
    }
    printf("\n");

    // Initial
    printf("  Initial state: %d\n", automaton->initial_state);

    // Accepting
    printf("  Accepting states (%d): ", automaton->num_accepting_states);
    for (int i = 0; i < automaton->num_accepting_states; i++) {
        printf("%d ", automaton->accepting_states[i]);
    }
    printf("\n");

    // Category
    printf("  Category ID: %d\n", automaton->category);
    printf("  Category name: %s\n",
           automaton->category_name ? automaton->category_name : "NULL");

    // Transition matrix
    printf("  Transition matrix:\n");
    for (int i = 0; i < automaton->num_states; i++) {
        printf("    ");
        for (int j = 0; j < automaton->alphabet_size; j++) {
            printf("%d ", automaton->transition_matrix[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}



automaton* generate_automatas(FILE* file,int* num_automatas){
    fscanf(file, "%d", num_automatas);
    // Consumir el newline después del número
    char dummy[MAX_LINE];
    fgets(dummy, MAX_LINE, file);

    automaton* automatas = malloc((*num_automatas) * sizeof(automaton));
    for (int k = 0; k < (*num_automatas); k++) {
        read_automatas(file, &automatas[k]);
        automatas[k].category = k+1; //Category 0 is reserved to CAT_NONRECONIZED
    }
    return automatas;
}

/*
Number of automatas: 1

alphabet: abc
num_states: 3
states: 0 1 2
initial: 0
num_accepting_states: 1
accepting states: 2
category: CAT_CATEGORY
transitions: states x symbols
1 2 0
2 0 1
0 1 2
*/
void read_automatas(FILE* file, automaton* a) {
    char line[MAX_LINE];
    
    // Consumir línea vacía demarcador
    fgets(line, MAX_LINE, file);

    // ---------- ALPHABET ---------- (The alphabet must be without spaces " " to work well)
    fgets(line, MAX_LINE, file);
    char alphabet_str[MAX_LINE];
    sscanf(line, "%s", alphabet_str);
    
    a->alphabet_size = strlen(alphabet_str);
    a->alphabet = malloc(a->alphabet_size + 1);
    strcpy(a->alphabet, alphabet_str);

    // ---------- NUM STATES ----------
    fgets(line, MAX_LINE, file); // num_states: 3
    char* token = strtok(line, " \n");
    a->num_states = atoi(token);

    //----------- STATES --------------
    a->states = malloc(a->num_states * sizeof(int));  
    fgets(line, MAX_LINE, file); // states: 0 1 2
    token = strtok(line, " \n");

    for(int i = 0; i < a->num_states;i++) {
        a->states[i] = atoi(token);
        token = strtok(NULL, " \n");
    }

    // ---------- INITIAL STATE ---------- 
    fgets(line, MAX_LINE, file);
    sscanf(line, "%d", &a->initial_state);

    // ---------- NUMBER ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file);
    sscanf(line, "%d", &a->num_accepting_states);
    
    // ---------- ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file); // accepting: 2
    token = strtok(line, " \n");
    a->accepting_states = malloc(a->num_accepting_states * sizeof(int));

    for(int i = 0; i< a->num_accepting_states; i++) {
        a->accepting_states[i] = atoi(token);
        token = strtok(NULL, " \n");
    }

    // ---------- CATEGORY ----------
    fgets(line, MAX_LINE, file);
    char cat[MAX_LINE];
    sscanf(line, "%s", cat);
    a->category_name = malloc(strlen(cat) + 1);
    strcpy(a->category_name, cat);

    // ---------- TRANSITIONS ---------
    // Reservar matriz
    a->transition_matrix = malloc(a->num_states * sizeof(int*));
    for (int i = 0; i < a->num_states; i++) {
        a->transition_matrix[i] = malloc(a->alphabet_size * sizeof(int));
    }

    // Leer filas
    for (int i = 0; i < a->num_states; i++) {
        fgets(line, MAX_LINE, file);
        token = strtok(line, " \n");

        for (int j = 0; j < a->alphabet_size; j++) {
            if (token != NULL) {
                a->transition_matrix[i][j] = atoi(token);
                token = strtok(NULL, " \n");
            }
        }
    }
}


void free_automaton(automaton* a) {
    if (!a) return;

    // Alphabet
    if (a->alphabet) {
        free(a->alphabet);
        a->alphabet = NULL;
    }

    // States
    if (a->states) {
        free(a->states);
        a->states = NULL;
    }

    // Accepting states
    if (a->accepting_states) {
        free(a->accepting_states);
        a->accepting_states = NULL;
    }

    // Transition matrix
    if (a->transition_matrix) {
        for (int i = 0; i < a->num_states; i++) {
            if (a->transition_matrix[i]) {
                free(a->transition_matrix[i]);
            }
        }
        free(a->transition_matrix);
        a->transition_matrix = NULL;
    }

    // Category name
    if (a->category_name) {
        free(a->category_name);
        a->category_name = NULL;
    }
}

void free_automatas(automaton* automatas, int num_automatas) {
    if (!automatas) return;

    for (int i = 0; i < num_automatas; i++) {
        free_automaton(&automatas[i]);
    }

    free(automatas);
}