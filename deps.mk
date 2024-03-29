lexer.o: ast.h lexer.h common.h tiger.y.h

parser.o: parser.h lexer.h tiger.y.h

sema.o: sema.h parser.h

driver.o: sema.h parser.h tiger.y.h

