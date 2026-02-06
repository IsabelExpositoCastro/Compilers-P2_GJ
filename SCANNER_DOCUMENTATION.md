# DOCUMENTACIÓN DE FUNCIONAMIENTO DEL SCANNER - PRÁCTICA 2

## Tabla de Contenidos
1. [Estructura del Proyecto](#estructura-del-proyecto)
2. [Flujo General del Scanner](#flujo-general-del-scanner)
3. [Componentes Principales](#componentes-principales)
4. [Ejemplo Paso a Paso con test_input.txt](#ejemplo-paso-a-paso)
5. [Detalle Técnico de la Arquitectura](#detalle-técnico)

---

## Estructura del Proyecto

```
src/
├── main.c                           # Punto de entrada del programa
├── scanner_module/
│   ├── scanner.h                    # Header del scanner
│   └── scanner.c                    # Implementación del scanner con lógica de autómatas
├── automatas_module/
│   ├── automatonDefinition.h        # Definición genérica de autómatas
│   ├── automatonDefinition.c        # Implementación de funciones genéricas
│   ├── special_characters.h         # Header del autómata de caracteres especiales
│   └── special_characters.c         # Implementación del autómata específico
└── ...otros módulos

build/
└── test_input.txt                   # Archivo de entrada para pruebas
```

---

## Flujo General del Scanner

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. PROGRAMA INICIA (main.c)                                     │
│    - Abre archivo de entrada (stdin en este caso)               │
│    - Configura output a stdout                                  │
│    - Llama a StartScanner()                                     │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ 2. INICIALIZAR AUTÓMATAS (scanner.c - initialize_all_automata) │
│    - Crea el autómata de caracteres especiales                  │
│    - Valida que esté bien formado                               │
│    - Almacena en array global all_automata[]                    │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ 3. LECTURA DEL ARCHIVO CARÁCTER POR CARÁCTER                   │
│    - Lee cada carácter                                          │
│    - Ignora espacios/tabs/newlines en el inicio                 │
│    - Para cada carácter no-delimitador, INICIA NUEVO TOKEN      │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ 4. PARA CADA TOKEN: EJECUTAR AUTÓMATAS EN PARALELO              │
│                                                                  │
│    a) Inicializar estados de todos los autómatas                │
│       - Estado inicial = initial_state del autómata             │
│       - is_active = 1 (todos activos)                           │
│                                                                  │
│    b) Para cada carácter del token:                             │
│       - Buscar símbolo en alfabeto de cada autómata             │
│       - Si NO está → autómata MUERE (is_active = 0)            │
│       - Si está → intentar transición en matriz                 │
│       - Si transición = -1 → autómata MUERE                    │
│       - Si transición válida → actualizar estado actual         │
│                                                                  │
│    c) Después de procesar carácter, GUARDAR si alguno acepta    │
│       - Si es_accepting_state() == 1 → guardar como candidato   │
│                                                                  │
│    d) LOOKAHEAD: ¿puede continuar algún autómata?              │
│       - Mirar siguiente carácter SIN consumirlo                 │
│       - Si ninguno puede continuar → PARAR y emitir token       │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ 5. EMITIR RESULTADO                                             │
│    - Si hay candidato válido → [TOKEN] 'lexema' -> CAT_X        │
│    - Si no → [ERROR] Non-recognized token                       │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ 6. REPETIR hasta EOF                                            │
└─────────────────────────────────────────────────────────────────┘
```

---

## Componentes Principales

### 1. **automatonDefinition.h / .c** - Infraestructura Genérica

Define la estructura `automaton2_t`:
```c
typedef struct {
    char* alphabet;              // Ej: "();{}[],"
    int alphabet_size;           // Ej: 8
    int* states;                 // Estados numerados [0, 1, 2, ...]
    int num_states;              // Ej: 2 (q0, q_accept)
    int initial_state;           // Ej: 0
    int* accepting_states;       // Ej: [1]
    int num_accepting_states;    // Ej: 1
    int** transition_matrix;     // Matriz [num_states][alphabet_size]
    category_t category;         // Categoría del token (CAT_SPECIALCHAR, etc.)
} automaton2_t;
```

**Funciones proporcionadas:**
- `create_empty_automaton()` - Crea autómata vacío
- `set_alphabet()` - Asigna alfabeto
- `set_num_states()` - Asigna número de estados y aloca memoria
- `set_initial_state()` - Asigna estado inicial
- `set_accepting_states()` - Asigna qué estados aceptan
- `set_transition_matrix()` - Llena la matriz de transiciones
- `set_category()` - Asigna categoría del token
- `validate_automaton()` - Valida que todo esté bien
- `free_automaton()` - Libera memoria
- `find_symbol_index()` - Busca índice de símbolo en alfabeto
- `is_accepting_state()` - Verifica si un estado es de aceptación
- `print_automaton_info()` - Muestra info del autómata (debug)

### 2. **special_characters.h / .c** - Autómata Específico

Crea el autómata para reconocer caracteres especiales: `( ) ; { } [ ] ,`

**Estructura:**
```
Estado q0 (inicial)
    ↓ --'('--> Estado q_accept (aceptación)
    ↓ --')'--> Estado q_accept
    ↓ --';'--> Estado q_accept
    ... (para todos los 8 caracteres especiales)
    
Estado q_accept
    ↓ --cualquier símbolo--> ERROR (-1)
```

**Inicialización:**
```c
automaton2_t* create_special_char_automaton() {
    automaton = create_empty_automaton();
    set_alphabet(automaton, "();{}[],");      // 8 caracteres
    set_num_states(automaton, 2, 1);          // 2 estados, 1 aceptor
    set_initial_state(automaton, 0);          // q0
    set_accepting_states(automaton, [1], 1);  // q_accept
    
    // Llenar matriz: desde q0, todos los símbolos → q_accept
    automaton->transition_matrix[0][0..7] = 1;  // Todos a q_accept
    automaton->transition_matrix[1][0..7] = -1; // Desde q_accept, error
    
    set_category(automaton, CAT_SPECIALCHAR);
    validate_automaton(automaton);
}
```

### 3. **scanner.h / .c** - Orquestador de Autómatas

**Estructuras principales:**
```c
typedef struct {
    automaton2_t* automaton;
    int current_state;      // Estado actual en este autómata
    int is_active;          // ¿Todavía puede reconocer?
} automaton_state_t;

typedef struct {
    FILE* input_file;
    FILE* output_file;
    int current_char;
    int lookahead_char;     // Para peek sin consumir
    int line_num;
    int col_num;
} scanner_context_t;
```

**Funciones principales:**
- `initialize_all_automata()` - Crea todos los autómatas
- `initialize_automaton_states()` - Inicializa estados de autómatas
- `can_any_automaton_continue()` - Verifica si alguno puede continuar
- `process_automata_transition()` - Ejecuta transiciones
- `find_accepting_automaton()` - Encuentra cuál aceptó
- `StartScanner()` - Función principal del scanner

---

## Ejemplo Paso a Paso

### Archivo de entrada: test_input.txt

```
( ) ; { } [ ] ,
( ) ( )
; ; ;

```

### Ejecución Detallada

#### **PASO 1: Inicialización**

```
main.c:
  ├─ Abre stdin
  ├─ Configura ofile = stdout
  └─ Llama StartScanner(stdin, stdout)

scanner.c - initialize_all_automata():
  ├─ Crea autómata especial_char_automaton
  │   ├─ alphabet = "();{}[],"
  │   ├─ num_states = 2
  │   ├─ initial_state = 0
  │   ├─ accepting_states = [1]
  │   ├─ transition_matrix = [[1,1,1,1,1,1,1,1], [-1,-1,-1,-1,-1,-1,-1,-1]]
  │   └─ category = CAT_SPECIALCHAR (5)
  │
  └─ num_automata = 1

SALIDA:
[VALIDATE] Autómata válido
[SCANNER] Initialized 1 automata
```

#### **PASO 2: Lectura del primer token '('**

```
Scanner lee carácter '('

Inicializar estados automáticos:
  automaton_state[0]:
    - automaton = special_char_automaton
    - current_state = 0 (estado inicial q0)
    - is_active = 1

Procesar carácter '(':
  
  Para automaton[0] (special_char):
    1. find_symbol_index(automaton, '(')
       → Busca en "();{}[],"
       → Encuentra en índice 0
       → symbol_idx = 0
    
    2. transition_matrix[0][0] = 1
       → Transición válida a estado 1 (q_accept)
    
    3. current_state = 1
    4. is_active = 1 (sigue activo)
    5. is_accepting_state(automaton, 1) == 1
       → ¡ACEPTADO! Guardar candidato: '(' con categoría 5

Lookahead: siguiente carácter es ' ' (espacio)
  → No está en alfabeto de ningún autómata
  → can_any_automaton_continue() retorna 0
  → PARAR

SALIDA:
[TOKEN] '(' -> CAT_5
```

#### **PASO 3: Lectura del segundo token ')'**

```
Scanner lee carácter ')'

Inicializar estados:
  automaton_state[0]:
    - current_state = 0
    - is_active = 1

Procesar carácter ')':
  
  Para automaton[0]:
    1. find_symbol_index(automaton, ')') = 1
    2. transition_matrix[0][1] = 1
    3. current_state = 1
    4. is_accepting_state(automaton, 1) == 1
       → ACEPTADO: ')' con categoría 5

Lookahead: ' ' (espacio)
  → PARAR

SALIDA:
[TOKEN] ')' -> CAT_5
```

#### **PASO 4: Lectura del tercer token ';'**

```
Igual que los anteriores...

SALIDA:
[TOKEN] ';' -> CAT_5
```

#### **Repetir para { } [ ] , en línea 1**

Cada uno sigue exactamente el mismo patrón:
- Lee carácter
- Busca en alfabeto → encuentra
- Transición de q0 a q_accept
- Lookahead ve espacio
- Para y emite token

```
[TOKEN] '{' -> CAT_5
[TOKEN] '}' -> CAT_5
[TOKEN] '[' -> CAT_5
[TOKEN] ']' -> CAT_5
[TOKEN] ',' -> CAT_5
```

#### **PASO 5: Línea 2 - "( ) ( )"**

```
Mismo proceso que línea 1:

[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
```

#### **PASO 6: Línea 3 - "; ; ;"**

```
[TOKEN] ';' -> CAT_5
[TOKEN] ';' -> CAT_5
[TOKEN] ';' -> CAT_5
```

#### **PASO 7: Línea vacía**

```
EOF alcanzado
free_all_automata()
```

### Salida Final Completa

```
=== SCANNER TEST - SPECIAL CHARACTERS ===

[VALIDATE] Autómata válido
[SCANNER] Initialized 1 automata
[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
[TOKEN] ';' -> CAT_5
[TOKEN] '{' -> CAT_5
[TOKEN] '}' -> CAT_5
[TOKEN] '[' -> CAT_5
[TOKEN] ']' -> CAT_5
[TOKEN] ',' -> CAT_5
[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
[TOKEN] '(' -> CAT_5
[TOKEN] ')' -> CAT_5
[TOKEN] ';' -> CAT_5
[TOKEN] ';' -> CAT_5
[TOKEN] ';' -> CAT_5

=== END OF SCAN ===
```

---

## Detalle Técnico

### Matriz de Transición del Autómata SPECIAL_CHARACTERS

La matriz es `2 x 8` (2 estados × 8 símbolos):

```
              (  )  ;  {  }  [  ]  ,
        q0:   1  1  1  1  1  1  1  1
        q_accept: -1 -1 -1 -1 -1 -1 -1 -1
```

**Explicación:**
- **Desde q0:** Para cualquier símbolo del alfabeto, transicionar a q_accept
- **Desde q_accept:** No hay transiciones válidas (todas retornan -1, indicando error)

### Lookahead de 1 Carácter

```c
// Función: can_any_automaton_continue()

int lookahead_char = peek_char(ctx);  // Leer sin consumir

for (int i = 0; i < num_automata; i++) {
    if (!states[i].is_active) continue;
    
    int symbol_idx = find_symbol_index(states[i].automaton, lookahead_char);
    if (symbol_idx == -1) continue;  // Símbolo no en alfabeto
    
    int next_state = states[i].automaton->transition_matrix[
                        states[i].current_state][symbol_idx];
    
    if (next_state != -1) {
        return 1;  // Alguno puede continuar
    }
}

return 0;  // Ninguno puede continuar → PARAR
```

**Ejemplo con '(' seguido de espacio:**
```
Lookahead ve ' ' (espacio)
  - ¿Está en "();{}[],"? NO
  - can_any_automaton_continue() retorna 0
  - Scanner para y emite el token
```

### Categorías de Token

```c
enum category_t {
    CAT_NUMBER      = 0,
    CAT_IDENTIFIER  = 1,
    CAT_KEYWORD     = 2,
    CAT_LITERAL     = 3,
    CAT_OPERATOR    = 4,
    CAT_SPECIALCHAR = 5,     // ← Nuestro autómata
    CAT_NONRECOGNIZED = 6
};
```

Por eso los tokens muestran `CAT_5` (el número 5 = CAT_SPECIALCHAR).

---

## Flujo de Memoria

```
create_empty_automaton()
    ↓
    malloc(sizeof(automaton2_t))
    ├─ automaton = pointer a struct (heap)
    │
set_alphabet()
    ├─ malloc() para alphabet string
    ├─ strcpy() contenido
    │
set_num_states(2, 1)
    ├─ malloc() para states array [2]
    ├─ malloc() para accepting_states [1]
    ├─ malloc() para transition_matrix [2 filas]
    │   ├─ malloc() para fila 0 [8 elementos]
    │   └─ malloc() para fila 1 [8 elementos]
    │
set_accepting_states()
    ├─ memcpy() valores al array
    │
... otras asignaciones ...

free_automaton()
    └─ free() en orden inverso
        ├─ free() transition_matrix filas
        ├─ free() transition_matrix
        ├─ free() accepting_states
        ├─ free() states
        ├─ free() alphabet
        └─ free() automaton
```

---

## Resumen Conceptual

1. **Autómatas en Paralelo**: El scanner ejecuta todos los autómatas simultáneamente
2. **Muerte Selectiva**: Los autómatas "mueren" cuando no pueden continuar
3. **Lookahead**: Determina cuándo parar de leer caracteres para un token
4. **Candidato Válido**: Se guarda el último estado de aceptación alcanzado
5. **Categorización**: Cada autómata asigna una categoría al token reconocido

Este diseño es **modular, extensible y eficiente**: agregar nuevos autómatas (keywords, números, identificadores) es tan simple como crear una función `create_xxx_automaton()` siguiendo el mismo patrón.

---

**Documento generado: Explicación completa del funcionamiento del Scanner con test_input.txt**
