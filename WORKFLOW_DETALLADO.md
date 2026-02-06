# WORKFLOW DETALLADO DEL SCANNER - STEP BY STEP

## INPUT EJEMPLO
Archivo: `test_special_chars.txt` contiene:
```
( ) ; { }
```

---

## PASO 1: EJECUCIÓN DE main() - main.c
### Firma: `int main(int argc, char *argv[])`

**Parámetros al ejecutar:**
```
argv[0] = "lexer.exe"
argv[1] = "./test_special_chars.txt"
argc = 2
```

**Variables Locales Creadas:**
```c
FILE* ofile = NULL;  // Global, asignado a stdout
```

### PASO 1.1: Asignar ofile
```c
ofile = stdout;
```
- **ofile (GLOBAL):** `FILE*` apuntando a stdout

### PASO 1.2: Llamar `Open_InputFile(argc, argv)` - input_handler.c
**Parámetros:**
- `argc = 2`
- `argv[]` = ["lexer.exe", "./test_special_chars.txt"]

**Ejecución:**
```c
if (argc > 1) {                    // ✓ true (argc = 2)
    FILE* inputFile = fopen(argv[1], "r");  
    // argv[1] = "./test_special_chars.txt"
    // inputFile = FILE* apertura válida
    
    if (inputFile == NULL) {       // ✗ false
        return NULL;
    }
    return inputFile;              // ✓ Retorna FILE* válido
}
```

**Retorna:** FILE* válido apuntando a `test_special_chars.txt`

### PASO 1.3: Asignar a variable local
```c
FILE* InputFile = Open_InputFile(argc, argv);
// InputFile = FILE* (válido)

if (InputFile == NULL) {           // ✗ false
    InputFile = stdin;
}
```

**Resultado:**
- **InputFile (LOCAL):** FILE* válido

### PASO 1.4: Imprimir mensaje
```c
fprintf(ofile, "=== SCANNER TEST - SPECIAL CHARACTERS ===\n\n");
// ofile = stdout
// OUTPUT en consola: "=== SCANNER TEST - SPECIAL CHARACTERS ===\n\n"
```

### PASO 1.5: Llamar `StartScanner(InputFile, ofile)` - scanner.c

---

## PASO 2: STARTSCANNER() - scanner.c
### Firma: `void StartScanner(FILE* InputFile, FILE* OutputFile)`

**Parámetros:**
```c
InputFile = FILE* (test_special_chars.txt)
OutputFile = FILE* (stdout)
```

### PASO 2.1: Validar parámetros
```c
if (!InputFile || !OutputFile) {   // ✗ false
    fprintf(stderr, "[ERROR] Scanner: Invalid input or output file\n");
    return;
}
```
✓ Pasa validación

### PASO 2.2: Inicializar autómatas - `initialize_all_automata()`

#### PASO 2.2.1: Llamar `create_special_char_automaton()` - special_characters.c

##### Paso 2.2.1a: Crear autómata vacío
```c
automaton2_t* automaton = create_empty_automaton();
```
**Retorna:** `automaton2_t*` con memoria asignada (todos campos = 0/NULL)

##### Paso 2.2.1b: Asignar alfabeto
```c
set_alphabet(automaton, SPECIAL_CHARS);
// SPECIAL_CHARS = "();{}[],"

// Resultado en automaton:
automaton->alphabet = "();{}[],"
automaton->alphabet_size = 8
```

##### Paso 2.2.1c: Asignar número de estados
```c
set_num_states(automaton, 2, 1);
// num_states = 2 (q0, q_accept)
// num_accepting = 1 (solo q_accept)

// Resultado en automaton:
automaton->num_states = 2
automaton->num_accepting_states = 1
// Asigna memoria para transition_matrix[2][8]
```

##### Paso 2.2.1d: Asignar estado inicial
```c
set_initial_state(automaton, STATE_INITIAL);
// STATE_INITIAL = 0

// Resultado:
automaton->initial_state = 0
```

##### Paso 2.2.1e: Asignar estados de aceptación
```c
int accepting_states[] = {STATE_ACCEPT};  // {1}
set_accepting_states(automaton, accepting_states, 1);

// Resultado:
automaton->accepting_states[0] = 1
automaton->num_accepting_states = 1
```

