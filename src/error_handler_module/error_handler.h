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
    SCANNER_ERR_INVALID_CHARACTER = 1001,
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