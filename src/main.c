// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"
#include "input_handler_module/input_handler.h"
#define AUTOMATA_FILE "automatas.txt"


// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    //Llamada a argumentos, sera copiar pegar de la anteriror.
    
    //Pendiente de error message.
    FILE* autfile = fopen("automatas.txt", "r");

    FILE* ifile = Open_InputFile(argc, argv);

    FILE* ofile = Open_OutputFile(argc, argv);
    
    char* input_filename = get_input_filename(argc, argv);
    StartScanner(ifile, ofile, autfile,input_filename);

    if (ifile != stdin) fclose(ifile);
    if (ofile != stdout) fclose(ofile);
    if (autfile) fclose(ofile);

    return 0;
}