#ifndef SPECIAL_CHARACTERS_H
#define SPECIAL_CHARACTERS_H

#include "automatonDefinition.h"

// ============ SPECIAL CHARACTERS AUTOMATON ============
// Reconoce caracteres especiales: ( ) ; { } [ ] ,
// 
// ESPECIFICACIÓN:
//   Alfabeto: "();{}[],"
//   Estados: q0 (inicial), q_accept (aceptación)
//   Transiciones: q0 --símbolo--> q_accept (para cada símbolo del alfabeto)
//   Categoría: CAT_SPECIALCHAR

// Crear e inicializar el autómata de caracteres especiales
automaton2_t* create_special_char_automaton();

#endif // SPECIAL_CHARACTERS_H
