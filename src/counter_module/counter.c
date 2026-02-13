#include "counter.h"
#include "../preprocesor_variables.h"
#include <string.h>

// Variable static (privada) - solo accesible a través de funciones
static counters counter = {0,0,0};

// Variables para rastrear contexto
static char current_function[128] = "";
static int current_line = 0;

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

void add_counter_io_n(int num_chars) {
    counter.count_io += num_chars;
}

void add_counter_gen(){
    counter.count_gen++;
}

counters get_counters(){
    return counter;
}

void set_counter_context(const char* function_name, int line_number) {
    if (function_name != NULL) {
        strncpy(current_function, function_name, sizeof(current_function) - 1);
        current_function[sizeof(current_function) - 1] = '\0';
    }
    current_line = line_number;
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
        char* count_filename = build_count_filename(input_filename);
        if (count_filename != NULL) {
            FILE *count_file = fopen(count_filename, "w");
            if (count_file != NULL) {
                fprintf(count_file, "=== Contadores ===\n");
                fprintf(count_file, "Contador COMP: %d\n", counter.count_comp);
                fprintf(count_file, "Contador IO: %d\n", counter.count_io);
                fprintf(count_file, "Contador GEN: %d\n", counter.count_gen);
                fclose(count_file);
            }
        }
    #endif
#endif
}

void write_counters_detailed(FILE* output_file, const char* input_filename) {
#ifdef COUNTCONFIG
    #if COUNTOUT == 1
        // Escribir en el output_file del scanner
        if (output_file != NULL) {
            fprintf(output_file, "\n=== CONTADORES DETALLADOS ===\n");
            fprintf(output_file, "Total COMP: %d\n", counter.count_comp);
            fprintf(output_file, "Total IO: %d\n", counter.count_io);
            fprintf(output_file, "Total GEN: %d\n", counter.count_gen);
        }
    #else
        // Escribir en archivo de debug
        char* count_filename = build_count_filename(input_filename);
        if (count_filename != NULL) {
            FILE *count_file = fopen(count_filename, "w");
            if (count_file != NULL) {
                fprintf(count_file, "=== CONTADORES DETALLADOS ===\n");
                fprintf(count_file, "Total COMP: %d\n", counter.count_comp);
                fprintf(count_file, "Total IO: %d\n", counter.count_io);
                fprintf(count_file, "Total GEN: %d\n", counter.count_gen);
                fclose(count_file);
            }
        }
    #endif
#endif
}