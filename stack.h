#ifndef __STACK_H__
#define __STACK_H__

#include "value.h"

typedef struct frame {
	value *       v;
	struct frame *down;
} frame;

#ifndef __STACK_C__
extern frame *stack;
#endif

#define STACK_HAS_1_ANY \
	if (!stack) return E_UNDERFLOW;

#define STACK_HAS_1(typ) \
	if (!stack) return E_UNDERFLOW; \
	if (stack->v->tp != (typ)) return E_TYPE;

#define STACK_HAS_2_ANY \
	if (!stack || !stack->down) return E_UNDERFLOW;

#define STACK_HAS_2(tp1, tp2) \
	if (!stack || !stack->down) return E_UNDERFLOW; \
	if (stack->v->tp != (tp1)) return E_TYPE; \
	if (stack->down->v->tp != (tp2)) return E_TYPE;

void push(value *); // retains
void pushstr(char *);
void pushint(int);
void pushvar(size_t);
void pushref(void *, destructor *);

value *pop(); // does not release
int    popint();
size_t popvar();

#endif
