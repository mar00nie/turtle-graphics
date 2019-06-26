CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -Wfloat-equal -ansi
INCS = neillsdl2.h
VALGRIND = --error-exitcode=1 --quiet --leak-check=full
SOURCES =  neillsdl2.c 
LIBS =  `sdl2-config --libs` -lm
CC = gcc

# Working test files for both parser and interpreter
TURTLE1 = turtle1.no
TURTLE2 = turtle2.no
TURTLE3 = turtle3.no
TURTLE4 = turtle4.no
TURTLE5 = turtle5.no

# Working test files for the extension
TURTLE2_EXT = turtle2_ext.no
TURTLE3_EXT = turtle3_ext.no
TURTLE4_EXT = turtle4_ext.no

all: parse interp parsetest interptest extension

# Parser and Interpreter files
parse: parse.c testparse.c parse.h
	$(CC) parse.c testparse.c -o parse $(CFLAGS)

interp: interp.c testinterp.c interp.h $(SOURCES) $(INCS)
	$(CC) interp.c testinterp.c $(SOURCES) -o interp $(CFLAGS) $(LIBS)

# Parser and interpreter files containing the test functions
parsetest: parsetest.c testparse.c parse.h
	$(CC) parsetest.c testparse.c -o parsetest $(CFLAGS)

interptest: interptest.c testinterp.c interp.h $(SOURCES) $(INCS)
	$(CC) interptest.c testinterp.c $(SOURCES) -o interptest $(CFLAGS) $(LIBS)

# Extension files
extension: extension.c extension.h $(SOURCES) $(INCS)
	$(CC) extension.c $(SOURCES) -o extension $(CFLAGS) $(LIBS)

run: all
	./parse $(TURTLE1)
	./parse $(TURTLE2)
	./parse $(TURTLE3)
	./parse $(TURTLE4)
	./parse $(TURTLE5)
	./parsetest
	./interp $(TURTLE1)
	./interp $(TURTLE2)
	./interp $(TURTLE3)
	./interp $(TURTLE4)
	./interp $(TURTLE5)
	./interptest
	./extension $(TURTLE2_EXT)
	./extension $(TURTLE3_EXT)
	./extension $(TURTLE4_EXT)

clean:
	rm -f parse interp parsetest interptest extension

.PHONY: clean run all

