CC      = g++
CFLAGS  = -std=c++17 -Wall -O2

OBJ     = main.o lexer.o
DEPS    = token.h lexer.h

lexer.o : lexer.cpp $(DEPS)
main.o  : main.cpp  $(DEPS)

compiler: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f *.o compiler
