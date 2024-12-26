%{

#include <assert.h>
#include <string.h>
#include "ast.h"
#include "y.tab.h"


/* keep track of current character number */
static int col = 1;
/* keep track of current line number */
static int line = 1;
static int varCount = 0;
/*static int blockd = 0;
static int indentl = 0;
static int curr_indent =0;*/

/*New Code*/
static int block_lvl = 0;
static int indentation_lvl =0;
static int space = 0;

#define MAX_BLOCK_SIZE 20
static int block_lvl_stack[MAX_BLOCK_SIZE] ={0};
static int current_block = 0;

static int block();

#define MAX_STR_SIZE 1024
static char string_buffer[MAX_STR_SIZE] = "\0";
static int partial_string_size = 0;
static void start_partial_string(){
	//yylval.col=col;
    //yylval.line =line;
    //yylval.blockdepth=block_lvl;
    //yylval.indentlevel = indentation_lvl;
  	partial_string_size =0;


}
static void add_partial_string(char c){

    string_buffer[partial_string_size]=c;
    partial_string_size+=1;



}
static char *  complete_partial_string(){

	string_buffer[partial_string_size] = '\0';
    return string_buffer;
}



%}


%option noyywrap
%x BLOCK
%x STARTCOM
%x STR



%%

^.*             {BEGIN(BLOCK);yyless(0);space=0;}
<BLOCK>{
" "    {space+=1;col+=1;}
"\t"   {space+=4;col+=1;}
"\n"  {yyless(0);BEGIN(INITIAL);}
.      {int tok = block(); yyless(0);if(tok>0) return tok;}
}



"return"                {/*yylval.blockdepth =block_lvl;yylval.indentlevel = indentation_lvl; yylval.col = col;yylval.line = line;*/ col+=yyleng; return RETURN; }




0x[0-9A-F]+          {col+=yyleng; yylval.integerConstant = strtol(yytext,NULL,16);return NUMBER;}
0[0-7]+             {col+=yyleng; yylval.integerConstant = strtol(yytext,NULL,8);return NUMBER;}
[0-9]+            {col+=yyleng; yylval.integerConstant = atoi(yytext);return NUMBER;}

[1-9][0-9]*[a-zA-Z]   { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }



0x([0-9A-Fa-f]+)[g-zG-Z] { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }

0x[g-zG-Z]+                 { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }




#.*$               /*ignore*/{}

";"                      {col+=yyleng; return SEMICOLON;}
":="                      {col+=yyleng; return ASSIGN ;}


"+"                      {col+=yyleng; return PLUS;}

"-"                      {col+=yyleng; return MINUS;}

"*"                      {col+=yyleng; return MUL;}

"/"                      {col+=yyleng; return DIV;}

"%"                      {col+=yyleng; return REM;}

"=="                      {col+=yyleng; return EQ;}

"<>"                      {col+=yyleng; return NE;}

"<"                      {col+=yyleng; return LT;}

"<="                      {col+=yyleng; return LE;}

">"                      {col+=yyleng; return GT;}

">="                      {col+=yyleng; return GE;}

"!"                      {col+=yyleng; return NOT;}

"&"                      {col+=yyleng; return BAND;}

"|"                      {col+=yyleng; return BOR;}

"^"                      {col+=yyleng; return XOR;}

"&&"                      {col+=yyleng; return AND;}

"||"                      {col+=yyleng; return OR;}

","                        {return COMMA;}

"("                      {col+=yyleng; return LEFTPARA;}

")"                      {col+=yyleng; return RIGHTPARA;}

"{"                      {col+=yyleng; return LEFTCB;}

"}"                      {col+=yyleng; return RIGHTCB;}

"["                      {col+=yyleng; return LEFTBRACKET;}

"]"                      {col+=yyleng; return RIGHTBRACKET;}

"while"                  {col+=yyleng; return WHILE;}

"if"            {col+=yyleng; return IF;}

"else"                     {col+=yyleng; return ELSE;}

"otherwise"                     {col+=yyleng; return OTHERWISE;}

"repeat"                     {col+=yyleng; return REPEAT;}

"fun"                          {col+=yyleng; return FUN;}

"implicit"        {col+=yyleng; return IMPLICIT;}

"var"                    {col+=yyleng; return DECLAREVAR;}



"int"       {col+=yyleng; yylval.type = "int";  return TYPE;}
"void"      {col+=yyleng; yylval.type = "void";  return TYPE;}
"string"   {col+=yyleng; yylval.type = "string";  return TYPE;}


[a-zA-Z][a-zA-Z0-9_]*   {col+=yyleng; yylval.name = strdup(yytext);return NAMES;}


"/*"                    { BEGIN STARTCOM;  }

<STARTCOM>.             { }
<STARTCOM>\n            { /*  yylval.line  = line; */ line++;}
<STARTCOM>"*/"          { BEGIN(INITIAL); }
<STARTCOM><<EOF>>        { fprintf(stderr,"Incomplete comment at line %d col %d: %s\n", line, col, yytext); exit(73); }


" "                     { col+= yyleng;}
"\t"                     { col+= yyleng;}
"\n"                        {col=1;line+=1;}


\"                     {start_partial_string(); col+=1; BEGIN(STR);}
<STR>{
  \"           {col+=1;yylval.str=strdup(complete_partial_string());BEGIN(INITIAL);return STRINGS;}
  `n            {col+=2; add_partial_string('\n');}
  `t             {col+=2;add_partial_string('\t');}
  ``             {col+=2;add_partial_string('`');}
  "`\""          {col+=2;add_partial_string('"');}
  "\n"               { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }
  `.                 { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }

  .              {col+=1;add_partial_string(yytext[0]);}
}



.                       { fprintf(stderr,"illegal token at line %d col %d: %s\n", line, col, yytext); exit(73); }

%%

static int block(){
    if(space==block_lvl_stack[current_block]){
      //we are in the same block, nothing changes
      BEGIN(INITIAL);
      return -1;
    }
    else if(space > block_lvl_stack[current_block]){
      //we are in a deeper block than before, since we have more spaces
      //PUSH TO STACK
        current_block +=1;
        assert(current_block < MAX_BLOCK_SIZE);
        block_lvl_stack[current_block] = space;
        block_lvl = current_block;
        indentation_lvl = space;
        BEGIN(INITIAL);
        return START_BLOCK;

    }
    else if(space < block_lvl_stack[current_block]){
      //we are back in the old block , since we have less spaces
      //POP FROM STACK and check either its new block with lower indentation or its a previous block

      current_block -=1;
      assert(current_block >= 0);
      block_lvl = current_block;
      indentation_lvl = space;
      if(space==block_lvl_stack[current_block]){
        BEGIN(INITIAL);}
      //Assert fails if we close more than one block at once
      return END_BLOCK;
      //block();

    }
    else{
      //should be a dead code
      assert(0);
    }
}