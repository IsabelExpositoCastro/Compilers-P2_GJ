// ------------------- LIBRARIES -------------------
#include "./input_handler_module/input_handler.h"






// ------------------- FUNCTIONS -------------------
FILE* Open_InputFile(int argc, char* argv[]) {
    // Check if an input file is provided as an argument
    if (argc > 1) {
        FILE* inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return NULL; // Return NULL to indicate failure
        }
        return inputFile; // Return the opened file pointer
    }
    return NULL; // No file provided, will use stdin
}