CFLAGS=-g -Wall -Wextra -Wpedantic

.PHONY: clean format

pop: main.o stack.o compile.o exec.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o pop main.o stack.o compile.o exec.o

clean:
	rm -f pop *.o

format:
	gindent -i4 -ts4 -l100 -br -brs -brf -npcs -npsl *.h *.c
	mv *~ .bak/
