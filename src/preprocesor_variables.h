#define DEBUG 1
#define RELEASE 0

//Put RELEASE to deactivate counters
#define OUTFORMAT DEBUG


#if OUTFORMAT == DEBUG
    #define COUNTCONFIG
    #define COUNTOUT 0 //1 to output file, 0 to countfile
    
    #define COUNT_COMP_N(amount) add_counter_comp_detailed(__func__, __LINE__, amount)
    #define COUNT_IO_N(amount) add_counter_io_detailed(__func__, __LINE__, amount)
    #define COUNT_GEN_N(amount) add_counter_gen_detailed(__func__, __LINE__, amount)
#else
    //Macros are empty so counters are not done
    #define COUNT_COMP_N(amount)
    #define COUNT_IO_N(amount)
    #define COUNT_GEN_N(amount)
#endif