#ifndef LITERALS_H
#define LITERALS_H

#include "automatonDefinition.h"  // For automaton types and functions

// ----------------- LITERAL AUTOMATON -----------------
// Alphabet for literal characters 
extern const char* LITERAL_ALPHABET;

// Function to create the literal automaton (DFA)
automaton2_t* create_literal_automaton();


#endif // LITERALS_H
