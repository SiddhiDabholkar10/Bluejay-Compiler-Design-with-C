SANITIZER=-O0 -fsanitize=address
FLAGS=-Wall -Wextra -Werror -std=c17 -Wpedantic -Wno-unused-parameter ${SANITIZER}

semtest: main.o lex.yy.o y.tab.o ast.o semantic_analysis_symbols.o semantic_analysis_types.o frames.o print_ast.o
	cc ${FLAGS} -g -o semtest main.o lex.yy.o y.tab.o symbol.o table.o ast.o semantic_analysis_types.o semantic_analysis_symbols.o frames.o print_ast.o

y.tab.o: y.tab.c y.tab.h
	cc ${FLAGS} -g -c y.tab.c

main.o: main.c y.tab.h semantic_analysis_symbols.o semantic_analysis_types.h
	cc ${FLAGS} -g -c main.c

ast.o: ast.c symbol.o
	cc ${FLAGS} -g -c ast.c

symbol.o: symbol.c table.o
	cc ${FLAGS} -g -c symbol.c

table.o: table.c
	cc ${FLAGS} -g -c table.c

semantic_analysis_types.o: semantic_analysis_types.c ast.o
	cc ${FLAGS} -g -c semantic_analysis_types.c

semantic_analysis_symbols.o: semantic_analysis_symbols.c ast.o
	cc ${FLAGS} -g -c semantic_analysis_symbols.c

frames.o: frames.c ast.o
	cc ${FLAGS} -g -c frames.c

print_ast.o: print_ast.c ast.o
	cc ${FLAGS} -g -c print_ast.c

lex.yy.o: lex.yy.c
	cc ${SANITIZER} -g -c lex.yy.c

lex.yy.c: lexer.lex y.tab.h
	lex lexer.lex

y.tab.c: parser.y ast.h ast.o
	 yacc -o y.tab.c -dv parser.y

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

all: semtest

clean: 
	rm -f semtest lex.yy.c y.tab.c y.tab.h y.output *.o
