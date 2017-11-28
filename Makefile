CFLAGS += -std=c99 -g -Wall -Wextra -Wpedantic
CLANG_FORMAT ?= clang-format

OS = $(shell uname -s)

ifeq ($(OS), Linux)
	CFLAGS += -fPIC -D_XOPEN_SOURCE=700
	LDFLAGS += -rdynamic
	LIBS += -ldl -lbsd
endif

.PHONY: clean format test

objs=main.o stack.o compile.o exec.o builtins.o memory.o link.o value.o

pop: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o pop $(objs) $(LIBS)

clean:
	rm -f pop *.o *.d

format:
	$(CLANG_FORMAT) -i *.c *.h ext/*/*.c include/pop/*.h

test: pop
	ruby tests/runner.rb

deps := $(objs:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(deps)
