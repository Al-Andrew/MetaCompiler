exe: folders lex yac
	gcc temp/lex.yy.c temp/y.tab.c -o build/exe


folders:
	-mkdir build 2> /dev/null
	-mkdir temp 2> /dev/null

yac:
	yacc -d parser.y 
	mv y.tab.* temp/

lex:
	flex lexer.l
	mv lex.yy.c temp/