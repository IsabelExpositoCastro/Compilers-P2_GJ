/*
 * -----------------------------------------------------------------------------
 * counter.h 
 *
 * Header file for counters, which provides functions to count operations
 * Functions:
 * - `add_counter_comp_detailed`: Adds a specified amount to the COMP counter
 * - `add_counter_io_detailed`: Adds a specified amount to the IO counter
 * - `add_counter_gen_detailed`: Adds a specified amount to the GEN counter
 * - `get_counters`: Retrieves the current values of all counters
 * - `build_count_filename`: Builds a filename for the counter output based on the input filename
 * - `register_entry`: Registers a counter entry with details about the function, line, type, and amount
 * - `write_counters`: Writes the counter values to an output file, with details
 * 
 * Usage:
 *     Use functions as defined in preprocesor_variables.h after each operation
 * 
 * author : Alex Martinez Penin u199133
 * -----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_FUNCTIONS 50
#define MAX_ENTRIES 1000
#define MAX_COUNTER_TYPE 10
#define MAX_FILENAME 256
#define MAX_FUNCTION_NAME 128
typedef struct {
    char function_name[MAX_FUNCTION_NAME];
    int count_comp;   //final counter for COMP operations
    int count_io;     //final counter for IO operations
    int count_gen;    //final counter for GEN operations
} counters;

typedef struct {
    char function_name[MAX_FUNCTION_NAME];
    int input_line;
    char counter_type[MAX_COUNTER_TYPE];  // "COMP", "IO", "GEN"
    int amount;                           //amount to add to the counter
    int partial_comp;                     //partial count for COMP at the moment of the entry
    int partial_io;                       //partial count for IO at the moment of the entry
    int partial_gen;                      //partial count for GEN at the moment of the entry
    int total_comp;                       //total count for COMP at the moment of the entry
    int total_io;                         //total count for IO at the moment of the entry
    int total_gen;                        //total count for GEN at the moment of the entry
} counter_entry;



void add_counter_comp_detailed(const char* function_name, int input_line, int amount);
void add_counter_io_detailed(const char* function_name, int input_line, int amount);
void add_counter_gen_detailed(const char* function_name, int input_line, int amount);




counters get_counters();


//void set_counter_context(const char* function_name, int input_line_number);


static char* build_count_filename(const char* input_filename); 

static void register_entry(const char* function_name, int input_line, const char* counter_type, int amount);
 
void write_counters(FILE* output_file, const char* input_filename);