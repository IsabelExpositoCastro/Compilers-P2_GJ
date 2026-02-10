/*
 * -----------------------------------------------------------------------------
 * input_handler.c
 *
 * Opens input file from CLI arguments.
 * - If argv[1] exists: tries to open it in read mode.
 * - If it fails: returns NULL (caller decides what to do).
 * - If no file argument: returns stdin.
 * -----------------------------------------------------------------------------
 */

#include "./input_handler_module/input_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ------------------- FUNCTIONS --------------------
FILE* Open_InputFile(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* inputFile = fopen(argv[1], "r");
        if (!inputFile) {
            fprintf(stderr, "[INPUT_HANDLER] Error: Could not open file %s\n", argv[1]);
            return NULL;
        }
        return inputFile;
    }
    return stdin;
}


//Create the output file
static char* build_output_name(const char* input_name) {
    // output = input_name + "scn"
    size_t len = strlen(input_name);
    char* out = (char*)malloc(len + 4); // "scn" + '\0'
    if (!out) return NULL;

    strcpy(out, input_name);
    strcat(out, "scn");
    return out;
}



// Opens the output file based on the provided filename, defaults to stdout if "stdout" is specified


FILE* Open_OutputFile(int argc, char* argv[]) {
    // If reading from stdin, output should be stdout (no filename available)
    if (argc <= 1) return stdout;

    char* outname = build_output_name(argv[1]);
    if (!outname) {
        fprintf(stderr, "[INPUT_HANDLER] Error: malloc failed creating output name\n");
        return NULL;
    }

    FILE* outputFile = fopen(outname, "w");
    if (!outputFile) {
        fprintf(stderr, "[INPUT_HANDLER] Error: Could not open output file %s\n", outname);
        free(outname);
        return NULL;
    }

    free(outname);
    return outputFile;
}

