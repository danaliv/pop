#include <stdio.h>

#include "builtins.h"
#include "stack.h"
#include "exec.h"

int builtin_puts(void) {
	if (!stack) {
		return E_EMPTY;
	}
	switch (stack->tp) {
	case F_STR:
		printf("%s\n", stack->s);
		break;
	case F_INT:
		printf("%d\n", stack->i);
		break;
	}
	pop();
	return E_OK;
}

int builtin_DEBUG_stack(void) {
	frame *f = stack;
	while (f) {
		printf("frame %016lx {\n", (uintptr_t) f);
		switch (f->tp) {
		case F_STR:
			printf("    tp = F_STR\n");
			break;
		case F_INT:
			printf("    tp = F_INT\n");
			break;
		}
		printf("    s = 0x%016lx \"%s\"\n", (uintptr_t) f->s, f->s);
		printf("    i = %d\n", f->i);
		printf("    down = 0x%016lx\n", (uintptr_t) f->down);
		printf("}\n");
		f = f->down;
	}
	return E_OK;
}
