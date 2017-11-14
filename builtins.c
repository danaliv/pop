#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "exec.h"
#include "stack.h"

int builtin_pop(void) {
	if (!stack) return E_EMPTY;
	pop();
	return E_OK;
}

int builtin_swap(void) {
	if (!stack || !stack->down) return E_TOOFEW;

	frame *f = stack;
	stack = stack->down;
	f->down = stack->down;
	stack->down = f;

	return E_OK;
}

int builtin_dup(void) {
	if (!stack) return E_EMPTY;

	switch (stack->tp) {
	case F_STR:
		pushs(stack->s);
		break;
	case F_INT:
		pushi(stack->i);
		break;
	case F_REF:
		pushref(stack->ref);
		break;
	}

	return E_OK;
}

int builtin_rot(void) {
	pushi(3);
	return builtin_rotate();
}

int builtin_rotate(void) {
	if (!stack) {
		return E_EMPTY;
	}
	if (stack->tp != F_INT) {
		return E_TYPE;
	}
	if (stack->i < 1) {
		return E_RANGE;
	}
	if (stack->i == 1) {
		pop();
		return E_OK;
	}

	frame *f = stack;
	for (int i = 1; i < stack->i; i++) {
		f = f->down;
		if (!f) {
			return E_TOOFEW;
		}
	}
	if (!f->down) {
		return E_TOOFEW;
	}

	frame *f2 = f->down;
	frame *f3 = f2->down;
	pop();
	f->down = f3;
	f2->down = stack;
	stack = f2;

	return E_OK;
}

int builtin_over(void) {
	pushi(2);
	return builtin_pick();
}

int builtin_pick(void) {
	if (!stack) {
		return E_EMPTY;
	}
	if (stack->tp != F_INT) {
		return E_TYPE;
	}
	if (stack->i < 1) {
		return E_RANGE;
	}

	frame *f = stack;
	for (int i = 0; i < stack->i; i++) {
		f = f->down;
		if (!f) {
			return E_TOOFEW;
		}
	}
	pop();

	switch (f->tp) {
	case F_STR:
		pushs(f->s);
		break;
	case F_INT:
		pushi(f->i);
		break;
	case F_REF:
		pushref(f->ref);
		break;
	}

	return E_OK;
}

int builtin_getenv(void) {
	if (!stack) {
		return E_EMPTY;
	}
	if (stack->tp != F_STR) {
		return E_TYPE;
	}

	char *val = getenv(stack->s);
	pop();
	pushs(val ? val : "");

	return E_OK;
}

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
	case F_REF:
		printf("VAR#%lu\n", stack->ref);
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
	pushi(a + b);

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
	pushi(b - a);

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
	pushi(a * b);

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
	pushi(b / a);

	return E_OK;
}

int builtin_eq(void) {
    if (!stack || !stack->down) return E_TOOFEW;

    if (stack->tp != stack->down->tp) {
        pop();
        pop();
        pushi(0);
        return E_OK;
    }

    int eq = 0;
    switch (stack->tp) {
    case F_STR:
        eq = strcmp(stack->s, stack->down->s) ? 0 : 1;
        break;
    case F_INT:
        eq = stack->i == stack->down->i ? 1 : 0;
        break;
    case F_REF:
        eq = stack->ref == stack->down->ref ? 1 : 0;
        break;
    }

    pop();
    pop();
    pushi(eq);

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
		case F_REF:
			printf("    tp = F_REF\n");
			break;
		}
		if (f->s) {
			printf("    s = 0x%016lx \"%s\"\n", (uintptr_t) f->s, f->s);
		} else {
			printf("    s = 0x0000000000000000\n");
		}
		printf("    i = %d\n", f->i);
		printf("    ref = VAR#%lu\n", f->ref);
		printf("    down = 0x%016lx\n", (uintptr_t) f->down);
		printf("}\n");
		f = f->down;
	}
	return E_OK;
}

int builtin_DEBUG_puts_all(void) {
	while (stack) {
		builtin_puts();
	}
	return E_OK;
}
