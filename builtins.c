#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "exec.h"
#include "stack.h"

#define STACK_HAS_1_POSITIVE_INT \
	STACK_HAS_1(TINT); \
	if (stack->v->val.i < 1) return E_RANGE

int builtin_pop() {
	STACK_HAS_1_ANY;
	release(pop());
	return E_OK;
}

int builtin_swap() {
	STACK_HAS_2_ANY;

	frame *f = stack;
	stack = stack->down;
	f->down = stack->down;
	stack->down = f;

	return E_OK;
}

int builtin_dup() {
	STACK_HAS_1_ANY;
	push(stack->v);
	return E_OK;
}

int builtin_rotate() {
	STACK_HAS_1_POSITIVE_INT;

	int n = popint();
	if (n == 1) return E_OK;
	if (n == 2) return builtin_swap();

	frame *f = stack;
	for (int i = 2; i < n; i++) {
		f = f->down;
		if (!f) return E_UNDERFLOW;
	}
	if (!f->down) return E_UNDERFLOW;

	frame *f2 = f->down;
	frame *f3 = f2->down;
	f->down = f3;
	f2->down = stack;
	stack = f2;

	return E_OK;
}

int builtin_rot() {
	pushint(3);
	return builtin_rotate();
}

int builtin_pick() {
	STACK_HAS_1_POSITIVE_INT;

	int n = popint();

	frame *f = stack;
	for (int i = 1; i < n; i++) {
		f = f->down;
		if (!f) return E_UNDERFLOW;
	}
	push(f->v);

	return E_OK;
}

int builtin_over() {
	pushint(2);
	return builtin_pick();
}

int builtin_getenv() {
	STACK_HAS_1(TSTR);

	value *v = pop();
	char * val = getenv(STR(v));
	release(v);

	if (val) {
		v = newstr(val);
		pushopt(v);
		release(v);
	} else {
		pushopt(NULL);
	}

	return E_OK;
}

int builtin_puts() {
	STACK_HAS_1_ANY;

	value *v = pop();
	char * s = vtos(v);
	if (s) printf("%s\n", s);
	release(v);

	return E_OK;
}

int builtin_add() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	pushint(n1 + n2);

	return E_OK;
}

int builtin_sub() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	pushint(n1 - n2);

	return E_OK;
}

int builtin_mul() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	pushint(n1 * n2);

	return E_OK;
}

int builtin_div() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	if (n2 == 0) return E_DIV0;
	pushint(n1 / n2);

	return E_OK;
}

int builtin_eq() {
	STACK_HAS_2_ANY;

	value *v2 = pop();
	value *v1 = pop();
	if (v1->tp == v2->tp) {
		int eq;
		switch (v1->tp) {
		case TSTR:
			eq = strcmp(STR(v1), STR(v2)) == 0;
			break;
		case TINT:
			eq = INT(v1) == INT(v2);
			break;
		case TVAR:
			eq = VAR(v1) == VAR(v2);
			break;
		case TREF:
			eq = REF(v1) == REF(v2);
			break;
		default:
			eq = 0;
		}
		pushint(eq ? -1 : 0);
	} else {
		pushint(0);
	}
	release(v1);
	release(v2);

	return E_OK;
}

int builtin_lt() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	pushint(n1 < n2);

	return E_OK;
}

int builtin_gt() {
	STACK_HAS_2(TINT, TINT);

	int n2 = popint();
	int n1 = popint();
	pushint(n1 > n2);

	return E_OK;
}

int builtin_not() {
	STACK_HAS_1(TINT);
	pushint(popint() ? 0 : -1);
	return E_OK;
}

int builtin_strlen() {
	STACK_HAS_1(TSTR);

	value *v = pop();
	pushint(strlen(STR(v)));
	release(v);

	return E_OK;
}

int builtin_strcat() {
	STACK_HAS_2(TSTR, TSTR);
	value *s2 = pop(), *s1 = pop();

	char s[strlen(STR(s1)) + strlen(STR(s2)) + 1];
	strcpy(s, STR(s1));
	strcat(s, STR(s2));
	pushstr(s);

	release(s1);
	release(s2);
	return E_OK;
}

int builtin_none() {
	pushopt(NULL);
	return E_OK;
}

int builtin_some() {
	STACK_HAS_1_ANY;

	value *v = pop();
	pushopt(v);
	release(v);

	return E_OK;
}

int builtin_DEBUG_puts_all() {
	while (stack) {
		builtin_puts();
	}
	return E_OK;
}