##### Paso 2.2.1f: Llenar matriz de transición
```c
for (int i = 0; i < SPECIAL_CHARS_COUNT; i++) {  // i = 0..7
    automaton->transition_matrix[STATE_INITIAL][i] = STATE_ACCEPT;
    // transition_matrix[0][0] = 1  (para '(')
    // transition_matrix[0][1] = 1  (para ')')
    // transition_matrix[0][2] = 1  (para ';')
    // ... hasta i=7
    
    automaton->transition_matrix[STATE_ACCEPT][i] = -1;
    // transition_matrix[1][0] = -1
    // transition_matrix[1][1] = -1
    // ... hasta i=7
}
```

**Estructura resultante:**
```c
automaton2_t {
    alphabet = "();{}[],"              // 8 caracteres
    alphabet_size = 8
    states = [0, 1]
    num_states = 2
    initial_state = 0
    accepting_states = [1]
    num_accepting_states = 1
    transition_matrix = [
        [1, 1, 1, 1, 1, 1, 1, 1],     // q0  → todos los símbolos van a q_accept
        [-1,-1,-1,-1,-1,-1,-1,-1]      // q_accept → error (no hay transiciones)
    ]
    category = CAT_SPECIALCHAR         // = 5
}
```

##### Paso 2.2.1g: Validar y retornar
```c
validate_automaton(automaton);  // ✓ válido
return automaton;               // Retorna automaton2_t*
```

#### PASO 2.2.2: Guardar en variable global
```c
// En initialize_all_automata():
all_automata[0] = create_special_char_automaton();
// all_automata[0] = automaton2_t*

num_automata = 1;

printf("[SCANNER] Initialized %d automata\n", num_automata);
// OUTPUT: "[SCANNER] Initialized 1 automata"
```

**Variables globales (STATIC):**
```c
static automaton2_t* all_automata[MAX_AUTOMATA];  
// all_automata[0] = automaton2_t* (special chars)
// all_automata[1..9] = NULL

static int num_automata = 1;
```

### PASO 2.3: Crear contexto del scanner
```c
scanner_context_t ctx = {
    .input_file = InputFile,       // FILE* (test_special_chars.txt)
    .output_file = OutputFile,     // FILE* (stdout)
    .current_char = -1,
    .lookahead_char = -2,          // No leído aún
    .line_num = 1,
    .col_num = 0
};
```

### PASO 2.4: Crear array de estados de autómatas
```c
automaton_state_t states[MAX_AUTOMATA];  // =  [10]
// Estados no inicializados aún
```

### PASO 2.5: LOOP PRINCIPAL - Leer caracteres
```c
int c;
while ((c = read_char(&ctx)) != EOF) {
    // Loop principal que procesa todo el archivo
}
```

---

## PASO 3: LECTURA DEL PRIMER CARÁCTER
### ITERACIÓN 1 DEL WHILE - Primer carácter '('

### PASO 3.1: `read_char(&ctx)`
```c
// En read_char():
int c;
if (ctx->lookahead_char != -2) {   // ✗ false (-2)
    c = ctx->lookahead_char;
    ctx->lookahead_char = -2;
} else {
    c = fgetc(ctx->input_file);    // ✓ Lee del archivo
    // c = '(' (ASCII 40)
}

if (c == '\n') {                   // ✗ false
    ctx->line_num++;
    ctx->col_num = 0;
} else if (c != EOF) {             // ✓ true
    ctx->col_num++;                // col_num = 0 → 1
}

return c;  // Retorna '('
```

**Resultado:**
- **ctx.current_char (LOCAL):** -1 (sin usar)
- **ctx.line_num:** 1
- **ctx.col_num:** 1
- **ctx.lookahead_char:** -2
- **c (LOCAL VAR en StartScanner):** '(' (40)

### PASO 3.2: Validar que NO sea delimitador
```c
if (c == ' ' || c == '\t' || c == '\n') {  // ✗ false
    continue;
}
// ✓ No es delimitador, procede
```

