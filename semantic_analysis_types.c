#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "semantic_analysis_types.h"

ty_node * typeCheckExpr(exp_node * e, S_table global_types, S_table function_decs, frame * f) {

    switch(e->kind){
        case int_exp: {
            return IntTyNode();
        }
        case str_exp: {
            return StrTyNode();
        }
        case binop_exp: {
            ty_node* actual_left = typeCheckExpr(e->data.bin_ops.e1,global_types,function_decs,f);
            ty_node* actual_right = typeCheckExpr(e->data.bin_ops.e2,global_types,function_decs,f);
            ty_node* expected = IntTyNode();
            if(expected != actual_left || expected!=actual_right) {
                fprintf(stderr,"Wrong data type for binary operator. Expected %s got %s and %s",typeToStr(expected),typeToStr(actual_left),typeToStr(actual_right));
                exit(73);
            }
            return expected;
            break;
        }
        case unop_exp: {
            ty_node* expected = typeCheckExpr(e->data.un_ops.e,global_types,function_decs,f);
            ty_node* actual = IntTyNode();
            if(expected != actual) {
                fprintf(stderr,"Wrong data type for unary operator. Expected %s got %s",typeToStr(expected),typeToStr(actual));
                exit(73);
            }
            return expected;
            break;
        }
        case var_read_exp: {
            ty_node * type = S_look(global_types, S_Symbol(e->data.sval));
            if(type == NULL) {
                type = S_look(f->args_locs_types, S_Symbol(e->data.sval));
            }

            assert(type != NULL);
            /*if (type == NULL) {
                type = S_look(f->args_locs_types, S_Symbol(e->data.var_ops.name));
            }
            assert(type);*/
            return type;
        }
        case funcall_exp: {
            fundec_node * fun = S_look(function_decs, S_Symbol(e->data.fun_ops.name));
            assert(fun != NULL) ;
            list * params = fun->params;
            list * args = e->data.fun_ops.args;
            while(params!=NULL) {
                if (args==NULL) {
                    fprintf(stderr,"Calling function %s with different number of arguments\n",fun->name);
                    exit(73);
                }
                param * p = params->head;
                exp_node * arg = args->head;
                ty_node * expected = p->ty;
                ty_node * actual = typeCheckExpr(arg,global_types,function_decs,f);
                if(actual != expected) {
                    fprintf(stderr,"Calling function %s with incompatible type  arguments for argument name %s. Expected : %s , actual:%s\n",fun->name,p->name, typeToStr(expected),typeToStr(actual));
                    exit(73);
                }

                params= params->next;
                args=args->next;

            }
            if(args!=NULL) {
                fprintf(stderr,"Calling function %s with different number of arguments\n",fun->name);
                exit(73);
            }


            /**/
            return fun->ret;

        }
        case array_exp: {
            ty_node * ret = S_look(global_types,S_Symbol(e->data.array_ops.name));
            if(f!=NULL) {
                ret = S_look(f->args_locs_types,S_Symbol(e->data.var_ops.name));
            }
            ty_node * expected_ind_type = IntTyNode();
            ty_node * actual_ind_type = typeCheckExpr(e->data.array_ops.e,global_types,function_decs,f);
            if(expected_ind_type!=actual_ind_type) {
                fprintf(stderr, "Wrong data type for array index. Expected %s got %s", typeToStr(expected_ind_type), typeToStr(actual_ind_type));
                exit(73);
            }

            return ret;
        }
        // TODO add more cases for all expressions
        default:
            assert(0); // Should be dead code
    }
}

void typeCheckStmts(list * l, S_table locals, S_table function_decs, frame * f);

