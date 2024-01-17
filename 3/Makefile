lexer: driver.o tiger.lex.o util.o
	gcc -g -o $@ $^

driver.o: driver.c token.h util.h
	gcc -g -c $<

util.o: util.c util.h
	gcc -g -c $<

tiger.lex.o: tiger.lex.c token.h
	gcc -g -c $<

tiger.lex.c: tiger.lex
	flex -o $@ $<

clean:
	rm -f *.o tiger.lex.c lexer
