tiger: driver.o tiger.y.o tiger.l.o parser.o lexer.o util.o
	gcc -g -o $@ $^

driver.o: driver.c ast.h util.h parser.h lexer.h tiger.y.h
	gcc -g -c $<

tiger.y.o: tiger.y.c ast.h util.h parser.h lexer.h tiger.y.h
	gcc -g -c $<

tiger.y.c tiger.y.h: tiger.y
	bison -v --header=$<.h -o $<.c $<

tiger.l.o: tiger.l.c ast.h util.h parser.h lexer.h tiger.y.h
	gcc -g -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

parser.o: parser.c ast.h util.h parser.h lexer.h tiger.y.h
	gcc -g -c $<

lexer.o: lexer.c ast.h util.h parser.h lexer.h tiger.y.h
	gcc -g -c $<

util.o: util.c util.h
	gcc -g -c $<

clean:
	rm -f *.o *.l.c *.y.h *.y.c *.y.output tiger
