 #include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"

static ty_node int_ty_node    = { int_ty };
static ty_node str_ty_node    = { str_ty };
static ty_node void_ty_node = {void_ty};


ty_node* IntTyNode(){
    return &int_ty_node;

}
ty_node* StrTyNode(){
    return &str_ty_node;
}

ty_node* VoidTyNode(){
    return &void_ty_node;
}

// TODO add more types

ty_node* TyNode(char * type) {
    if (!strcmp(type,"int")) {
        return IntTyNode();
    }
    if(!strcmp(type,"string")) {
        return StrTyNode();
    }
    if (!strcmp(type, "void")) {
        return VoidTyNode();
    }

    // TODO add more types

    assert(0);
}



char* typeToStr(ty_node* type) {
    switch (type->kind) {
        case int_ty:
            return "int";
        // TODO add more types
        case str_ty:
                return "string";
        case void_ty:
                return "void";
        default:
            assert(0);
    }
}

param * Param(ty_node * type, char * name) {
    param * p = malloc(sizeof(param));
    p->name = name;
    p->ty = type;
    return p;
}

exp_node* IntNode(int val){
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = int_exp;
    node->data.ival = val;
    return node;
}
exp_node* StrNode(char * sval){
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = str_exp;
    node->data.sval = sval;
    return node;
}

exp_node* BinOpNode(binop operation, exp_node * left, exp_node * right) {
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = binop_exp;
    node->data.bin_ops.op = operation;
    node->data.bin_ops.e1 = left;
    node->data.bin_ops.e2 = right;
    return node;
}

exp_node* UnOpNode(unop operation, exp_node * expr) {
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = unop_exp;
    node->data.un_ops.op = operation;
    node->data.un_ops.e = expr;
    return node;
}

stmt_node* RetNode(exp_node* e) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = ret_stmt;
    node->data.ret_exp = e;
    return node;

}

stmt_node* IfNode(exp_node* cond, list * thenStmts, list * elseStmts) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = if_stmt;
    node->data.if_ops.cond = cond;
    node->data.if_ops.then_stmts = thenStmts;
    node->data.if_ops.else_stmts = elseStmts;
    return node;
}
stmt_node* WhileNode(exp_node* guard, list * body, list * otherwise) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = while_stmt;
    node->data.while_ops.guard = guard ;
    node->data.while_ops.body = body;
    node->data.while_ops.otherwise = otherwise;
    return node;
}
stmt_node* RepeatNode(exp_node* count, list * body) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = repeat_stmt;
    node->data.repeat_ops.count = count;
    node->data.repeat_ops.body = body;
    return node;
}

stmt_node* AssignNode(char* var_name, exp_node* init) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = assign_stmt;
    node->data.assign_ops.var_name = var_name;
    node->data.assign_ops.init  = init;
    return node;
}

stmt_node* FunCallStmtNode(char * name, list * args) {
    stmt_node* node = malloc(sizeof(stmt_node));
    node->kind = funcall_stmt;
    node->data.fun_ops.name = name;
    node->data.fun_ops.args = args;
    return node;
}

vardec_node * VarDecNode(char * name, ty_node * type, exp_node * init, int implicit) {
    vardec_node* node = malloc(sizeof(vardec_node));
    node->name = name;
    node->type = type;
    node->init = init;
    node->implicit = implicit;
    return node;
}

exp_node* VarReadNode(char* name)
{
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = var_read_exp;
    node->data.sval = name;
    return node;
}



fundec_node * FunDecNode(char * name, ty_node * ret,list * params,list * locals, list * body) {
    fundec_node * node = malloc(sizeof(fundec_node ));
    node->name = name;
    node->ret = ret;
    node->params = params;
    node->locals = locals;
    node->body = body;

    return node;
}

exp_node* FunCallNode(char * name, list * args) {
    exp_node* node = malloc(sizeof(exp_node));
    node->kind = funcall_exp;
    node->data.fun_ops.name = name;
    node->data.fun_ops.args = args;

    return node;
}

exp_node* ArrayNode(char * name,exp_node * expr) {
    exp_node * node = malloc(sizeof(exp_node));
    node->kind = array_exp;
    node->data.array_ops.name = name;
    node->data.array_ops.e = expr;
    return node;
}

stmt_node * ArrayStmtNode(char * name,exp_node * e,ty_node * type,exp_node * init) {
    stmt_node * node = malloc(sizeof(stmt_node));
    node->kind = array_stmt;
    node->data.array_ops.name = name;
    node -> data.array_ops.e = e;
    node ->data.array_ops.init = init;
    node ->data.array_ops.type = type;

    return node;
}

list* ListAddFirst(void* hd, list* lst){
    list* l = malloc(sizeof(list));
    l->head = hd;
    l->next = lst;
    return l;
}

list* ListAddLast(void* hd, list* lst){
    list* l = malloc(sizeof(list));
    l->head = hd;
    l->next = NULL;

    // Empty list
    if (lst == NULL) {
        return l;
    }

    // Find last
    list * last = lst;
    while (last->next != NULL) {
        last = last->next;
    }


    last->next = l;
    return lst;
}
