#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STACK_C__
#include "stack.h"
#undef __STACK_C__

frame *stack = NULL;

frame *pushs(char *s) {
	frame *f = malloc(sizeof(frame));
	if (f) {
		f->tp = F_STR;
		f->s = malloc(strlen(s) + 1);
		if (!s) {
			free(f);
			return NULL;
		}
		strcpy(f->s, s);
		f->down = stack;
		stack = f;
	}
	return f;
}

frame *pushi(int i) {
	frame *f = malloc(sizeof(frame));
	if (f) {
		f->tp = F_INT;
		f->s = NULL;
		f->i = i;
		f->down = stack;
		stack = f;
	}
	return f;
}

void pushf(frame *f) {
	f->down = stack;
	stack = f;
}

void pop() {
	if (stack) {
		frame *f = stack;
		stack = stack->down;
		if (f->tp == F_STR) {
			free(f->s);
		}
		free(f);
	}
}
