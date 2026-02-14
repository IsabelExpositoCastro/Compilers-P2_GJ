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
    
    // ===== MACROS PARA CONTAR (VERSIÓN DETALLADA CON FUNCIÓN) =====
    // Estas macros registran automáticamente la función donde se llaman
    #define COUNT_COMP_N(amount) add_counter_comp_detailed(__func__, __LINE__, amount)
    #define COUNT_IO_N(amount) add_counter_io_detailed(__func__, __LINE__, amount)
    #define COUNT_GEN_N(amount) add_counter_gen_detailed(__func__, __LINE__, amount)
#else
    // Macros vacías en RELEASE - sin overhead
    #define COUNT_COMP_N(amount)
    #define COUNT_IO_N(amount)
    #define COUNT_GEN_N(amount)
    
    #define COUNT_COMP()
    #define COUNT_IO(chars)
    #define COUNT_GEN()
#endif