// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"



// ------------------ GLOBAL VARIABLES ------------------
FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)



// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    

    ofile = stdout; // Puntero default del OutputFIle  (ya estaba hecho por la bro de la Dolors)
    FILE* InputFile = Open_InputFile(argc, argv); // Abrir el InputFile de forma rápida
    StartScanner(InputFile, ofile); 
    
    // Chequear y obtener args
    // Chequear y abrir Input/Output files
    // Llamar al scanner hasta EOF
    // Cerrar ficheros y liberar memoria
    
    printCasual();

    fclose(ofile); 
    return 0;
}