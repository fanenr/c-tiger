util.o: util.h
sema.o: sema.h parser.h
lexer.o: lexer.h tiger.y.h
driver.o: sema.h parser.h tiger.y.h
parser.o: parser.h lexer.h tiger.y.h
