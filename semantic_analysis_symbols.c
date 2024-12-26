#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "semantic_analysis_symbols.h"

void symbolResolutionExpr(exp_node * e, S_table global_types, S_table function_decs, frame * f) {

    if(!e) return;
    switch(e->kind){
        case int_exp: {
            return; //no variable to check
        }
        case str_exp: {
            return; //no variable to check
        }

        case binop_exp: {
            symbolResolutionExpr(e->data.bin_ops.e1,global_types,function_decs,f);
            symbolResolutionExpr(e->data.bin_ops.e2,global_types,function_decs,f);
            break;

        }
        case unop_exp:  {
                symbolResolutionExpr(e->data.un_ops.e,global_types,function_decs,f);// Not implemented
                 break;
        }
        // TODO add more cases for all expressions
        case var_read_exp: {



            ty_node * type = S_look(global_types,S_Symbol(e->data.sval));
            if(type==NULL) {
                if(f==NULL || (type = S_look(f->args_locs_types, S_Symbol(e->data.sval)))==NULL) {
                    fprintf(stderr,"Attempted to read variable %s without declaring it.",e->data.sval);
                    exit(73);
                }

                assert(type!=NULL);
            }
            return;


        }
        case funcall_exp: {
            fundec_node * fun = S_look(function_decs, S_Symbol(e->data.fun_ops.name));
            if(fun == NULL) {
                fprintf(stderr,"Function \"%s\" is called without declaration.\n", e->data.fun_ops.name);
                exit(73);
            }

            list * l = e->data.fun_ops.args;
            while(l!=NULL) {
                exp_node * e = l->head;
                symbolResolutionExpr(e,global_types,function_decs,f);
                l=l->next;
            }

            //assert(fun->params == NULL && e->data.fun_ops.args == NULL);
                /**/
            break;
        }
        case array_exp: {
            if(!S_look(global_types,S_Symbol(e->data.array_ops.name))) {
                fprintf(stderr,"Attempting to read array %s without declaring it.\n",e->data.array_ops.name);
                exit(73);
            }
            symbolResolutionExpr(e->data.array_ops.e,global_types,function_decs,f);
            return;
        }
        default:
            assert(0); // Should be dead code
    }
}


void symbolResolutionStmts(list * l, S_table locals, S_table function_decs, frame * f);

