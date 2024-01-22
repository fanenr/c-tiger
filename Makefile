parser: driver.o tiger.y.o tiger.l.o parser.o lexer.o
	gcc -g -o $@ $^

driver.o: driver.c tiger.y.h lexer.h parser.h
	gcc -g -c $<

tiger.y.o: tiger.y.c parser.h
	gcc -g -c $<

# generate tiger.y.c and tiger.y.h
tiger.y.c tiger.y.h: tiger.y
	bison -v --header=$<.h -o $<.c $<

tiger.l.o: tiger.l.c tiger.y.h lexer.h
	gcc -g -c $<

# generate tiger.l.c
tiger.l.c: tiger.l
	flex -o $@ $<

parser.o: parser.c parser.h lexer.h ast.h
	gcc -g -c $<

lexer.o: lexer.c lexer.h tiger.y.h
	gcc -g -c $<

clean:
	rm -f *.o *.l.c *.y.h *.y.c parser
