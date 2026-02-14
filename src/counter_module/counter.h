// Here we will define the 4 counter for P2 project.
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char function_name[128];
    int count_comp; 
    int count_io;
    int count_gen;
} counters;

typedef struct {
    char function_name[128];
    int input_line;
    char counter_type[10];  // "COMP", "IO", "GEN"
    int amount;
    int partial_comp;
    int partial_io;
    int partial_gen;
    int total_comp;
    int total_io;
    int total_gen;
} counter_entry;



// Funciones para incrementar contadores con tracking detallado
void add_counter_comp_detailed(const char* function_name, int input_line, int amount);
void add_counter_io_detailed(const char* function_name, int input_line, int amount);
void add_counter_gen_detailed(const char* function_name, int input_line, int amount);



// Función para obtener los valores de los contadores
counters get_counters();

// Función para rastrear contexto actual (línea de entrada procesada)
//void set_counter_context(const char* function_name, int input_line_number);

// Función auxiliar para construir el nombre del archivo debug
static char* build_count_filename(const char* input_filename); 

static void register_entry(const char* function_name, int input_line, const char* counter_type, int amount);
 
// Función para escribir los contadores
// output_file: archivo del scanner (usado si COUNTOUT==1)
// input_filename: nombre del archivo de entrada (para generar <filename>.<ext>dbgcnt si COUNTOUT==0)
void write_counters(FILE* output_file, const char* input_filename);
