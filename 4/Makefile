parser: driver.o tiger.y.o tiger.l.o lexer.o
	gcc -g -o $@ $^

driver.o: driver.c tiger.y.h lexer.h
	gcc -g -c $<

tiger.y.o: tiger.y.c
	gcc -g -c $<

tiger.y.c tiger.y.h: tiger.y
	bison -v --header=$<.h -o $<.c $<

tiger.l.o: tiger.l.c tiger.y.h lexer.h
	gcc -g -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

lexer.o: lexer.c lexer.h tiger.y.h
	gcc -g -c $<

clean:
	rm -f *.o *.l.c *.y.h *.y.c parser
