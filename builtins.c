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

int builtin_add(void) {
    if (!stack || !stack->down) {
        return E_TOOFEW;
    }
    if (stack->tp != F_INT || stack->down->tp != F_INT) {
        return E_TYPE;
    }

    int a = stack->i;
    int b = stack->down->i;
    pop();
    pop();
    if (!pushi(a + b)) {
        return E_OOM;
    }

    return E_OK;
}

int builtin_sub(void) {
    if (!stack || !stack->down) {
        return E_TOOFEW;
    }
    if (stack->tp != F_INT || stack->down->tp != F_INT) {
        return E_TYPE;
    }

    int a = stack->i;
    int b = stack->down->i;
    pop();
    pop();
    if (!pushi(b - a)) {
        return E_OOM;
    }

    return E_OK;
}

int builtin_mul(void) {
    if (!stack || !stack->down) {
        return E_TOOFEW;
    }
    if (stack->tp != F_INT || stack->down->tp != F_INT) {
        return E_TYPE;
    }

    int a = stack->i;
    int b = stack->down->i;
    pop();
    pop();
    if (!pushi(a * b)) {
        return E_OOM;
    }

    return E_OK;
}

int builtin_div(void) {
    if (!stack || !stack->down) {
        return E_TOOFEW;
    }
    if (stack->tp != F_INT || stack->down->tp != F_INT) {
        return E_TYPE;
    }
    if (stack->i == 0) {
        return E_DIV0;
    }

    int a = stack->i;
    int b = stack->down->i;
    pop();
    pop();
    if (!pushi(b / a)) {
        return E_OOM;
    }

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
