lexer: driver.o tiger.l.o util.o
	gcc -g -o $@ $^

driver.o: driver.c token.h util.h
	gcc -g -c $<

util.o: util.c util.h
	gcc -g -c $<

tiger.l.o: tiger.l.c token.h
	gcc -g -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

tiger.l:
	touch $@

clean:
	rm -f *.o tiger.l.c lexer
