// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"
#include "input_handler_module/input_handler.h"
#include "counter_module/counter.h"
#define AUTOMATA_FILE "../automatas_module/automatas.txt"


// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    //Llamada a argumentos, sera copiar pegar de la anteriror.
    
    //Pendiente de error message.
    FILE* autfile = fopen(AUTOMATA_FILE, "r");
    FILE* ifile = Open_InputFile(argc, argv);
    FILE* ofile = Open_OutputFile(argc, argv);

    StartScanner(ifile, ofile, autfile);

    if (ifile != stdin) fclose(ifile);
    if (ofile != stdout) fclose(ofile);

    return 0;
}