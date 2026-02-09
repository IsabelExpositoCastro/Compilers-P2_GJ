// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"
#include "input_handler_module/input_handler.h"



// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {

    //Input / Output
    FILE* ofile = set_output_t_file(argv[argc - 1]);
    FILE* ifile = Open_InputFile(argc, argv);

    // Ejecutar el scanner
    StartScanner(ifile, ofile);
    
    if (ifile != stdin && ifile != NULL) {
        fclose(ifile);}
    if (ofile != NULL && ofile != stdout) {
        fclose(ofile);}

    return 0;
}