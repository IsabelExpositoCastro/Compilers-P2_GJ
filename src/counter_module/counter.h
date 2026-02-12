#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int count_comp; 
    int count_io;
    int count_gen;
} counters;

// Funciones para incrementar contadores
void add_counter_comp();

void add_counter_io();

void add_counter_gen();

// Función para obtener los valores de los contadores
counters get_counters();

static char* build_count_filename(const char* input_filename); // Función para escribir los contadores
 
// Función para escribir los contadores
// output_file: archivo del scanner (usado si COUNTOUT==1)
// input_filename: nombre del archivo de entrada (para generar <filename>.<ext>dbgcnt si COUNTOUT==0)
void write_counters(FILE* output_file, const char* input_filename);
