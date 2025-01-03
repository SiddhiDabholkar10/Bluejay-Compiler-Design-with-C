#ifndef SYMBOL_H
#define SYMBOL_H

/*
 * symbol.h - Symbols and symbol-tables
 *
 */

typedef struct S_symbol_ *S_symbol;

/* Make a unique symbol from a given string.  
 *  Different calls to S_Symbol("foo") will yield the same S_symbol
 *  value, even if the "foo" strings are at different locations. */
S_symbol S_Symbol(char*);

/* Extract the underlying string from a symbol */
char* S_name(S_symbol);

/* S_table is a mapping from S_symbol->any, where "any" is represented
 *     here by void*  */
typedef struct TAB_table_ *S_table;

/* Make a new table */
S_table S_empty(void);

/* Enter a binding "sym->value" into "t", shadowing but not deleting
 *    any previous binding of "sym". */
void S_enter(S_table t, S_symbol sym, void *value);

/* Look up the most recent binding of "sym" in "t", or return NULL
 *    if sym is unbound. */
void *S_look(S_table t, S_symbol sym);

/* Start a new "scope" in "t".  Scopes are nested. */
void S_beginScope(S_table t);

/* Remove any bindings entered since the current scope began,
   and end the current scope. */
void S_endScope(S_table t);

/* Call "show" on every "key"->"value" pair in the table
 * show is a function defined as:
 * void show(S_symbol, void*);
 * */
void S_dump(S_table t, void (*show)(S_symbol sym, void *binding));

/* Call "destructor" on every "key"->"value" pair in the table.
 * Also frees internal memory to the table
 * destructor is a function defined as:
 * void destructor(S_symbol, void*);
 * */
void S_destroy(S_table t, void (*destructor)(S_symbol sym, void *binding));

#endif
