// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"



// ------------------ GLOBAL VARIABLES ------------------
FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)



// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    
    // Configurar output (stdout por defecto)
    ofile = stdout;
    
    // Para ahora, simplemente leer de stdin y escribir a stdout
    // TODO: Implementar Open_InputFile() después
    FILE* InputFile = stdin;
    
    fprintf(ofile, "=== SCANNER TEST - SPECIAL CHARACTERS ===\n\n");
    
    // Ejecutar el scanner
    StartScanner(InputFile, ofile);
    
    fprintf(ofile, "\n=== END OF SCAN ===\n");
    
    return 0;
}