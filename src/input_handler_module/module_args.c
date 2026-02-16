/*
 * -----------------------------------------------------------------------------
 * module_args.c 
 *
 * This module provides functionality to process command-line arguments.
 * 
 * - `print_arguments`: Prints each command-line argument with its index.
 * - `get_args`: Processes command-line arguments and sets output mode and input file name.
 * - `print_manHelp`: Prints a help message with usage instructions.
 *
 * Usage:
 *     Called from the main application or test modules to process arguments.
 *
 * Status:
 *     Implemented and ready for integration with the main application.
 *
 * Team: GJ
 * Author: Margarita Crespo 
 * -----------------------------------------------------------------------------
 */

#include "module_args.h"
#include "../error_handler_module/error_handler.h"
// Function to print the arguments (optional for debugging)
void print_arguments(int argc, char *argv[]) {
    printf("Arguments received (%d):\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
}

// Function to handle command-line arguments
void get_args(int argc, char *argv[], int *output_mode, char **input_file_name) {
    *output_mode = RELEASE_MODE;  // Default to release mode
    *input_file_name = NULL;      // Default to no file name

    // Ensure there are at least 2 arguments (program name + 1 flag + 1 file)
    if (argc < MIN_ARGS) {
        error_report(SCANNER_ERR_INVALID_ARGUMENTS, STEP_SCANNER, __FILE__, __LINE__);
        return;
    }

    // Parse flags and the input file
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], DEBUG_FLAG_LOWER) == 0 || strcmp(argv[i], DEBUG_FLAG_UPPER) == 0) {
            *output_mode = DEBUG_MODE;  // Set to debug mode
        } else if (strcmp(argv[i], RELEASE_FLAG_LOWER) == 0 || strcmp(argv[i], RELEASE_FLAG_UPPER) == 0) {
            *output_mode = RELEASE_MODE;  // Set to release mode
        } else if (strcmp(argv[i], HELP_FLAG) == 0) {
            print_manHelp();  // Display the help message
            return;  // Exit after printing help
        } else if (argv[i][0] != '-') {
            // The last non-flag argument should be the input file
            *input_file_name = argv[i];
        } else {
            // If any invalid flags are found
            error_report(SCANNER_ERR_INVALID_ARGUMENTS, STEP_SCANNER, __FILE__, __LINE__);
            return;
        }
    }

    // If no input file was specified
    if (*input_file_name == NULL) {
        error_report(SCANNER_ERR_INVALID_FILE_FORMAT, STEP_SCANNER, __FILE__, __LINE__, "Missing input file", "No input file provided");
    }
}

// Function to print the help message
void print_manHelp(void) {
    printf("Usage: ./program [options] <file_path>\n");
    printf("Options (only for errors):\n");
    printf("-d        Enable debug mode\n");
    printf("-r        Enable release mode\n");
    printf("-help     Display this help message\n");
}

