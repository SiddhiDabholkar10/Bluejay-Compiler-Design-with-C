#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symbol.h"
#include "y.tab.h"
#include "semantic_analysis_symbols.h"
#include "semantic_analysis_types.h"
#include "frames.h"
#include "print_ast.h"

extern FILE *yyin;
extern int lexer();

program p;
static void destroy_frame(S_symbol sym,void * binding) {
  frame * f = binding;
    S_destroy((f->args_locs_types),NULL);
    S_destroy(f->indexes,NULL);
    free(f->args_locs_types);
    free(f->indexes);

    free(f);
}


static void * types, *retTypes, *framess;

int main(int argc, char **argv) {
 char * fname;
 if (argc!=2) {fprintf(stderr,"usage: yacctest filename\n"); return 1;}

 fname=argv[1];

 p.variables = NULL;
 p.functions = NULL;
 p.statements = NULL;

 yyin = fopen(fname,"r");
 if (!yyin) { fprintf(stderr, "cannot open %s\n", fname); return 1; }

 int ret = yyparse();

 // Map from S_Symbol to ty_node
 S_table globals_types = S_empty();

 // Map from S_Symbol to ty_node
 S_table functions_ret_types = S_empty();

 // Map from S_Symbol to fun_frame
 S_table functions_frames = S_empty();


 // symbol analysis
 symbolResolution(&p, globals_types, functions_ret_types, functions_frames);

 // type checking
 typeCheck(&p, globals_types, functions_ret_types, functions_frames);

 // compute frames
 frames(&p, globals_types, functions_ret_types, functions_frames);

 // print results
 printSymbolsNamesTypes(&p, globals_types, functions_ret_types, functions_frames);

 fflush(stdout);


  types = globals_types;
 retTypes = functions_ret_types;
 framess = functions_frames;

 S_destroy(globals_types,NULL);
 S_destroy(functions_ret_types,NULL);
 S_destroy(functions_frames,destroy_frame);

 free(globals_types);
 free(functions_ret_types);
 free(functions_frames);

 return ret;



}

