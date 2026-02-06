// ------------------- LIBRARIES -------------------
#include "./input_handler_module/input_handler.h"






// ------------------- FUNCTIONS -------------------
FILE* Open_InputFile(int argc, char* argv[]) {
    // Check if an input file is provided as an argument
    if (argc > 1) {
        FILE* inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return stdin; // Return stdin to indicate failure
        }
        return inputFile; // Return the opened file pointer
    }
    return NULL; // No file provided, will use stdin
}

FILE* Open_OutputFile(int argc, char* argv[]) {
    if (argc > 1) {
        
        char* tempnam = malloc(strlen(argv[1]) + strlen("scn") + 1);
        strcpy(tempnam, argv[1]);
        strcat(tempnam, "scn");

        FILE* outputFile = fopen(tempnam, "w");
        if (outputFile == NULL) {
            printf("Error: Could not open file %s for writing\n", tempnam);
            free(tempnam);
            return stdout;;
        }
        return outputFile;
    }
    return NULL;
}