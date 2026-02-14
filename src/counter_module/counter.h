#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../automatas_module/automatonDefinition.h"

// Initialize counters. `max_categories` should be the number of automata/categories
// available. An extra slot is reserved for non-recognized tokens.
void counters_init(int max_categories, FILE* out);

// Increment counter for given category (use CAT_NONRECOGNIZED for unknowns)
void counters_inc(int category);

// Print counters summary to `out` (if NULL prints to stdout)
void counters_print(FILE* out);

// Free internal data structures
void counters_free();

#endif // COUNTER_H