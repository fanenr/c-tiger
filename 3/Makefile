parser: driver.o tiger.y.o tiger.l.o util.o
	gcc -g -o $@ $^

driver.o: driver.c tiger.y.h util.h 
	gcc -g -c $<

tiger.y.o: tiger.y.c
	gcc -g -c $<

tiger.y.c tiger.y.h: tiger.y
	bison -d --defines=$<.h -o $<.c $<

tiger.l.o: tiger.l.c tiger.y.h
	gcc -g -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

util.o: util.c util.h
	gcc -g -c $<

clean:
	rm -f *.o *.l.c *.y.h *.y.c parser
