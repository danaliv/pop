#ifndef __STACK_H__
#define __STACK_H__

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

#ifndef __STACK_C__
extern frame *stack;
#endif

frame *pushs(char *);
frame *pushi(int);
frame *pushref(size_t);
void   pop();

frame *copyframe(frame *);
void   freeframe(frame *);

#endif
