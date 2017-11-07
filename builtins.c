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