### PASO 3.3: Inicializar estados de autómatas
```c
initialize_automaton_states(states, all_automata, num_automata);
// all_automata = [automaton2_t* special_chars, NULL, ...]
// num_automata = 1

// En initialize_automaton_states():
for (int i = 0; i < num_auto; i++) {  // i = 0
    states[i].automaton = automata[i];
    // states[0].automaton = automaton2_t* (special_chars)
    
    states[i].current_state = automata[i]->initial_state;
    // states[0].current_state = 0 (estado q0)
    
    states[i].is_active = 1;
    // states[0].is_active = 1 (activo)
}
```

**Estructura resultante:**
```c
automaton_state_t states[10] {
    [0] = {
        .automaton = automaton2_t* (special_chars),
        .current_state = 0,        // Estado inicial q0
        .is_active = 1
    },
    [1..9] = no inicializados
}
```

### PASO 3.4: Crear token_buffer y variables
```c
char token_buffer[TOKEN_BUFFER_SIZE] = {0};  // = [256] inicializado
int token_pos = 0;

token_candidate_t last_valid = {
    .buffer = {0},                 // Array [256] inicializado a 0
    .length = 0,
    .category = CAT_NONRECOGNIZED, // = 6
    .is_valid = 0
};
```

### PASO 3.5: LOOP INTERNO - Procesar caracteres del token
```c
while (c != EOF && c != ' ' && c != '\t' && c != '\n') {
    // c = '('
}
```

#### PASO 3.5.1: Guardar carácter en buffer
```c
token_buffer[token_pos] = (char)c;
// token_buffer[0] = '('

token_pos++;
// token_pos = 0 → 1
```

#### PASO 3.5.2: Procesar transiciones - `process_automata_transition()`
```c
process_automata_transition(states, num_automata, (char)c);
// states = [automaton_state_t, ...]
// num_automata = 1
// c = '('
```

**En process_automata_transition():**
```c
for (int i = 0; i < num_auto; i++) {  // i = 0
    if (!states[i].is_active) continue;  // ✓ is_active = 1
    
    // Buscar símbolo en alfabeto
    int symbol_idx = find_symbol_index(states[i].automaton, c);
    // states[0].automaton->alphabet = "();{}[],"
    // c = '('
    // find_symbol_index busca '(' en "();{}[],"
    // symbol_idx = 0 (posición 0)
    
    if (symbol_idx == -1) {        // ✗ false
        states[i].is_active = 0;
        continue;
    }
    
    // Intentar transición
    int next_state = states[i].automaton->transition_matrix[states[i].current_state][symbol_idx];
    // transition_matrix[0][0] = 1
    // next_state = 1
    
    if (next_state == -1) {        // ✗ false
        states[i].is_active = 0;
        continue;
    }
    
    // Transición válida
    states[i].current_state = next_state;
    // states[0].current_state = 0 → 1 (estado de aceptación)
    
    any_active = 1;
}
```

**Resultado después de transition:**
```c
states[0] = {
    .automaton = automaton2_t*,
    .current_state = 1,    // ✓ Estado de aceptación
    .is_active = 1
}
```

#### PASO 3.5.3: Buscar autómata aceptor - `find_accepting_automaton()`
```c
int accepting_idx = find_accepting_automaton(states, num_automata);
// num_automata = 1

for (int i = 0; i < num_auto; i++) {  // i = 0
    if (states[i].is_active && is_accepting_state(states[i].automaton, states[i].current_state)) {
        // states[0].is_active = 1 ✓
        // is_accepting_state(automaton2_t*, 1)
        //   → busca si 1 está en accepting_states[0] = 1
        //   → ✓ true
        
        return i;  // Retorna 0
    }
}
```

**Resultado:**
- **accepting_idx (LOCAL):** 0

#### PASO 3.5.4: Guardar token válido
```c
if (accepting_idx != -1) {         // ✓ true (accepting_idx = 0)
    strncpy(last_valid.buffer, token_buffer, token_pos);
    // Copia token_buffer[0..0] a last_valid.buffer
    // last_valid.buffer[0] = '('
    
    last_valid.buffer[token_pos] = '\0';
    // last_valid.buffer[1] = '\0'  (null terminator)
    
    last_valid.length = token_pos;
    // last_valid.length = 1
    
    last_valid.category = states[accepting_idx].automaton->category;
    // states[0].automaton->category = CAT_SPECIALCHAR = 5
    // last_valid.category = 5
    
    last_valid.is_valid = 1;
}
```

