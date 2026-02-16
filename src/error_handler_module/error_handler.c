#include "error_handler.h"
#include <stdio.h>
#include <string.h>



static int *debug_flag_ptr = NULL; 


static const Errors error_messages[] = {
    //scanner
    {
        SCANNER_ERR_INVALID_CHARACTER,
        STEP_SCANNER,
        "Invalid character '%c' found"
    },
    {
        SCANNER_ERR_INVALID_ARGUMENTS,
        STEP_SCANNER,
        "Invalid arguments provided to the program, print -help for usage information"
    },
    {
        SCANNER_ERR_FOUND_NON_RECOGNIZABLE,
        STEP_SCANNER,
        "Found non-recognizable token: '%s'"
    }, 
    {
        SCANNER_ERR_INVALID_FILE_FORMAT,
        STEP_SCANNER,
        "Invalid file format while reading automaton definition: expected alphabet definition but found missing or malformed data."
    },
    {
        SCANNER_ERR_INVALID_ALPHABET,
        STEP_SCANNER,
        "Invalid alphabet definition: alphabet should contain only valid characters, but found '%s'."
    },
    {
        SCANNER_ERR_INVALID_NUM_STATES,
        STEP_SCANNER,
        "Invalid number of states: %d"
    },
    {
        SCANNER_ERR_INVALID_STATE,
        STEP_SCANNER,
        "Invalid state definition: expected %d states but found %d"
    },
    {
        SCANNER_ERR_INVALID_INITIAL_STATE,
        STEP_SCANNER,
        "Invalid initial state: expected a state between 0 and %d but found %d"
    },
    {
        SCANNER_ERR_INVALID_ACCEPTING_STATE,
        STEP_SCANNER,
        "Invalid accepting state definition: expected %d accepting states but found %d"
    },
    {
        SCANNER_ERR_INVALID_CATEGORY_NAME,
        STEP_SCANNER,
        "Invalid category name: '%s'"
    },
    {
        SCANNER_ERR_MEMORY_ALLOCATION,
        STEP_SCANNER,
        "Memory allocation failed while processing automata definitions"
    }
};

//funciones auxiliares

/**
 * Get step name as string
 */
static const char* step_name(ErrorStep step) {
    switch (step) {
        case STEP_SCANNER: return "SCANNER";
        case STEP_PARSER: return "PARSER";
        default: return "UNKNOWN";
    }
}

/**
 * Find error definition by ID
 * @return: pointer to error definition or NULL if not found
 */
static const Errors* find_error(ErrorID error_id) {
    int error_message_count = sizeof(error_messages) / sizeof(error_messages[0]);
    for (int i = 0; i < error_message_count; i++) {
        if (error_messages[i].error_id == error_id) {
            return &error_messages[i];
        }
    }
    return NULL; 
}

//funciones principales

void error_handler_init(int *debug_flag) {
    debug_flag_ptr = debug_flag;
}

const char* error_get_message(ErrorID error_id) {
    const Errors *err_def = find_error(error_id);
    return err_def ? err_def->message : "Unknown error";
}

void error_report(ErrorID error_id, ErrorStep step, const char *file, int line, ...) {
    // Check if debug flag is enabled
    if (debug_flag_ptr && *debug_flag_ptr) {
        // Find the error definition based on the error ID
        const Errors *err_def = find_error(error_id);
        if (err_def) {
            // Initialize variadic arguments, we do not know how many parameters or their types, 
            // so we rely on the error message template to format them correctly
            va_list args;
            va_start(args, line);

            // Print the error to stderr (or log file if needed)
            fprintf(stdout, "[%s] Error in file %s at line %d: ", step_name(step), file, line);
            vfprintf(stdout, err_def->message, args); //Print message depending on the template and provided parameters
            fprintf(stdout, "\n");

            // Clean up variadic arguments
            va_end(args);
        }
    }
}