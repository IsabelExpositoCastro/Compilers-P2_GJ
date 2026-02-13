#include "automatonDefinition.h"
#include "../counter_module/counter.h"
#include "../preprocesor_variables.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

// ============ FUNCIONES COMUNES DE USO ============

int find_symbol_index(automaton* automaton, char symbol) {
    if (!automaton || !automaton->alphabet || automaton->alphabet_size <= 0) {
        COUNT_COMP();  // Comparación en el if
        return -1;
    }

    for (int i = 0; i < automaton->alphabet_size; i++) {
        COUNT_COMP();  // Comparación en cada iteración del loop
        if (automaton->alphabet[i] == symbol) {
            COUNT_COMP();  // Comparación del símbolo
            return i;
        }
    }
    return -1;
}

int is_accepting_state(automaton* automaton, int state) {
    if (!automaton || !automaton->accepting_states || automaton->num_accepting_states <= 0) {
        COUNT_COMP();  // Comparación en el if
        return 0;
    }

    for (int i = 0; i < automaton->num_accepting_states; i++) {
        COUNT_COMP();  // Comparación en cada iteración del loop
        if (automaton->accepting_states[i] == state) {
            COUNT_COMP();  // Comparación del estado
            return 1;
        }
    }
    return 0;
}

void print_automaton_info(automaton* automaton, const char* name) {
    if (!automaton) {
        COUNT_COMP();  // Comparación null check
        printf("[AUTOMATON] %s: NULL\n", name);
        COUNT_IO(25 + strlen(name));  // IO aproximado
        return;
    }

    printf("\n[AUTOMATON] %s\n", name);
    COUNT_IO(15 + strlen(name));  // IO

    // Alphabet
    printf("  Alphabet: '%s' (size: %d)\n",
           automaton->alphabet ? automaton->alphabet : "NULL",
           automaton->alphabet_size);
    COUNT_COMP();  // Comparación ternaria
    COUNT_IO(30);  // IO aproximado

    // States
    printf("  States (%d): ", automaton->num_states);
    COUNT_IO(15);  // IO
    
    for (int i = 0; i < automaton->num_states; i++) {
        COUNT_COMP();  // Comparación en loop
        printf("%d ", automaton->states[i]);
        COUNT_IO(3);  // IO
    }
    printf("\n");
    COUNT_IO(1);  // IO

    // Initial
    printf("  Initial state: %d\n", automaton->initial_state);
    COUNT_IO(20);  // IO

    // Accepting
    printf("  Accepting states (%d): ", automaton->num_accepting_states);
    COUNT_IO(20);  // IO
    
    for (int i = 0; i < automaton->num_accepting_states; i++) {
        COUNT_COMP();  // Comparación en loop
        printf("%d ", automaton->accepting_states[i]);
        COUNT_IO(3);  // IO
    }
    printf("\n");
    COUNT_IO(1);  // IO

    // Category
    printf("  Category ID: %d\n", automaton->category);
    COUNT_IO(20);  // IO
    
    printf("  Category name: %s\n",
           automaton->category_name ? automaton->category_name : "NULL");
    COUNT_COMP();  // Comparación ternaria
    COUNT_IO(25);  // IO

    // Transition matrix
    printf("  Transition matrix:\n");
    COUNT_IO(20);  // IO
    
    for (int i = 0; i < automaton->num_states; i++) {
        COUNT_COMP();  // Comparación en loop
        printf("    ");
        COUNT_IO(4);  // IO
        
        for (int j = 0; j < automaton->alphabet_size; j++) {
            COUNT_COMP();  // Comparación en loop
            printf("%d ", automaton->transition_matrix[i][j]);
            COUNT_IO(3);  // IO
        }
        printf("\n");
        COUNT_IO(1);  // IO
    }

    printf("\n");
    COUNT_IO(1);  // IO
}



