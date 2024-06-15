MODE = debug

include config.mk
export CFLAGS LDFLAGS

src := driver.c parser.c lexer.c mstr.c array.c rbtree.c \
       tiger.l.c tiger.y.c
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
json: clean
	bear -- make

.PHONY: clean
clean:
	-rm -f *.o *.l.c *.y.h *.y.c *.y.output tiger
