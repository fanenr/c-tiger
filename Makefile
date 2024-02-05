LDFLAGS = -g
NOWARN  = -Wno-unused-variable -Wno-unused-function
CFLAGS  = -Wall -Wextra $(NOWARN) -ggdb3 -std=gnu11

src := util.c sema.c lexer.c parser.c driver.c 
src += tiger.l.c tiger.y.c
obj := $(src:%.c=%.o)

.PHONY: all
all: tiger

tiger: $(obj)
	gcc $(LDFLAGS) -o $@ $^ 

include deps.mk

$(obj): %.o: %.c
	gcc $(CFLAGS) -c $<

tiger.l.c: tiger.l
	flex -o $@ $<

tiger.y.c tiger.y.h: tiger.y
	bison -v --header=$<.h -o $<.c $<

.PHONY: json
json:
	make clean
	bear -- make

.PHONY: clean
clean:
	rm -f *.o *.l.c *.y.h *.y.c *.y.output tiger
