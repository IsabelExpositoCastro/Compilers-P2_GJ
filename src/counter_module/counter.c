#include "counter.h"
#include "../preprocesor_variables.h"
#include <string.h>

// ============ VARIABLES GLOBALES ============

// Variable static (privada) - solo accesible a través de funciones
static counters counter = {0, 0, 0};

// Array para acumular contadores por función (máximo 50 funciones)
static counters func_counters[50];
static int num_functions = 0;

// Array para registrar eventos detallados (máximo 10000 eventos)
static counter_entry entries[10000];
static int num_entries = 0;

// Variables para rastrear contexto actual
static char current_function[128] = "";
static int current_line = 0;

// ============ FUNCIONES AUXILIARES ============

// Función auxiliar para construir el nombre del archivo debug
static char* build_count_filename(const char* input_filename) {
    if (input_filename == NULL) return NULL;
    
    static char filename[256];
    strcpy(filename, input_filename);
    strcat(filename, "dbgcnt");
    return filename;
}

// Buscar o crear entrada de función en el array
static counters* function_entry_manager(const char* function_name) {
    if (function_name == NULL) return NULL;
    
    // Buscar si ya existe
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(func_counters[i].function_name, function_name) == 0) {
            return &func_counters[i];
        }
    }
    
    // Crear nueva entrada si hay espacio
    if (num_functions < 50) {
        strncpy(func_counters[num_functions].function_name, function_name, 127);
        func_counters[num_functions].function_name[127] = '\0';
        func_counters[num_functions].count_comp = 0;
        func_counters[num_functions].count_io = 0;
        func_counters[num_functions].count_gen = 0;
        return &func_counters[num_functions++];
    }
    
    return NULL;
}

// Registrar evento detallado
static void register_entry(const char* function_name, int input_line, const char* counter_type, int amount) {
    counter_entry* entry = &entries[num_entries];
    strncpy(entry->function_name, function_name, 127);
    entry->function_name[127] = '\0';
    entry->input_line = input_line;
    strncpy(entry->counter_type, counter_type, 9);
    entry->counter_type[9] = '\0';
    entry->amount = amount;
    
    //partial counting
    counters* func = function_entry_manager(function_name);
    if (func) {
        entry->partial_comp = func->count_comp;
        entry->partial_io = func->count_io;
        entry->partial_gen = func->count_gen;
    }
    
    //global counting
    entry->total_comp = counter.count_comp;
    entry->total_io = counter.count_io;
    entry->total_gen = counter.count_gen;
    
    num_entries++;
}

void add_counter_comp_detailed(const char* function_name, int input_line, int amount) {
    counter.count_comp += amount;
    
    counters* func_entry = function_entry_manager(function_name);
    if (func_entry) {
        func_entry->count_comp += amount;
    }
    
    register_entry(function_name, input_line, "COMP", amount);
}

void add_counter_io_detailed(const char* function_name, int input_line, int amount) {
    counter.count_io += amount;
    
    counters* func_entry = function_entry_manager(function_name);
    if (func_entry) {
        func_entry->count_io += amount;
    }
    
    register_entry(function_name, input_line, "IO", amount);
}

void add_counter_gen_detailed(const char* function_name, int input_line, int amount) {
    counter.count_gen += amount;
    
    counters* func_entry = function_entry_manager(function_name);
    if (func_entry) {
        func_entry->count_gen += amount;
    }
    
    register_entry(function_name, input_line, "GEN", amount);
}

counters get_counters() {
    return counter;
}

/*
void set_counter_context(const char* function_name, int input_line_number) {
    if (function_name != NULL) {
        strncpy(current_function, function_name, sizeof(current_function) - 1);
        current_function[sizeof(current_function) - 1] = '\0';
    }
    current_line = input_line_number;
}*/

// ============ FUNCIONES DE ESCRITURA ============
void print_partial_counters(char function[], FILE* out) {

        for (int i = 0; i < num_entries; i++) {
            counter_entry* event = &entries[i];
            
            // Marcar cambio de función
            if (strcmp(event->function_name,function) != 0) {
                if (i > 0) {
                    fprintf(out, "\n");
                }
                fprintf(out, "FUNCTION: %s adding in counters\n", event->function_name);
                strcpy(function, event->function_name);
            }
            fprintf(out, " (LINE: %d) +%d to counter %s | ", event->input_line, event->amount, event->counter_type);
            
            //partial counting
            if (strcmp(event->counter_type, "COMP") == 0) {
                fprintf(out, "PARTIAL COMP: COMP=%d |", event->partial_comp);
            } else if (strcmp(event->counter_type, "IO") == 0) {
                fprintf(out, "PARTIAL IO: IO=%d |", event->partial_io);
            } else {
                fprintf(out, "PARTIAL GEN: GEN=%d |", event->partial_gen);
            }
            
            //Total on that line
            fprintf(out, "TOTAL COUNT: (COMP=%d, IO=%d, GEN=%d)\n", 
                    event->total_comp, event->total_io, event->total_gen);
        }
        
}
void write_counters(FILE* output_file, const char* input_filename) {
#ifdef COUNTCONFIG
    FILE* out = NULL;
    int should_close = 0;
    
    #if COUNTOUT == 1
        // Escribir en el output_file del scanner
        out = output_file;
    #else
        // Escribir en archivo de debug
        char* count_filename = build_count_filename(input_filename);
        if (count_filename != NULL) {
            out = fopen(count_filename, "w");
            should_close = 1;
        }
    #endif
    
    if (out != NULL) {
        // Encabezado principal
        fprintf(out, "\n");
        fprintf(out, "----------\n");
        fprintf(out, "COUNTERS\n");
        fprintf(out, "----------\n\n");
        
        char last_function[128] = "";
        print_partial_counters(last_function, out);
        fprintf(out, "\n");
        
        //Global counters (shown at the end of the file)
        fprintf(out, "FINAL COUNT VALUES\n");
        fprintf(out, "__________________\n");
        fprintf(out, "Total COMP: %d\n", counter.count_comp);
        fprintf(out, "Total IO: %d\n", counter.count_io);
        fprintf(out, "Total GEN: %d\n", counter.count_gen);
        fprintf(out, "\n");
        
        if (should_close) {
            fclose(out);
        }
    }
#endif
}
