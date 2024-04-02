lexer.o: ast.h lexer.h common.h tiger.y.h

parser.o: parser.h lexer.h tiger.y.h

driver.o: parser.h tiger.y.h

