// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "scanner_module/scanner.h"
#include "input_handler_module/input_handler.h"



// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    FILE* ifile = Open_InputFile(argc, argv);
    if (!ifile) return 1;

    FILE* ofile = Open_OutputFile(argc, argv);
    if (!ofile) {
        if (ifile != stdin) fclose(ifile);
        return 1;
    }

    StartScanner(ifile, ofile);

    if (ifile != stdin) fclose(ifile);
    if (ofile != stdout) fclose(ofile);

    return 0;
}