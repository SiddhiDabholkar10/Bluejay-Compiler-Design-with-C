#include <stdio.h>
#include <assert.h>
#include "ast.h"
#include "print_ast.h"
#include <stdlib.h>

void printSymbolsNamesTypesExpr(exp_node * e, S_table global_types, S_table function_decs, frame * f) {
    if(!e) return;
    switch(e->kind){
        case int_exp: {
            return;  //Nothing is printed here

        }
        case str_exp: {
            return;
        }


        case binop_exp: {
            printSymbolsNamesTypesExpr(e->data.bin_ops.e1, global_types, function_decs, f);
            printSymbolsNamesTypesExpr(e->data.bin_ops.e2, global_types, function_decs, f);

            return;  // Not implemented

        }
        case unop_exp: {

            printSymbolsNamesTypesExpr(e->data.un_ops.e, global_types, function_decs, f);
            return;

        }
        case var_read_exp: {
            char * varname = e->data.sval;
            ty_node * type = S_look(global_types,S_Symbol(varname));
            if(type != NULL) {
                printf("Variable read \"%s\" type %s\n",varname, typeToStr(type));
            }
            else {
                type = S_look(f->args_locs_types,S_Symbol(varname));
                long idx = (long)S_look(f->indexes, S_Symbol(varname));
                assert(type!= NULL);
                printf("Argument/local read \"%s\" type %s frame position %ld\n",varname,typeToStr(type),idx);
                assert(type != NULL);


            }
            break;
        }
            case funcall_exp: {
                fundec_node * fun = S_look(function_decs, S_Symbol(e->data.fun_ops.name));
                assert(fun != NULL) ;
                printf("Function called \"%s\" returns %s\n", e->data.fun_ops.name, typeToStr(fun->ret));
                break;
            }
        case array_exp: {
            ty_node * ret = (ty_node *)S_look(global_types,S_Symbol(e->data.array_ops.name));
            if(ret == NULL) {
                ret = (ty_node *) S_look(f->args_locs_types,S_Symbol(e->data.array_ops.name));
                unsigned long idx = (unsigned long) S_look(f->indexes,S_Symbol(e->data.array_ops.name));
                printf("Argument/local read \"%s\" type %s frame position %ld\n", e->data.array_ops.name, typeToStr(ret), idx);
            }else {
                printf("Array read \"%s\" type %s\n",e->data.array_ops.name,typeToStr(ret));

            }
            return;

        }

            // TODO add more cases for all expressions
            default:
                assert(0); // Should be dead code
        }
    }


void printSymbolsNamesTypesStmts(list * l, S_table globals_types, S_table function_decs, frame * f);

void printSymbolsNamesTypesStmt(stmt_node * s, S_table global_types, S_table function_decs, frame * f) {
    if(!s) return;
    switch(s->kind){
        case if_stmt: {
            printSymbolsNamesTypesExpr(s->data.if_ops.cond,global_types,function_decs,f);
            printSymbolsNamesTypesStmts(s->data.if_ops.then_stmts,global_types,function_decs,f);
            printSymbolsNamesTypesStmts(s->data.if_ops.else_stmts,global_types,function_decs,f);
            break;
        }
        case while_stmt: {
            printSymbolsNamesTypesExpr(s->data.while_ops.guard,global_types,function_decs,f);
            printSymbolsNamesTypesStmts(s->data.while_ops.body,global_types,function_decs,f);
            printSymbolsNamesTypesStmts(s->data.while_ops.otherwise,global_types,function_decs,f);
            break;
        }
        case repeat_stmt: {
            printSymbolsNamesTypesExpr(s->data.repeat_ops.count,global_types,function_decs,f);
            printSymbolsNamesTypesStmts(s->data.repeat_ops.body,global_types,function_decs,f);
            break;
        }
        case ret_stmt: {

            printSymbolsNamesTypesExpr(s->data.ret_exp,global_types,function_decs,f);
            break;
        }
        case assign_stmt: {
            /*printSymbolsNamesTypesExpr(s->data.assign_ops.init, global_types, function_decs, f);
            ty_node * type = (ty_node *) S_look(global_types, S_Symbol(s->data.assign_ops.lhs));
            if (type == NULL) {
                type = (ty_node *) S_look(f->args_locs_types, S_Symbol(s->data.assign_ops.lhs));
                assert(type);
                long pos = (long) S_look(f->indexes, S_Symbol(s->data.assign_ops.lhs));
                printf("Argument/local written \"%s\" type %s frame position %ld\n", s->data.assign_ops., typeToStr(type), pos);
            }else {
                printf("Variable written \"%s\" type %s\n", s->data.assign_ops.lhs, typeToStr(type));
            }*/
            printSymbolsNamesTypesExpr(s->data.assign_ops.init, global_types, function_decs, f);
            char * varname = s->data.assign_ops.var_name;
            ty_node * type = S_look(f->args_locs_types, S_Symbol(varname));
            if(type == NULL) {
                type = S_look(global_types, S_Symbol(varname));
                assert(type!=NULL);
               printf("Variable written \"%s\" type %s\n",varname,typeToStr(type) );

            }else {
                long idx = (long)S_look(f->indexes, S_Symbol(varname));
                printf("Argument/local written \"%s\" type %s frame position %ld\n",varname,typeToStr(type),idx);
            }
            break;
        }
        case funcall_stmt: {
            fundec_node * ret = S_look(function_decs, S_Symbol(s->data.fun_ops.name));
            assert(ret);
            printf("Function called \"%s\" returns %s\n", s->data.fun_ops.name, typeToStr(ret->ret));
            break;
        }
        case array_stmt: {
            printSymbolsNamesTypesExpr(s->data.array_ops.e,global_types,function_decs,f);
            printf("Array written \"%s\" type %s\n", s->data.array_ops.name, typeToStr(s->data.array_ops.type));
            break;
        }
        // TODO add more cases for all expressions
        default:
            assert(0); // Should be dead code
    }
}

