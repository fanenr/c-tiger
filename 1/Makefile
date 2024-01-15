test: test.o impl.o
	gcc -g -o $@ $^

test.o: test.c types.h
	gcc -g -c $<

impl.o:	impl.c types.h
	gcc -g -c $<
