#ifndef POP_EXT_H
#define POP_EXT_H

#include <stddef.h>

enum {
	E_OK = 0,
	E_UNDERFLOW,
	E_TYPE,
	E_DIV0,
	E_RANGE,
};

typedef enum {
	TSTR,
	TINT,
	TVAR,
	TREF,
} valtype;

typedef void destructor(void *);

typedef struct {
	valtype     tp;
	destructor *onfree;
	size_t      refs;
	union {
		void * p;
		int    i;
		size_t u;
	} val;
} value;

#define STR(v) ((char *) (v)->val.p)
#define INT(v) ((v)->val.i)
#define VAR(v) ((v)->val.u)
#define REF(v) ((v)->val.p)

typedef struct frame {
	value *       v;
	struct frame *down;
} frame;

extern frame *stack;

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

extern void pushstr(char *);
extern void pushint(int);
extern void pushref(void *, destructor *);
extern void pushopt(value *);

extern value *pop();

extern value *newstr(char *);
extern value *newint(int);
extern value *newref(void *, destructor *);
extern value *newopt(value *);

extern void release(value *);

#define xmalloc xmalloc_
extern void *xmalloc_(size_t);

#endif // POP_EXT_H