void typeCheckStmt(stmt_node * s, S_table global_types, S_table function_decs, frame * f) {
    if(!s) return;
    switch(s->kind){
        case if_stmt: {
            ty_node * actual = typeCheckExpr(s->data.if_ops.cond,global_types,function_decs,f);
            ty_node * expected = IntTyNode();

            if(expected != actual) {
                fprintf(stderr,"Wrong data type for if condtion. Expected %s got %s",typeToStr(expected),typeToStr(actual));
                exit(73);
            }
            typeCheckStmts(s->data.if_ops.then_stmts,global_types,function_decs,f);
            typeCheckStmts(s->data.if_ops.else_stmts,global_types,function_decs,f);
            break;
            }
        case while_stmt: {
            ty_node * actual = typeCheckExpr(s->data.while_ops.guard,global_types,function_decs,f);
            ty_node * expected = IntTyNode();

            if(expected != actual) {
                fprintf(stderr,"Wrong data type for while guard. Expected %s got %s",typeToStr(expected),typeToStr(actual));
                exit(73);
            }
            typeCheckStmts(s->data.while_ops.body,global_types,function_decs,f);
            typeCheckStmts(s->data.while_ops.otherwise,global_types,function_decs,f);
            break;
        }
        case repeat_stmt: {

            ty_node * actual = typeCheckExpr(s->data.repeat_ops.count,global_types,function_decs,f);
            ty_node * expected = IntTyNode();

            if(expected != actual) {
                fprintf(stderr,"Wrong data type for repeat guard. Expected %s got %s",typeToStr(expected),typeToStr(actual));
                exit(73);
            }
            typeCheckStmts(s->data.repeat_ops.body,global_types,function_decs,f);

            break;
        }
        case ret_stmt: {
            //Assume that returns only take integers
            ty_node * expected ;
            if(f != NULL) {
                expected = f->ret;
            }
           else {
                expected = IntTyNode(); // TODO check if we are inside a function, get the return type of the function
            }
            ty_node * actual;

            if (s->data.ret_exp == NULL) {
                actual = VoidTyNode();
            } else {
                actual = typeCheckExpr(s->data.ret_exp, global_types, function_decs, f);
            }

            if(expected != actual) {
                fprintf(stderr,"Wrong data type for return. Expected %s got %s",typeToStr(expected),typeToStr(actual));
                exit(73);
            }
            break ;


            //TODO support functions different values
        }
        case assign_stmt: {
            ty_node * expected = S_look(global_types, S_Symbol(s->data.assign_ops.var_name));

            if(expected == NULL && f!= NULL) {
                 expected = S_look(f->args_locs_types, S_Symbol(s->data.assign_ops.var_name));
            }

            assert(expected != NULL);
            /*if (expected == NULL) {
                expected = S_look(f->args_locs_types, S_Symbol(s->data.assign_ops.init));
            }
            assert(expected);*/

            ty_node * actual = typeCheckExpr(s->data.assign_ops.init, global_types, function_decs, f);
            if(expected != actual) {
                fprintf(stderr,"Wrong data type for writing to variable %s.  Expected %s, but got %s\n",s->data.assign_ops.var_name, typeToStr(expected), typeToStr(actual));
                exit(73);
            }
            break ;

        }
        case funcall_stmt: {
            fundec_node * fun= (fundec_node*) S_look(function_decs, S_Symbol(s->data.fun_ops.name));
            assert(fun!=NULL);
            //to support arguments
            list * params = fun->params;
            list * args = s->data.fun_ops.args;
            while(params!=NULL) {
                if (args==NULL) {
                    fprintf(stderr,"Calling function %s with different number of arguments\n",fun->name);
                    exit(73);
                }
                param * p = params->head;
                exp_node * arg = args->head;
                ty_node * expected = p->ty;
                ty_node * actual = typeCheckExpr(arg,global_types,function_decs,f);
                if(actual != expected) {
                    fprintf(stderr,"Calling function %s with incompatible type  arguments for argument name %s. Expected : %s , actual:%s\n",fun->name,p->name, typeToStr(expected),typeToStr(actual));
                    exit(73);
                }

                params= params->next;
                args=args->next;

            }
            break;

        }
        case array_stmt: {
            ty_node * expected_ind = IntTyNode();
            ty_node * actual_ind = typeCheckExpr(s->data.array_ops.e,global_types,function_decs,f);
            if(expected_ind != actual_ind) {
                fprintf(stderr, "Wrong data type for array index. Expected %s got %s", typeToStr(expected_ind), typeToStr(actual_ind));
                exit(73);
            }
            ty_node * expected = s->data.array_ops.type;
            ty_node * actual = typeCheckExpr(s->data.array_ops.init, global_types, function_decs, f);
            if(actual!=expected)
            {
                fprintf(stderr, "Wrong data type for writing to array. Expected %s got %s", typeToStr(expected), typeToStr(actual));
                exit(73);
            }

            break;
        }


        // TODO add more cases for all expressions
        default:
            assert(0); // Should be dead code
    }
}

void typeCheckStmts(list * l, S_table globals_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    typeCheckStmt(l->head, globals_types, function_decs, f);
    typeCheckStmts(l->next, globals_types, function_decs, f);
}

void typeCheckVariable(vardec_node * vardec, S_table globals_types, S_table function_decs, frame * f) {
    // ensure that variable initialization expression has the same type as the variable definition

    ty_node * expected = vardec->type;
    ty_node * actual = typeCheckExpr(vardec->init,globals_types, function_decs,f);
    if(expected != actual) {
        fprintf(stderr,"Wrong data type for initializing variable %s. Expected %s got %s",vardec->name, typeToStr(expected),typeToStr(actual));
        exit(73);
    }
    return;

}

void typeCheckVariables(list * l, S_table global_types, S_table function_decs, frame * f) {
    if (l == NULL) return;
    typeCheckVariable((vardec_node *)l->head, global_types, function_decs, f);
    typeCheckVariables(l->next, global_types, function_decs, f);
}

void typeCheckFunction(fundec_node * fundec, S_table globals_types, S_table functions_rets, frame * f) {
    // TODO ensure that the body of the function is well typed
    // TODO ensure local variables are initialized with the correct type

    {
        list * l = fundec->locals;
        while(l!=NULL) {
            vardec_node * v = l->head;
            ty_node * expected = v->type;
            ty_node * actual = typeCheckExpr(v->init, globals_types,functions_rets,f);
            if(expected != actual) {
                fprintf(stderr,"Wrong type for local variable %s initialization.Expected %s got %s.\n", v->name, typeToStr(expected),typeToStr(actual));
                exit(73);
            }



            l=l->next;
        }
    }
    /*typeCheckVariables(fundec->locals, globals_types, functions_rets, f);*/
    /*f->ret = fundec->ret;*/
    typeCheckStmts(fundec->body, globals_types, functions_rets, f);
    return;


}

void typeCheckFunctions(list * l, S_table global_types, S_table function_decs, S_table frames) {
    if (l == NULL) return;
    fundec_node * fundec = (fundec_node*) l->head;
    frame * f = S_look(frames, S_Symbol(fundec->name));
    assert(f);
    typeCheckFunction(fundec, global_types, function_decs, f);
    typeCheckFunctions(l->next, global_types, function_decs, frames);
}

void typeCheck(program * p, S_table global_types, S_table function_decs, S_table frames) {
    typeCheckVariables(p->variables, global_types, function_decs, NULL);
    typeCheckFunctions(p->functions, global_types, function_decs, frames);
    typeCheckStmts(p->statements, global_types, function_decs, NULL);
}