automaton* generate_automatas(FILE* file,int* num_automatas){
    fscanf(file, "%d", num_automatas);
    COUNT_IO(1);  // Lectura de 1 número entero
    // Consumir el newline después del número
    char dummy[MAX_LINE];
    fgets(dummy, MAX_LINE, file);
    COUNT_IO(strlen(dummy));  // Contar caracteres leídos de la línea

    automaton* automatas = malloc((*num_automatas) * sizeof(automaton));
    COUNT_GEN();  // Asignación de memoria es una operación general
    
    for (int k = 0; k < (*num_automatas); k++) {
        COUNT_COMP();  // Comparación en el loop
        read_automatas(file, &automatas[k]);
        automatas[k].category = k; //Category -1 is reserved to CAT_NONRECONIZED
        COUNT_GEN();  // Asignación
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
accepting states: 2
category: CAT_CATEGORY
transitions: states x symbols
        a b c
state0: 1 2 0
state1: 2 0 1
state2: 0 1 2
*/
void read_automatas(FILE* file, automaton* a) {
    char line[MAX_LINE];
    
    // Consumir línea vacía demarcador
    fgets(line, MAX_LINE, file);
    COUNT_IO(strlen(line));  // IO

    // ---------- ALPHABET ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO(strlen(line));  // IO
    
    char alphabet_str[MAX_LINE];
    sscanf(line, "%s", alphabet_str);
    COUNT_GEN();  // Parseo de string
    
    a->alphabet_size = strlen(alphabet_str);
    COUNT_GEN();  // Cálculo de tamaño
    
    a->alphabet = malloc(a->alphabet_size + 1);
    COUNT_GEN();  // Allocation
    
    strcpy(a->alphabet, alphabet_str);
    COUNT_GEN();  // Copia

    // ---------- NUM STATES ----------
    fgets(line, MAX_LINE, file);  // num_states: 3
    COUNT_IO(strlen(line));  // IO
    
    char* token = strtok(line, " \n");
    COUNT_GEN();  // Tokenización
    
    a->num_states = atoi(token);
    COUNT_GEN();  // Conversión

    //----------- STATES --------------
    a->states = malloc(a->num_states * sizeof(int));  
    COUNT_GEN();  // Allocation
    
    fgets(line, MAX_LINE, file);  // states: 0 1 2
    COUNT_IO(strlen(line));  // IO
    
    token = strtok(line, " \n");
    COUNT_GEN();  // Tokenización

    for(int i = 0; i < a->num_states; i++) {
        COUNT_COMP();  // Comparación en loop
        a->states[i] = atoi(token);
        COUNT_GEN();  // Conversión
        
        token = strtok(NULL, " \n");
        COUNT_GEN();  // Siguiente token
    }

    // ---------- INITIAL STATE ---------- 
    fgets(line, MAX_LINE, file);
    COUNT_IO(strlen(line));  // IO
    
    sscanf(line, "%d", &a->initial_state);
    COUNT_GEN();  // Parseo

    // ---------- NUMBER ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO(strlen(line));  // IO
    
    sscanf(line, "%d", &a->num_accepting_states);
    COUNT_GEN();  // Parseo
    
    // ---------- ACCEPTING STATES ----------
    fgets(line, MAX_LINE, file);  // accepting: 2
    COUNT_IO(strlen(line));  // IO
    
    token = strtok(line, " \n");
    COUNT_GEN();  // Tokenización
    
    a->accepting_states = malloc(a->num_accepting_states * sizeof(int));
    COUNT_GEN();  // Allocation

    for(int i = 0; i < a->num_accepting_states; i++) {
        COUNT_COMP();  // Comparación en loop
        a->accepting_states[i] = atoi(token);
        COUNT_GEN();  // Conversión
        
        token = strtok(NULL, " \n");
        COUNT_GEN();  // Siguiente token
    }

    // ---------- CATEGORY ----------
    fgets(line, MAX_LINE, file);
    COUNT_IO(strlen(line));  // IO
    
    char cat[MAX_LINE];
    sscanf(line, "%s", cat);
    COUNT_GEN();  // Parseo
    
    a->category_name = malloc(strlen(cat) + 1);
    COUNT_GEN();  // Allocation
    
    strcpy(a->category_name, cat);
    COUNT_GEN();  // Copia

    // ---------- TRANSITIONS ---------
    // Reservar matriz
    a->transition_matrix = malloc(a->num_states * sizeof(int*));
    COUNT_GEN();  // Allocation
    
    for (int i = 0; i < a->num_states; i++) {
        COUNT_COMP();  // Comparación en loop
        a->transition_matrix[i] = malloc(a->alphabet_size * sizeof(int));
        COUNT_GEN();  // Allocation
    }

    // Leer filas
    for (int i = 0; i < a->num_states; i++) {
        COUNT_COMP();  // Comparación en loop
        
        fgets(line, MAX_LINE, file);
        COUNT_IO(strlen(line));  // IO
        
        token = strtok(line, " \n");
        COUNT_GEN();  // Tokenización

        for (int j = 0; j < a->alphabet_size; j++) {
            COUNT_COMP();  // Comparación en loop
            
            if (token != NULL) {
                COUNT_COMP();  // Comparación null check
                a->transition_matrix[i][j] = atoi(token);
                COUNT_GEN();  // Conversión
                
                token = strtok(NULL, " \n");
                COUNT_GEN();  // Siguiente token
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