**Estructura resultante:**
```c
token_candidate_t last_valid = {
    .buffer = "(\0...",
    .length = 1,
    .category = 5 (CAT_SPECIALCHAR),
    .is_valid = 1
}
```

#### PASO 3.5.5: Lookahead del siguiente carácter - `peek_char()`
```c
int next_c = peek_char(&ctx);
// En peek_char():
if (ctx->lookahead_char == -2) {   // ✓ true
    ctx->lookahead_char = fgetc(ctx->input_file);
    // Lee siguiente carácter del archivo = ' ' (espacio)
    // ctx->lookahead_char = ' '
}
return ctx->lookahead_char;        // Retorna ' '
```

**Resultado:**
- **next_c (LOCAL):** ' ' (espacio, ASCII 32)

#### PASO 3.5.6: Validar si es delimitador
```c
if (next_c == EOF || next_c == ' ' || next_c == '\t' || next_c == '\n') {
    // ✓ true (next_c = ' ')
    break;  // Salir del loop interior
}
```

**Salida del loop de lectura de token**

#### PASO 3.5.7: Generación del token
```c
// RESULTADO FINAL DEL TOKEN
if (last_valid.is_valid) {         // ✓ true
    fprintf(OutputFile, "[TOKEN] '%s' -> CAT_%d\n", last_valid.buffer, last_valid.category);
    // OutputFile = stdout
    // last_valid.buffer = "("
    // last_valid.category = 5
    
    // OUTPUT en consola:
    // [TOKEN] '(' -> CAT_5
}
```

---

## PASO 4: ITERACIÓN 2 DEL LOOP - Saltar espacios

### PASO 4.1: Leer carácter (ya en lookahead)
```c
c = read_char(&ctx);
// En read_char():
if (ctx->lookahead_char != -2) {   // ✓ true (' ')
    c = ctx->lookahead_char;       // c = ' '
    ctx->lookahead_char = -2;
} else {
    c = fgetc(ctx->input_file);
}

if (c == '\n') {
    ...
} else if (c != EOF) {
    ctx->col_num++;                // col_num = 1 → 2
}

return c;  // Retorna ' '
```

### PASO 4.2: Validar delimitador
```c
if (c == ' ' || c == '\t' || c == '\n') {  // ✓ true
    continue;  // Salta a siguiente iteración del loop principal
}
```

**El espacio se ignora, no genera token**

---

## PASO 5: ITERACIÓN 3 DEL LOOP - Segundo carácter ')'

**Se repite el mismo proceso que la ITERACIÓN 1:**

### PASO 5.1: `read_char()` → c = ')'
```c
// lookahead_char = -2, lee nuevo carácter
// c = ')'
```

### PASO 5.2: NO es delimitador → procede

### PASO 5.3: Reinicializar estados
```c
initialize_automaton_states(states, all_automata, num_automata);
// states[0].current_state = 0
// states[0].is_active = 1
// Vuelve a estado inicial q0
```

### PASO 5.4: Crear nuevo token_buffer
```c
char token_buffer[TOKEN_BUFFER_SIZE] = {0};
int token_pos = 0;
token_candidate_t last_valid = {.is_valid = 0, ...};
```

### PASO 5.5: Procesar ')' 
```c
token_buffer[0] = ')';
token_pos = 1;

process_automata_transition(states, num_automata, ')');
// find_symbol_index(')', "();{}[],") = 1
// transition_matrix[0][1] = 1
// states[0].current_state = 0 → 1

finding_accepting_automaton() = 0;
// Guarda token válido

last_valid = {
    .buffer = ")\0...",
    .length = 1,
    .category = CAT_SPECIALCHAR = 5,
    .is_valid = 1
}

// Lookahead siguiente = ';'
peek_char() = ';'
// ¡No es delimitador! → No rompe

can_any_automaton_continue(states, num_automata, ';')
// ¿Puede q_accept procesar ';'?
// transition_matrix[1][symbol_idx(';')] = -1
// NO puede continuar → false
```

#### Lookahead detecta que nadie puede continuar
```c
if (!can_any_automaton_continue(states, num_automata, (char)next_c)) {
    // ✓ true
    break;  // Salir del loop interior
}
```

