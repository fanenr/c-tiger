lexer: driver.o lex.yy.o util.o
	gcc -g -o $@ $^

driver.o: driver.c token.h util.h
	gcc -g -c $<

util.o: util.c util.h
	gcc -g -c $<

lex.yy.o: lex.yy.c token.h util.h
	gcc -g -c $<

lex.yy.c: tiger.lex
	flex $<

clean:
	rm -f *.o lex.yy.c lexer
