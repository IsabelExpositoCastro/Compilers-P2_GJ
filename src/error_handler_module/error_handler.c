#include "error_handler.h"
#include <stdio.h>
#include <string.h>

//declaraciones
static int *debug_flag_ptr = NULL; //ponerle lo que sea el debug flag


static const Errors error_messages[] = {
    //scanner
    {
        SCANNER_ERR_INVALID_CHARACTER,
        STEP_SCANNER,
        "Invalid character '%c' found"
    },
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
   //primero mirar si esta la debug flag activada

   //luego imprimir errores en consola o file (printf o fprintf)
}