### PASO 5.6: Generar token
```c
fprintf(OutputFile, "[TOKEN] '%s' -> CAT_%d\n", ")", 5);
// OUTPUT: [TOKEN] ')' -> CAT_5
```

---

## PASO 6: ITERACIONES FINALES

**Se repiten los mismos pasos para:**
- `;` → Índice 2 en alfabeto → TOKEN '/' → CAT_5
- Espacio → Se ignora
- `{` → Índice 3 en alfabeto → TOKEN '{' → CAT_5
- `}` → Índice 4 en alfabeto → TOKEN '}' → CAT_5
- (Fin de línea)

---

## PASO 7: FIN DEL ARCHIVO

```c
while ((c = read_char(&ctx)) != EOF) {
    // Última iteración: fgetc() retorna EOF
    // c = -1 (EOF)
}
// ✗ Condición false, sale del loop
```

---

## PASO 8: LIBERACIÓN DE MEMORIA

```c
free_all_automata();
// En free_all_automata():
for (int i = 0; i < num_automata; i++) {  // i = 0
    if (all_automata[i]) {
        free_automaton(all_automata[i]);
        // Libera la memoria del autómata
        
        all_automata[i] = NULL;
    }
}
num_automata = 0;
```

---

## PASO 9: RETORNO A MAIN

```c
// Fin de StartScanner()
// Regresa a main()

fprintf(ofile, "\n=== END OF SCAN ===\n");
// OUTPUT: "=== END OF SCAN ===" en stdout

if (InputFile != stdin && InputFile != NULL) {
    fclose(InputFile);  // Cierra test_special_chars.txt
}

return 0;  // Termina el programa
```

---

## RESUMEN DE OUTPUT FINAL EN CONSOLA

```
=== SCANNER TEST - SPECIAL CHARACTERS ===

[SCANNER] Initialized 1 automata
[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
[TOKEN] ';' -> CAT_5
[TOKEN] '{' -> CAT_5
[TOKEN] '}' -> CAT_5

=== END OF SCAN ===
```

---

## ESTRUCTURAS PRINCIPALES USADAS

### 1. automaton2_t
```c
typedef struct {
    char* alphabet;                 // "();{}[],"
    int alphabet_size;              // 8
    int* states;                    // [0, 1]
    int num_states;                 // 2
    int initial_state;              // 0
    int* accepting_states;          // [1]
    int num_accepting_states;       // 1
    int** transition_matrix;        // [2][8]
    category_t category;            // CAT_SPECIALCHAR = 5
} automaton2_t;
```

### 2. automaton_state_t
```c
typedef struct {
    automaton2_t* automaton;        // Puntero al autómata
    int current_state;              // Estado actual (0 o 1)
    int is_active;                  // 0 o 1
} automaton_state_t;
```

### 3. scanner_context_t
```c
typedef struct {
    FILE* input_file;               // Archivo de entrada
    FILE* output_file;              // Archivo de salida
    int current_char;               // Sin usar
    int lookahead_char;             // Carácter lookahead
    int line_num;                   // Línea actual
    int col_num;                    // Columna actual
} scanner_context_t;
```

### 4. token_candidate_t
```c
typedef struct {
    char buffer[256];               // Lexema ("(", ")", etc)
    int length;                     // Longitud del lexema
    category_t category;            // Categoría (CAT_SPECIALCHAR)
    int is_valid;                   // 0 o 1
} token_candidate_t;
```

---

## VARIABLES GLOBALES Y LOCALES CLAVE

### Globales (static en scanner.c):
- `automaton2_t* all_automata[MAX_AUTOMATA]` - Array de punteros a autómatas
- `int num_automata` - Número de autómatas inicializados (1)

### Locales en main():
- `FILE* ofile` - Manejador de salida (stdout)
- `FILE* InputFile` - Manejador del archivo de entrada

### Locales en StartScanner():
- `scanner_context_t ctx` - Contexto del scanner
- `automaton_state_t states[MAX_AUTOMATA]` - Estados de cada autómata
- `int c` - Carácter actual
- `char token_buffer[256]` - Buffer temporal del token
- `int token_pos` - Posición en el buffer
- `token_candidate_t last_valid` - Último token válido encontrado

