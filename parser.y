%{

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

extern char * result;

int yylex(void); /* function prototype */

void yyerror(const char *s) {
    fprintf(stderr,"parsing error %s\n", s); exit(73);
}

//static int counter =0;
#define BUF_SIZE 100000

__attribute__((unused))
static char * buf_printf(const char * format, ...) {
   static char buf[BUF_SIZE];
   va_list argp;
   va_start(argp, format);
   vsnprintf(buf, BUF_SIZE, format, argp);
   va_end(argp);
   return strdup(buf);

}

extern program p;

%}


%union {

	int ival;
    int integerConstant;
    char *type;
    char *name ;
    char * str;

    stmt_node * stmt;
    list * stmts;
    exp_node * exp;
    vardec_node * var;
    list * vars;
    list * args;
    param * arg;
    list * callargs;
    /*char * callarg;
    char * callarglist;*/
    fundec_node * fun;
    list * funs;


    }

%define parse.error verbose

%token <ival> NUMBER
%token <type>  TYPE
%token <name>  NAMES
%token <str>  STRINGS

%nonassoc THEN
%nonassoc ELSE OTHERWISE

%token
  RETURN
  ZER0
  SEMICOLON
  LEFTPARA
  RIGHTPARA
  LEFTCB
  RIGHTCB
  LEFTBRACKET
  RIGHTBRACKET
  WHILE
  IF
  ELSE
  ASSIGN
  OTHERWISE
  REPEAT
  DECLAREVAR
  IMPLICIT
  FUN
  COMMA
  START_BLOCK
  END_BLOCK

%left OR
%left AND
%left BOR
%left XOR
%left BAND
%left NE EQ
%left LT LE GT GE
%left PLUS MINUS
%left MUL DIV REM
%right NOT


%type <stmt> stmt
%type <stmts> stmts
%type <stmts> block
%type <exp> exp
%type <var> var
%type <vars> vars
%type <var> localvar
%type <vars> localvars
%type <funs> funs
%type <fun> fun
%type <arg> arg
%type <args> args
%type <args> arglist
%type <callargs> callargs
%type <callargs> callarglist




%start program


%%

program:vars funs stmts                                                      {p.variables=$1; p.functions=$2; p.statements=$3;  }

funs: /*empty*/                                                                {$$=NULL;}
|funs fun                                                                     {$$=ListAddLast($2,$1);}

fun: FUN NAMES TYPE LEFTPARA args RIGHTPARA START_BLOCK localvars stmts END_BLOCK       {$$=FunDecNode($2,TyNode($3),$5,$8,$9);}

args: /*empty*/                                                           {$$=NULL; }
       |arglist                                                           { $$ = $1; }

arglist:arg                                                              {$$= ListAddFirst($1,NULL); }
       |arglist COMMA arg                                                {$$=ListAddLast($3,$1); }

arg: NAMES TYPE                                                           { $$ = Param(TyNode($2),$1);}

localvars: /*empty*/                                                      {$$=NULL;}
|localvars localvar                                                       {$$=ListAddLast($2,$1);}

localvar: DECLAREVAR NAMES TYPE ASSIGN exp SEMICOLON                      {$$=VarDecNode($2,TyNode($3),$5,0);}

vars: /*empty*/                                                           {$$=NULL;}
|vars var                                                                 {$$ = ListAddLast($2,$1);}

var: DECLAREVAR NAMES TYPE ASSIGN exp SEMICOLON                            {$$=VarDecNode($2,TyNode($3),$5,0);}
|IMPLICIT NAMES TYPE ASSIGN exp  SEMICOLON                                 {$$=VarDecNode($2,TyNode($3),$5,1);}



stmts: stmt                                                                {$$=ListAddFirst($1,NULL);}
      |stmts stmt                                                          {$$=ListAddLast($2,$1);}

block: stmt                                                                {$$=ListAddFirst($1,NULL);}
      | START_BLOCK stmts END_BLOCK                                        {$$=$2;}
      | START_BLOCK stmts YYEOF                                            {$$=$2;}


stmt: RETURN exp SEMICOLON                                                    {$$=RetNode($2);}
|RETURN SEMICOLON                                                             {$$ = RetNode(NULL);}
     | IF LEFTPARA exp RIGHTPARA block %prec THEN                             { $$=IfNode($3,$5,NULL);}
     | IF LEFTPARA exp RIGHTPARA block ELSE block                               { $$=IfNode($3,$5,$7);}
     | WHILE LEFTPARA exp RIGHTPARA block %prec THEN                          {$$=WhileNode($3,$5,NULL);}
     | WHILE LEFTPARA exp RIGHTPARA block OTHERWISE block                     {$$=WhileNode($3,$5,$7);}
     |REPEAT LEFTPARA exp RIGHTPARA block                                     {$$=RepeatNode($3,$5);}
     |NAMES ASSIGN exp SEMICOLON                                              {$$ = AssignNode($1,$3);}
     |NAMES LEFTPARA callargs RIGHTPARA SEMICOLON                             {$$ = FunCallStmtNode($1,$3);}
     | NAMES LEFTBRACKET exp RIGHTBRACKET TYPE ASSIGN exp SEMICOLON                {$$=ArrayStmtNode($1,$3,TyNode($5),$7);}



exp: NUMBER                                                          {$$= IntNode($1);}
|NAMES                                                               { $$ = VarReadNode($1); }
|NAMES LEFTPARA callargs RIGHTPARA                                   { $$ = FunCallNode($1,$3);}
|STRINGS                                                             { $$= StrNode($1);}
|exp PLUS exp                                                        { $$=BinOpNode(plus_op,$1,$3);}
|exp MINUS exp                                                       {$$=BinOpNode(sub_op,$1,$3);}
|exp MUL exp                                                         { $$=BinOpNode(mul_op,$1,$3);}
|exp DIV exp                                                         {$$=BinOpNode(div_op,$1,$3);}
|exp REM exp                                                         { $$=BinOpNode(mod_op,$1,$3);}
|exp BOR exp                                                         { $$=BinOpNode(bor_op,$1,$3);}
|exp BAND exp                                                        { $$=BinOpNode(band_op,$1,$3);}
|exp OR exp                                                          {$$=BinOpNode(or_op,$1,$3);}
|exp AND exp                                                         { $$=BinOpNode(and_op,$1,$3);}
|exp LT exp                                                          { $$=BinOpNode(lt_op,$1,$3);}
|exp GT exp                                                          { $$=BinOpNode(gt_op,$1,$3);}
|exp LE exp                                                          { $$=BinOpNode(le_op,$1,$3);}
|exp GE exp                                                          {$$=BinOpNode(ge_op,$1,$3);}
|exp EQ exp                                                          { $$=BinOpNode(eq_op,$1,$3);}
|exp NE exp                                                          { $$=BinOpNode(ne_op,$1,$3);}
|exp XOR exp                                                         { $$=BinOpNode(xor_op,$1,$3);}
|NOT exp                                                              {$$= UnOpNode(not_op,$2);}
|LEFTPARA exp RIGHTPARA                                              {$$=$2;}
|NAMES LEFTBRACKET exp RIGHTBRACKET                                  {$$=ArrayNode($1,$3);}

callargs: /*empty*/     {$$=NULL;}
       |callarglist    {$$ = $1;}

callarglist: exp {$$=ListAddFirst($1,NULL);}
       |callarglist COMMA exp  {$$ = ListAddLast($3,$1);}
