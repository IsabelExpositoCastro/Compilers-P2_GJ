#include "counter.h"
// Simple counters implementation: counts tokens per category and non-recognized

static long *counts = NULL; // dynamic array of counts
static int slots = 0;       // number of category slots (max_categories)
static FILE* out_file = NULL;

// Initialize counters. `max_categories` should be the number of automata/categories
// available. An extra slot is reserved for non-recognized tokens.
void counters_init(int max_categories, FILE* out) {
	if (max_categories < 1) max_categories = 1;
	// allocate one extra slot for non-recognized tokens
	slots = max_categories + 1;
	counts = (long*)calloc(slots, sizeof(long));
	out_file = out ? out : stdout;
}


// Map category to index in counts array. Recognized categories are 0..slots-2, non-recognized is slots-1.
static int idx_for_category(int category) {
	if (!counts || slots <= 0) return -1;
	if (category >= 0 && category < slots - 1) return category;
	// map non-recognized (negative) or out-of-range to last slot
	return slots - 1;
}


// Increment counter for given category (use CAT_NONRECOGNIZED for unknowns)
void counters_inc(int category) {
	int idx = idx_for_category(category);
	if (idx < 0) return;
	counts[idx]++;
}


// Print counters summary to `out` (if NULL prints to stdout)
void counters_print(FILE* out) {
	FILE* f = out ? out : out_file ? out_file : stdout;
	if (!counts) {
		fprintf(f, "[COUNTERS] not initialized\n");
		return;
	}

	long total = 0;
	for (int i = 0; i < slots - 1; i++) total += counts[i];
	long nonrec = counts[slots - 1];
	fprintf(f, "\n[COUNTERS] Summary:\n");
	for (int i = 0; i < slots - 1; i++) {
		fprintf(f, "  CAT_%d : %ld\n", i, counts[i]);
	}
	fprintf(f, "  CAT_NONRECOGNIZED : %ld\n", nonrec);
	fprintf(f, "  TOTAL_RECOGNIZED : %ld\n", total);
	fprintf(f, "  TOTAL           : %ld\n\n", total + nonrec);
	fflush(f);
}

// Free internal data structures
void counters_free() {
	if (counts) free(counts);
	counts = NULL;
	slots = 0;
	out_file = NULL;
}

