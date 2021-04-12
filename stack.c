#include <stdlib.h>

#include "memory.h"
#define STACK_C
#include "stack.h"
#undef STACK_C

frame *stack = NULL;

void push(value *v) {
	frame *f = xmalloc(sizeof(frame));
	f->v = retain(v);
	f->down = stack;
	stack = f;
}

void pushstr(char *s) {
	value *v = newstr(s);
	push(v);
	release(v);
}

void pushint(int i) {
	value *v = newint(i);
	push(v);
	release(v);
}

void pushvar(size_t var) {
	value *v = newvar(var);
	push(v);
	release(v);
}

void pushref(void *ref, destructor *onfree) {
	value *v = newref(ref, onfree);
	push(v);
	release(v);
}

void pushopt(value *v1) {
	value *v = newopt(v1);
	push(v);
	release(v);
}

value *pop() {
	frame *f = stack;
	value *v = stack->v;
	stack = stack->down;
	free(f);
	return v;
}

int popint() {
	value *v = pop();
	int    i = INT(v);
	release(v);
	return i;
}

size_t popvar() {
	value *v = pop();
	size_t var = VAR(v);
	release(v);
	return var;
}
