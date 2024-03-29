CSTD   = -std=gnu11
CXXSTD = -std=c++17
WARN   = -Wall -Wextra -Werror
NOWARN = -Wno-unused-variable -Wno-unused-function

OPT_LEVEL    = -Og
DBG_LDFLAGS  = -g
DBG_CFLAGS   = -ggdb3

CFLAGS   = $(WARN) $(NOWARN) $(OPT_LEVEL) $(DBG_CFLAGS) $(LTO_CFLAGS) \
          $(CSTD) $(ASAN_CFLAGS)
LDFLAGS  = $(DBG_LDFLAGS) $(LTO_LDFLAGS) $(ASAN_LDFLAGS)

src := driver.c sema.c parser.c ast.c lexer.c \
       mstr.c array.c rbtree.c \
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
	rm -f *.o *.l.c *.y.h *.y.c *.y.output tiger
