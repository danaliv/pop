#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#define __STACK_C__
#include "stack.h"
#undef __STACK_C__

frame *stack = NULL;

frame *pushs(char *s) {
	frame *f = xcalloc(1, sizeof(frame));
	f->tp = F_STR;
	f->s = xstrdup(s);
	f->down = stack;
	stack = f;
	return f;
}

frame *pushi(int i) {
	frame *f = xcalloc(1, sizeof(frame));
	f->tp = F_INT;
	f->i = i;
	f->down = stack;
	stack = f;
	return f;
}

frame *pushref(size_t ref) {
	frame *f = xcalloc(1, sizeof(frame));
	f->tp = F_REF;
	f->ref = ref;
	f->down = stack;
	stack = f;
	return f;
}

frame *pushobj(void *obj, destructor *destruct) {
	frame *f = xcalloc(1, sizeof(frame));
	f->tp = F_OBJ;
	f->obj = xmalloc(sizeof(object));
	f->obj->obj = obj;
	f->obj->destruct = destruct;
	f->obj->refs = 1;
	f->down = stack;
	stack = f;
	return f;
}

frame *dupobj(object *obj) {
	frame *f = xcalloc(1, sizeof(frame));
	f->tp = F_OBJ;
	f->obj = obj;
	f->obj->refs++;
	f->down = stack;
	stack = f;
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
	frame *f2 = xmalloc(sizeof(frame));
	memcpy(f2, f1, sizeof(frame));
	switch (f1->tp) {
	case F_STR:
		f2->s = xstrdup(f1->s);
		break;
	case F_OBJ:
		f2->obj->refs++;
		break;
	default:
		// no special processing needed for other data types
		break;
	}
	return f2;
}

void freeframe(frame *f) {
	if (f->tp == F_STR) free(f->s);
	if (f->tp == F_OBJ && --f->obj->refs == 0) {
		f->obj->destruct(f->obj->obj);
		free(f->obj);
	}
	free(f);
}

void popall() {
	while (stack) pop();
}
