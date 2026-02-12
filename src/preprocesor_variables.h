#define OUTFORMAT RELEASE // RELEASE or DEBUG
#define COUNTCONFIG // Eliminate if don't want the counters.
#ifdef COUNTCONFIG
    #define COUNTOUT 1 //1 to output file, 0 to countfile
#endif