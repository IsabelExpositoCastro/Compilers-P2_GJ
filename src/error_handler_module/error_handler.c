#include "error_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "preprocesor_variables.h"





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
        "Invalid arguments provided to the program, write the program input in the terminal or provide a file as argument"
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
    (void)debug_flag;
}

const char* error_get_message(ErrorID error_id) {
    const Errors *err_def = find_error(error_id);
    return err_def ? err_def->message : "Unknown error";
}

void error_report(ErrorID error_id, ErrorStep step, const char *file, int line, FILE *out, ...) {
    const Errors *err_def = find_error(error_id);
    if (!err_def) return;

#if DEBUG == 0
    FILE *target = stdout;
#else
    FILE *target = out ? out : stdout;
    if(target == NULL) {
        target = stdout; 
    }
#endif

    va_list args;
    va_start(args, out);

    fprintf(target, "[%s] Error in file %s at line %d: ", step_name(step), file, line);
    vfprintf(target, err_def->message, args);
    fprintf(target, "\n");

    va_end(args);
}