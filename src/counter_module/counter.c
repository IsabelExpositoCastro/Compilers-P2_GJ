/*
 * -----------------------------------------------------------------------------
 * counter.c
 *
 * adds operation counting functionality
 *
 * Author: Alex Martinez Penin u199133
 * -----------------------------------------------------------------------------
 */

#include "counter.h"
#include "../preprocesor_variables.h"
#include <string.h>

//global constants
#define NULL_CHAR '\0'
//Global counters
static counters counter = {0, 0, 0};

//counters for functions (partial counting)
static counters func_counters[MAX_FUNCTIONS];
static int num_functions = 0;

//variables of registering entries (functions, line, type, amount, partials and total counts)
static counter_entry entries[MAX_ENTRIES];
static int num_entries = 0;

//global local variables for storing function and line
static char current_function[MAX_FUNCTION_NAME] = "";
static int current_line = 0;

//Aux functions

/*
 * creates the filename with dbgcnt extension if COUNTOUT is 0
 *
 * @param input file name
 *
 * @return input file name with dbgcnt extension
 */
static char* build_count_filename(const char* input_filename) {
    if (input_filename == NULL) return NULL;
    
    static char filename[MAX_FILENAME];
    strcpy(filename, input_filename);
    strcat(filename, "dbgcnt");
    return filename;
}

/*
 * manages the entries for function counters, creates new entry if not exists
 *
 * @param function name
 *
 * @return pointer to the function counters if exists, NULL otherwise
 */
static counters* function_entry_manager(const char* function_name) {
    if (function_name == NULL) return NULL;
    
    //if it exists, return it
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(func_counters[i].function_name, function_name) == 0) {
            return &func_counters[i];
        }
    }
    
    //new entry creation
    if (num_functions < MAX_FUNCTIONS) {
        strncpy(func_counters[num_functions].function_name, function_name, MAX_FUNCTION_NAME - 1);
        func_counters[num_functions].function_name[MAX_FUNCTION_NAME - 1] = NULL_CHAR;
        func_counters[num_functions].count_comp = 0;
        func_counters[num_functions].count_io = 0;
        func_counters[num_functions].count_gen = 0;
        return &func_counters[num_functions++];
    }
    
    return NULL;
}

/*
 * stores the counter entry
 *
 * @param function name
 * @param input line the line number on the code
 * @param counter type (COMP, IO, GEN)
 * @param amount the amount to add to the counters
 *
 */
static void register_entry(const char* function_name, int input_line, const char* counter_type, int amount) {
    counter_entry* entry = &entries[num_entries];
    strncpy(entry->function_name, function_name, MAX_FUNCTION_NAME - 1);
    entry->function_name[MAX_FUNCTION_NAME - 1] = NULL_CHAR;
    entry->input_line = input_line;
    strncpy(entry->counter_type, counter_type, 9);
    entry->counter_type[9] = NULL_CHAR;
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

/*
 * adds the amount to the corresponding counter, updates function counters and registers the entry
 *
 * @param function name
 * @param input line the line number on the code
 * @param amount the amount to add to the counter COMP
 *
 */
void add_counter_comp_detailed(const char* function_name, int input_line, int amount) {
    counter.count_comp += amount;
    
    counters* func_entry = function_entry_manager(function_name);
    if (func_entry) {
        func_entry->count_comp += amount;
    }
    
    register_entry(function_name, input_line, "COMP", amount);
}

/*
 * adds the amount to the corresponding counter, updates function counters and registers the entry
 *
 * @param function name
 * @param input line the line number on the code
 * @param amount the amount to add to the counter IO
 *
 */
void add_counter_io_detailed(const char* function_name, int input_line, int amount) {
    counter.count_io += amount;
    
    counters* func_entry = function_entry_manager(function_name);
    if (func_entry) {
        func_entry->count_io += amount;
    }
    
    register_entry(function_name, input_line, "IO", amount);
}

/*
 * adds the amount to the corresponding counter, updates function counters and registers the entry
 *
 * @param function name
 * @param input line the line number on the code
 * @param amount the amount to add to the counter GEN
 *
 */
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

//MAIN FUNCTIONS

/*
 * prints the partial counters for each entry with the specified format
 *
 * @param function
 * @param output file
 *
 */
void print_partial_counters(char function[], FILE* out) {

        for (int i = 0; i < num_entries; i++) {
            counter_entry* event = &entries[i];
            
            //if we find another function
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

/*
 * writes the counters to the output file
 *
 * @param output file
 * @param input file name
 *
 */
void write_counters(FILE* output_file, const char* input_filename) {
#ifdef COUNTCONFIG
    FILE* out = NULL;
    int should_close = 0;
    
    #if COUNTOUT == 1
        //here the output file is the one we have as parameter, the same as on the scanner
        out = output_file;
    #else
        //here we create the new file for counters
        char* count_filename = build_count_filename(input_filename);
        if (count_filename != NULL) {
            out = fopen(count_filename, "w");
            should_close = 1;
        }
    #endif
    
    if (out != NULL) {
        
        fprintf(out, "\n");
        fprintf(out, "----------\n");
        fprintf(out, "COUNTERS\n");
        fprintf(out, "----------\n\n");
        
        char last_function[MAX_FUNCTION_NAME] = "";
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
