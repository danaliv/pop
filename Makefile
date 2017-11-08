CFLAGS=-g -Wall -Wextra -Wpedantic

.PHONY: clean format

objs=main.o stack.o compile.o exec.o builtins.o

pop: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o pop $(objs)

clean:
	rm -f pop *.o

format:
	gindent -i4 -ts4 -l100 -br -ce -brs -brf -npcs -npsl *.h *.c
	mv *~ .bak/
