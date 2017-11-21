#ifndef __POP_EXT_H__
#define __POP_EXT_H__

#include <stddef.h>

enum {
	E_OK = 0,
	E_UNDERFLOW,
	E_TYPE,
	E_DIV0,
	E_RANGE,
};

typedef struct frame {
	enum {
		F_STR,
		F_INT,
		F_REF,
	} tp;
	char *        s;
	int           i;
	size_t        ref;
	struct frame *down;
} frame;

extern frame *stack;

#define STACK_HAS_1_ANY \
	if (!stack) return E_UNDERFLOW;

#define STACK_HAS_1(typ) \
	if (!stack) return E_UNDERFLOW; \
	if (stack->tp != (typ)) return E_TYPE;

#define STACK_HAS_2_ANY \
	if (!stack || !stack->down) return E_UNDERFLOW;

#define STACK_HAS_2(tp1, tp2) \
	if (!stack || !stack->down) return E_UNDERFLOW; \
	if (stack->tp != (tp1)) return E_TYPE; \
	if (stack->down->tp != (tp2)) return E_TYPE;

extern frame *pushs(char *);
extern frame *pushi(int);
extern void   pop();

#endif