void printSymbolsNamesTypesStmts(list * l, S_table globals_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    while(l!=NULL) {
        stmt_node * s = (stmt_node*)l->head;
        printSymbolsNamesTypesStmt(s,globals_types,function_decs,f );
        l=l->next;
    }
    // TODO iterate over list of statements
}



void printSymbolsNamesTypesVariables(list * l, S_table global_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    while(l!=NULL) {

        vardec_node * v = l->head;
        if(v->implicit == 1) {
            l=l->next;
            continue;
        }

        printf("Variable declared \"%s\" type %s\n", v->name, typeToStr(v->type));
        printSymbolsNamesTypesExpr(v->init, global_types, function_decs, f);
        l=l->next;
    }
    //assert(0); // TODO print details of list of variables
}
/*void printSymbolsNamesTypesFunction(fundec_node * fundec, S_table globals, S_table functions_decs, frame * f) {
    fundec_node * funty = (fundec_node*) S_look(functions_decs, S_Symbol(fundec->name));


    list * localls = fundec->locals;

    while (localls != NULL) {
        assert(f);
        vardec_node * p = (vardec_node *) localls->head;
        ty_node * ty = S_look(f->args_locs_types, S_Symbol(p->name));
        assert(ty);
        long pos = (long) S_look(f->indexes, S_Symbol(p->name));
        printf("\tLocal variable \"%s\" type %s position %ld\n", p->name, typeToStr(ty), pos);
        printSymbolsNamesTypesExpr(p->init, globals, functions_decs, f);
        localls = localls->next;
    }
    printSymbolsNamesTypesStmts(fundec->body, globals, functions_decs, f);

}*/
extern program p;
void printSymbolsNamesTypesFunctions(list * l, S_table global_types, S_table function_decs, S_table frames) {
    if (l == NULL) return;
    S_table printed = S_empty();
    while(l!=NULL) {
        fundec_node * fun = l->head;
        frame * f = S_look(frames, S_Symbol(fun->name));
        assert(f!=NULL);
        printf("Function declared \"%s\" returns %s\n",fun->name,typeToStr(fun->ret));
        //pasted code starts
        {
            list * l = fun->params;
            while(l!=NULL) {
                param * p = l->head;
                long idx = (long)S_look(f->indexes, S_Symbol(p->name));
                printf("\tArgument \"%s\" type %s position %ld\n",p->name,typeToStr(p->ty),idx);
                S_enter(printed,S_Symbol(p->name),(void*)1);
                l=l->next;
            }
        }
        {
            list * l = fun->locals;
            while(l!=NULL) {
                vardec_node * v = l->head;
                long idx = (long)S_look(f->indexes, S_Symbol(v->name));
                printf("\tLocal variable \"%s\" type %s position %ld\n",v->name,typeToStr(v->type),idx);

                printSymbolsNamesTypesExpr(v->init, global_types, function_decs,f);
                S_enter(printed,S_Symbol(v->name),(void*)1);

                l=l->next;
            }
        }
        {
            list * l = p.variables;
            while(l!=NULL) {
                vardec_node * v = l->head;
                if(v->implicit != 1 || S_look(printed,S_Symbol(v->name))!= 0) {
                    l=l->next;
                    continue;
                }
                long idx = (long)S_look(f->indexes, S_Symbol(v->name));
                printf("\tImplicit variable \"%s\" type %s position %ld\n",v->name,typeToStr(v->type),idx);
                printSymbolsNamesTypesExpr(v->init, global_types, function_decs,f);


                l=l->next;
            }
        }
        //pasted code ends


        printSymbolsNamesTypesStmts(fun->body, global_types, function_decs, f);
        l=l->next;

    }
    S_destroy(printed,NULL);
    free(printed);


     // TODO print details of list of function declarations
}

// Used for code and variables outside of functions
static frame global_empty_frame;

void printSymbolsNamesTypes(program * p, S_table global_types, S_table function_decs, S_table frames) {

    global_empty_frame.args_locs_types = S_empty();
    global_empty_frame.indexes = S_empty();

    printSymbolsNamesTypesVariables(p->variables, global_types, function_decs, &global_empty_frame);
    printSymbolsNamesTypesFunctions(p->functions, global_types, function_decs, frames);
    printSymbolsNamesTypesStmts(p->statements, global_types, function_decs, &global_empty_frame);
}
