CFLAGS = -g -Wall -Wextra -Wpedantic
CLANG_FORMAT ?= clang-format

.PHONY: clean format test

objs=main.o stack.o compile.o exec.o builtins.o

pop: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o pop $(objs)

clean:
	rm -f pop *.o *.d

format:
	$(CLANG_FORMAT) -i *.c *.h

test: pop
	ruby tests/runner.rb

deps := $(objs:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(deps)
