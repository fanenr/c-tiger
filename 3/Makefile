driver: driver.o tiger.y.o tiger.l.o util.o
	gcc -g -o $@ $^

driver.o: driver.c token.h util.h tokname
	gcc -g -c $<

tiger.y.o: tiger.y.c
	gcc -g -c $<

tiger.y.c: tiger.y
	bison -d --defines=$<.h -o $<.c $<

tiger.y:
	touch $@

tiger.l.o: tiger.l.c token.h
	gcc -g -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

tiger.l:
	touch $@

util.o: util.c util.h
	gcc -g -c $<

clean:
	rm -f *.o *.l.c *.y.h *.y.c lexer