void symbolResolutionStmt(stmt_node * s, S_table global_types, S_table function_decs, frame * f) {
    if(!s) return;
    switch(s->kind){
        case if_stmt: {

            symbolResolutionExpr(s->data.if_ops.cond,global_types,function_decs,f);
            symbolResolutionStmts(s->data.if_ops.then_stmts,global_types,function_decs,f);
            symbolResolutionStmts(s->data.if_ops.else_stmts,global_types,function_decs,f);
            break;
        }
        case while_stmt: {
            symbolResolutionExpr( s->data.while_ops.guard,global_types,function_decs,f);
            symbolResolutionStmts( s->data.while_ops.body,global_types,function_decs,f);
            symbolResolutionStmts(s->data.while_ops.otherwise,global_types,function_decs,f);
            break;

        }
        case repeat_stmt: {

            symbolResolutionExpr(s->data.repeat_ops.count,global_types,function_decs,f);
            symbolResolutionStmts(s->data.repeat_ops.body,global_types,function_decs,f);
            break;

        }
        case ret_stmt: {
            exp_node * ret_exp = s->data.ret_exp;
            symbolResolutionExpr(ret_exp,global_types,function_decs,f);
            break;
        }
        // TODO add more cases for all expressions
        case assign_stmt: {
            /*if(f!=NULL) {
                if (!S_look(global_types, S_Symbol(s->data.assign_ops.lhs))) {
                    if (!S_look(f->args_locs_types, S_Symbol(s->data.assign_ops.lhs))) {
                        fprintf(stderr,"Trying to read variable \"%s\" without declaring it.\n", s->data.assign_ops.lhs);
                        exit(73);
                    }
                }
            }else {
                if (!S_look(global_types, S_Symbol(s->data.assign_ops.lhs))) {
                    fprintf(stderr,"Trying to read \"%s\" without declaring it.\n", s->data.assign_ops.lhs);
                    exit(73);
                }
            }

            */
            ty_node * type = S_look(global_types,S_Symbol(s->data.assign_ops.var_name));
            if(type==NULL) {
                if(f==NULL || (type = S_look(f->args_locs_types, S_Symbol(s->data.assign_ops.var_name)))==NULL){
                    fprintf(stderr,"Attempted to write variable %s without declaring it.", s->data.assign_ops.var_name);
                    exit(73);
                }
                assert(type!= NULL);

            }
            symbolResolutionExpr(s->data.assign_ops.init, global_types, function_decs, f);
            break;
        }
        case funcall_stmt: {
            /*if (!S_look(function_decs, S_Symbol(s->data.fun_ops.name))) {
                fprintf(stderr,"Function  %s is called without declaring it.\n", s->data.fun_ops.name);
                exit(73);
            }
            return;*/
            fundec_node * fun = S_look(function_decs, S_Symbol(s->data.fun_ops.name));
            if(fun == NULL) {
                fprintf(stderr,"Function \"%s\" is called without declaration.\n",s->data.fun_ops.name);
                exit(73);
            }
            //Support argument
            list * l = s->data.fun_ops.args;
            while(l!=NULL) {
                exp_node * e = l->head;
                symbolResolutionExpr(e,global_types,function_decs,f);
                l=l->next;
            }
            break;


        }
        case array_stmt: {

            if(S_look(global_types,S_Symbol(s->data.array_ops.name))){
                fprintf(stderr,"Attempting to define array with same name again %s.\n", s->data.array_ops.name);
                exit(73);
            }

            symbolResolutionExpr(s->data.array_ops.e,global_types,function_decs,f);
            symbolResolutionExpr(s->data.array_ops.init,global_types,function_decs,f);
            S_enter(global_types,S_Symbol(s->data.array_ops.name),s->data.array_ops.type);
            return;

        }
        default:
            assert(0); // Should be dead code
    }
}

void symbolResolutionStmts(list * l, S_table globals_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    symbolResolutionStmt(l->head, globals_types, function_decs, f);
    symbolResolutionStmts(l->next, globals_types, function_decs, f);
}

S_table implicit_types;
void symbolResolutionVariable(vardec_node * vardec, S_table globals_types, S_table function_decs, frame * f) {

    // TODO figure out if this is a global variable or a local variable
    // TODO Hint:  to start, just assume that everything is a global variable

    symbolResolutionExpr(vardec->init, globals_types, function_decs, f);

    if (vardec->implicit == 0) {
        //Register global variable types in global_types
        if(S_look(globals_types,S_Symbol(vardec->name))!=NULL || S_look(implicit_types,S_Symbol(vardec->name))!=NULL ) {
            fprintf(stderr,"Attempted define implicit variable \"%s\" with a duplicate name.\n", vardec->name);
            exit(73);
        }
        S_enter(globals_types, S_Symbol(vardec->name), vardec->type);
    } else {
        // Register implicit variable types in implicit_types
        if(S_look(globals_types,S_Symbol(vardec->name))!=NULL || S_look(implicit_types,S_Symbol(vardec->name))!=NULL) {
            fprintf(stderr,"Attempted define a variable \"%s\" with a duplicate name.\n", vardec->name);
            exit(73);
        }

        S_enter(implicit_types, S_Symbol(vardec->name), vardec->type);


    }

    // TODO check that all symbols in the variable initialization are known to fail negative tests


}

void symbolResolutionVariables(list * l, S_table global_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    symbolResolutionVariable((vardec_node *)l->head, global_types, function_decs, f);
    symbolResolutionVariables(l->next, global_types, function_decs, f);
}

