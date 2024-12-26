#ifndef AST_H
#define AST_H

#include "symbol.h"

typedef struct list_ list;

struct list_ {
    void* head;
    struct list_ * next;
};

list* ListAddFirst(void* hd, list* tl);
list* ListAddLast(void* hd, list* tl);

typedef struct program_ {
    list * variables;
    list * functions;
    list * statements;
} program;

// Types

typedef struct ty_node {
    enum {
        int_ty,
        str_ty,
        void_ty
        // TODO Add more types if needed
    } kind;
} ty_node;

ty_node* IntTyNode();
ty_node* StrTyNode();
ty_node* VoidTyNode();
// TODO Add more types if needed
ty_node* TyNode(char * type);

char* typeToStr(ty_node* type);


// Expressions:

typedef enum {
    plus_op,
    sub_op,
    mul_op,
    div_op,
    mod_op,
    bor_op,
    band_op,
    or_op,
    and_op,
    lt_op,
    gt_op,
    le_op,
    ge_op,
    eq_op,
    ne_op,
    xor_op,
    // TODO Add more binary operators
} binop;

typedef enum { not_op } unop;

typedef struct exp_node {
    // This is a tagged union
    // TODO add more variants to support all types of expressions
    enum { int_exp, binop_exp, unop_exp,str_exp,var_read_exp,void_exp,funcall_exp,array_exp} kind;
    union {
        int ival;
        char * sval;
        struct { binop op; struct exp_node* e1; struct exp_node* e2; } bin_ops;

        struct { unop op; struct exp_node* e; } un_ops;
        struct { char* name; } var_ops;
        struct {char * name;list * args;} fun_ops;
        struct {char * name; struct exp_node * e;}  array_ops;


    } data;
} exp_node;

exp_node* IntNode(int val);
exp_node* StrNode(char * sval);
exp_node* BinOpNode(binop operation, exp_node * left, exp_node * right);
exp_node* UnOpNode(unop operation, exp_node * expr);
exp_node* FunCallNode(char * name, list * args);
exp_node* ArrayNode(char * name,exp_node * expr);

exp_node* VarReadNode(char * var_name);
// TODO add more constructor functions, one per variant in the tagged union above

// Statements:

typedef struct stmt_node {
    // This is a tagged union
    // TODO add more variants to support all types of statements
    enum { if_stmt,while_stmt,repeat_stmt, ret_stmt, assign_stmt,funcall_stmt,array_stmt} kind;
    union {
        struct{char* var_name; exp_node * init;} assign_ops;
        struct { exp_node* cond; struct list_* then_stmts; struct list_* else_stmts;} if_ops;
        struct { exp_node* guard; struct list_* body; struct list_* otherwise;} while_ops;
        struct{exp_node* count; struct list_* body;} repeat_ops;
        struct { char* name; list * args; } fun_ops;
        struct{char * name;exp_node * e; ty_node * type; exp_node * init;} array_ops;
        exp_node* ret_exp;
    } data;
} stmt_node;

stmt_node* RetNode(exp_node* e);
stmt_node* IfNode(exp_node* cond, list * thenStmts, list * elseStmts);
stmt_node* WhileNode(exp_node* guard, list * body , list * otherwise);
stmt_node* RepeatNode(exp_node* count,list * body);
stmt_node* AssignNode(char* var_name, exp_node * init);
stmt_node* FunCallStmtNode(char * name, list * args);
stmt_node * ArrayStmtNode(char * name,exp_node * e,ty_node * type,exp_node * init);
// TODO add more constructor functions, one per variant in the tagged union above

// Variable declaration:

typedef struct vardec_node {
    char * name;
    ty_node * type;
    exp_node * init;
    int implicit;
} vardec_node;

vardec_node * VarDecNode(char * name, ty_node * type, exp_node * init,int implicit);



// Function declaration:

typedef struct param {
    ty_node* ty;
    char* name;
} param ;

param * Param(ty_node * type, char * name);

typedef struct fundec_node {

    char * name;
    ty_node * ret;
    list * params;
    list * locals;
    list * body;
    // TODO add everything that a function declaration needs
} fundec_node;

fundec_node * FunDecNode(char * name, ty_node * ret,list * params,list * locals, list * body);
// TODO improve constructor with everything that a function declaration needs

typedef struct frame {
    S_table args_locs_types;
    S_table indexes;
    ty_node * ret;
} frame;

#endif
