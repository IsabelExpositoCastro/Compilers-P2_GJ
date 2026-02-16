#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdarg.h>

/* ===== STEP IDENTIFIERS ===== */
typedef enum { 
    STEP_SCANNER = 1, 
    STEP_PARSER = 2 
} ErrorStep;

/* ===== ERROR IDENTIFIERS (organized by phase) ===== */
typedef enum {
    //scanner errors
    SCANNER_ERR_INVALID_CHARACTER = 1001, //Not sure
    SCANNER_ERR_FOUND_NON_RECOGNIZABLE = 1003,
    SCANNER_ERR_INVALID_ARGUMENTS = 1002,
    //scanner errors related to automata definition reading
    SCANNER_ERR_INVALID_FILE_FORMAT = 1004,
    SCANNER_ERR_INVALID_ALPHABET = 1005,
    SCANNER_ERR_INVALID_NUM_STATES = 1006,
    SCANNER_ERR_INVALID_STATE = 1007,
    SCANNER_ERR_INVALID_INITIAL_STATE = 1008,
    SCANNER_ERR_INVALID_ACCEPTING_STATE = 1009,
    SCANNER_ERR_INVALID_CATEGORY_NAME = 1010,
    SCANNER_ERR_MEMORY_ALLOCATION = 1011,
    SCANNER_ERR_INVALID_NULL_STATE = 1012
    //parser errors (to be defined later)

} ErrorID;

/* ===== ERROR STRUCTURE ===== */
typedef struct {
    ErrorID error_id;
    ErrorStep step;
    const char *message;  /* Message template with %s, %d, etc. */
} Errors;

/* ===== INITIALIZATION ===== */
/**
 * Initialize error handler with reference to DEBUG flag
 * @param debug_flag: pointer to external DEBUG flag (from main.c or config)
 */
void error_handler_init(int *debug_flag);

/* ===== ERROR REPORTING ===== */
/**
 * Report an error with parameters (variadic)
 * @param error_id: unique error identifier
 * @param step: compiler phase identifier
 * @param file: source file name where error occurred
 * @param line: line number where error occurred
 * @param ...: parameters to format the error message (printf-style)
 * 
 * Only prints if DEBUG flag is 1
 */
void error_report(ErrorID error_id, ErrorStep step, const char *file, int line, ...);

/**
 * Get error message template by ID
 * @param error_id: the error identifier
 * @return: message template string or NULL if not found
 */
const char* error_get_message(ErrorID error_id);

#endif