extern program p;
void symbolResolutionFunction(fundec_node * fundec, S_table globals, S_table functions_decs, frame * f) {
    // Add each argument type to the f->args_locs_types
    {
        list * l = fundec->params;
        while(l!=NULL) {
            param * p = l->head;
            if(S_look(globals, S_Symbol(p->name))!= NULL && S_look(implicit_types, S_Symbol(p->name))== NULL ) {

                fprintf(stderr,"Attempted define a function argument %s with the same name as a global.\n", p->name);
                exit(73);

            }
            if(S_look(f->args_locs_types, S_Symbol(p->name))!= NULL) {

                fprintf(stderr,"Attempted define a function argument \"%s\" with a duplicate name.\n", p->name);
                exit(73);

            }
            S_enter(f->args_locs_types, S_Symbol(p->name), p->ty);

            l=l->next;
        }
    }

    // Add each local variable type to f->args_locs_types
    {
        list * l = fundec->locals;
        while(l!=NULL) {
            vardec_node * v = l->head;
            assert(v->implicit == 0);
            symbolResolutionExpr(v->init, globals,functions_decs,f);
            if(S_look(globals, S_Symbol(v->name))!= NULL) {

                fprintf(stderr,"Attmepted define a local variable %s with the same name as a global.\n", v->name);
                exit(73);

            }
            if(S_look(f->args_locs_types, S_Symbol(v->name))!= NULL) {

                fprintf(stderr,"Attmepted define a local variable \"%s\" with a duplicate name.\n", v->name);
                exit(73);

            }
            S_enter(f->args_locs_types, S_Symbol(v->name), v->type);

            l=l->next;
        }
    }
    // Add each implicit variable type to f->args_locs_types
    {
        list * l = p.variables;
        while(l!=NULL) {
            vardec_node * v = l->head;
            if(v->implicit != 1){
                l=l->next;
                continue;
            }

            symbolResolutionExpr(v->init, globals,functions_decs,f);

            if(S_look(f->args_locs_types, S_Symbol(v->name)) == NULL) {

                S_enter(f->args_locs_types, S_Symbol(v->name), v->type);

            }

            l=l->next;
        }
    }


    /*list * l = fundec->locals;
    while (l != NULL) {
        vardec_node * vdec = (vardec_node*) l->head;
        if (S_look(globals, S_Symbol(vdec->name))) {
            fprintf(stderr,"Re-defining local variable with same name as global variable \"%s\".\n", vdec->name);
            exit(73);
        }
        if (S_look(f->args_locs_types, S_Symbol(vdec->name))) {
            fprintf(stderr,"Re-defining local variable with same name as argument \"%s\".\n", vdec->name);
            exit(73);
        }
    S_enter(functions_rets, S_Symbol(fundec->name), fundec);
    l=l->next
    }
*/
    // TODO check that the statements in the body of the function only use known variables and functions

    symbolResolutionStmts(fundec->body, globals, functions_decs, f);

    return;
}

void symbolResolutionFunctions(list * l, S_table global_types, S_table function_decs, S_table frames) {
    if (l == NULL) return;
    fundec_node * fundec = (fundec_node*) l->head;
    /*if (S_look(function_decs, S_Symbol(fundec->name))) {
        fprintf(stderr,"Redefining duplicate function %s.\n", fundec->name);
        exit(73);
    }*/

    // Create a new frame
    frame * f = malloc(sizeof(frame));

    // Initialize all memory so we can read it later
    f->args_locs_types = S_empty();
    f->indexes = S_empty();
    f->ret = fundec->ret;

    // Add newly computed frame to frames
    if(S_look(frames,S_Symbol(fundec->name))!=NULL) {
        fprintf(stderr,"Attempted to define a function with a duplicate name %s.", fundec->name);
        exit(73);
    }
    S_enter(frames, S_Symbol(fundec->name), f);
    S_enter(function_decs, S_Symbol(fundec->name),fundec);

    symbolResolutionFunction(fundec, global_types, function_decs, f);
    symbolResolutionFunctions(l->next, global_types, function_decs, frames);
}

void symbolResolution(program * p, S_table global_types, S_table function_decs, S_table frames) {
    implicit_types = S_empty();
    symbolResolutionVariables(p->variables, global_types, function_decs, NULL);
    symbolResolutionFunctions(p->functions, global_types, function_decs, frames);
    symbolResolutionStmts(p->statements, global_types, function_decs, NULL);
}
