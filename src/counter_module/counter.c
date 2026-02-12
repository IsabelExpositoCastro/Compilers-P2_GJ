#include "counter.h"
#include "../preprocesor_variables.h"
#include <string.h>

// Variable static (privada) - solo accesible a través de funciones
static counters counter = {0,0,0};

// Función auxiliar para construir el nombre del archivo debug
static char* build_count_filename(const char* input_filename) {
    if (input_filename == NULL) return NULL;
    
    static char filename[256];
    strcpy(filename, input_filename);
    strcat(filename, "dbgcnt");
    return filename;
}

void add_counter_comp(){
    counter.count_comp++;
}

void add_counter_io(){
    counter.count_io++;
}

void add_counter_gen(){
    counter.count_gen++;
}

counters get_counters(){
    return counter;
}

void write_counters(FILE* output_file, const char* input_filename) {
#ifdef COUNTCONFIG
    #if COUNTOUT == 1
        // Escribir en el output_file del scanner
        if (output_file != NULL) {
            fprintf(output_file, "\n=== Contadores ===");
            fprintf(output_file, "\nContador COMP: %d\n", counter.count_comp);
            fprintf(output_file, "Contador IO: %d\n", counter.count_io);
            fprintf(output_file, "Contador GEN: %d\n", counter.count_gen);
        }
    #else
        // Escribir en archivo de debug: <filename>.<ext>dbgcnt
        char* debug_filename = build_count_filename(input_filename);
        if (debug_filename != NULL) {
            FILE *debug_file = fopen(debug_filename, "w");
            if (debug_file != NULL) {
                fprintf(debug_file, "=== Contadores ===");
                fprintf(debug_file, "\nContador COMP: %d\n", counter.count_comp);
                fprintf(debug_file, "Contador IO: %d\n", counter.count_io);
                fprintf(debug_file, "Contador GEN: %d\n", counter.count_gen);
                fclose(debug_file);
            }
        }
    #endif
#endif
}


