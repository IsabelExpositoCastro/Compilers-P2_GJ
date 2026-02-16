/*
 * -----------------------------------------------------------------------------
 * module_args.h 
 *
 * Header file for module_args, which provides functions to handle command-line
 * arguments in a modular and testable way.
 *
 * Functions:
 * - `print_arguments`: Logs each argument received by the program.
 * - `get_args`: Processes command-line arguments and sets output mode and input file name.
 * - `print_manHelp`: Prints a help message with usage instructions.
 *
 * Usage:
 *     Include this header in main modules or test modules that require access
 *     to command-line processing functionality.
 *
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently. 
 *
 * Team: GJ
 * Contributor/s: Margarita Crespo
 * -----------------------------------------------------------------------------
 */

#include "../main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../error_handler_module/error_handler.h"

#define MIN_ARGS 2  // Minimum arguments (1 flag and 1 input file)
#define DEBUG_MODE 1
#define RELEASE_MODE 0
#define DEBUG_FLAG_LOWER "-d"
#define DEBUG_FLAG_UPPER "-D"
#define RELEASE_FLAG_LOWER "-r"
#define RELEASE_FLAG_UPPER "-R"
#define HELP_FLAG "-help"

void print_arguments(int argc, char *argv[]);
void get_args(int argc, char *argv[], int *output_mode, char **input_file_name);
void print_manHelp();

