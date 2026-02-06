// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"
#include "input_handler_module/input_handler.h"



// ------------------ GLOBAL VARIABLES ------------------
FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)



// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    
    // Configurar output (stdout por defecto)
    ofile = stdout;
    
    // Abrir archivo de entrada
    FILE* InputFile = Open_InputFile(argc, argv);
    if (InputFile == NULL) {
        InputFile = stdin;  // Si no se proporciona archivo, usar stdin
    }
    
    fprintf(ofile, "=== SCANNER TEST - SPECIAL CHARACTERS ===\n\n");
    
    // Ejecutar el scanner
    StartScanner(InputFile, ofile);
    
    fprintf(ofile, "\n=== END OF SCAN ===\n");
    
    // Cerrar archivo si se abrió
    if (InputFile != stdin && InputFile != NULL) {
        fclose(InputFile);
    }
    
    return 0;
}