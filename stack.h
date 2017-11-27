#ifndef __STACK_H__
#define __STACK_H__

typedef void destructor(void *);

typedef struct {
	void *      obj;
	destructor *destruct;
	size_t      refs;
} object;

typedef struct frame {
	enum {
		F_STR,
		F_INT,
		F_REF,
		F_OBJ,
	} tp;
	char *        s;
	int           i;
	size_t        ref;
	object *      obj;
	struct frame *down;
} frame;

#ifndef __STACK_C__
extern frame *stack;
#endif

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

frame *pushs(char *);
frame *pushi(int);
frame *pushref(size_t);
frame *pushobj(void *, destructor *);
frame *dupobj(object *);
void   pop();

frame *copyframe(frame *);
void   freeframe(frame *);

void popall();

#endif
