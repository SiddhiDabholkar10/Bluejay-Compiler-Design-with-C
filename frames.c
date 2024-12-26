#include <stdio.h>
#include<stdlib.h>
#include "ast.h"
#include "frames.h"

#include <assert.h>
extern program p;
void framesFunction(fundec_node * fundec, S_table globals, S_table functions_rets, S_table frames) {
     S_table  added = S_empty();
    // Get frame created during symbol resolution
    __attribute__ ((unused))
    frame * f = S_look(frames, S_Symbol(fundec->name));
    long count = 0;

    // TODO add each argument position to indexes
    {
        list * l = fundec->params;
        while (l != NULL) {
            param* p = l->head;
            S_enter(f->indexes, S_Symbol(p->name), (void*)count);
            S_enter(added, S_Symbol(p->name), (void*)1);
            count += 1;
            l = l->next;
        }
    }

    // TODO add each local variable position to indexes

    {
        list * l = fundec->locals;
        while (l != NULL) {
            vardec_node * v = l->head;
            S_enter(f->indexes, S_Symbol(v->name), (void*)count);
            S_enter(added, S_Symbol(v->name),(void*)1);
            count += 1;
            l = l->next;

        }

    }



    // TODO don't forget implicit variables!
    {
        list * l = p.variables;
        while (l != NULL) {
            vardec_node * v = l->head;
            if(v->implicit != 1) {
                l=l->next;
                continue;
            }
            if(S_look(added,S_Symbol(v->name))==0) {
                S_enter(f->indexes, S_Symbol(v->name), (void*)count);
                count += 1;
            }
            l = l->next;


        }

    }
    S_destroy(added,NULL);
    free(added);

}

void framesFunctions(list * l, S_table global_types, S_table function_decs, S_table frames) {
    if (l == NULL) return;
    fundec_node * fundec = (fundec_node*) l->head;
    framesFunction(fundec, global_types, function_decs, frames);
    framesFunctions(l->next, global_types, function_decs, frames);
}

void frames(program * p, S_table global_types, S_table function_decs, S_table frames) {
    framesFunctions(p->functions, global_types, function_decs, frames);
}
