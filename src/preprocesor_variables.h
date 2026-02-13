// Definir valores para DEBUG y RELEASE
#define DEBUG 1
#define RELEASE 0

// Seleccionar formato: cambiar el valor a RELEASE cuando sea necesario
#define OUTFORMAT DEBUG

// Solo activar contadores cuando OUTFORMAT == DEBUG
#if OUTFORMAT == DEBUG
    #define COUNTCONFIG
    #define COUNTOUT 0 //1 to output file, 0 to countfile
    #define COUNTFILE "debug.dbgcnt" // Nombre del archivo de conteo si COUNTOUT==0
    
    // Macros para contar (activas solo en DEBUG)
    #define COUNT_COMP() add_counter_comp()
    #define COUNT_IO(chars) add_counter_io_n(chars)
    #define COUNT_GEN() add_counter_gen()
#else
    // Macros vacías en RELEASE - sin overhead
    #define COUNT_COMP()
    #define COUNT_IO(chars)
    #define COUNT_GEN()
#endif