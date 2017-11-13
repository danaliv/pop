#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STACK_C__
#include "stack.h"
#undef __STACK_C__

frame *stack = NULL;

frame *pushs(char *s) {
	frame *f = calloc(1, sizeof(frame));
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
	frame *f = calloc(1, sizeof(frame));
	if (f) {
		f->tp = F_INT;
		f->i = i;
		f->down = stack;
		stack = f;
	}
	return f;
}

frame *pushref(size_t ref) {
	frame *f = calloc(1, sizeof(frame));
	if (f) {
		f->tp = F_REF;
		f->ref = ref;
		f->down = stack;
		stack = f;
	}
	return f;
}

void pop() {
	if (stack) {
		frame *f = stack;
		stack = stack->down;
		freeframe(f);
	}
}

frame *copyframe(frame *f1) {
	frame *f2 = malloc(sizeof(frame));
	if (f2) {
		memcpy(f2, f1, sizeof(frame));
		if (f1->tp == F_STR) {
			f2->s = malloc(strlen(f1->s) + 1);
			if (!f2->s) {
				free(f2);
				return NULL;
			}
			strcpy(f2->s, f1->s);
		}
	}
	return f2;
}

void freeframe(frame *f) {
	if (f->tp == F_STR) {
		free(f->s);
	}
	free(f);
}
