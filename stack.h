#ifndef __STACK_H__
#define __STACK_H__

typedef struct frame {
	enum {
		F_STR,
		F_INT
	} tp;
	char *s;
	int i;
	struct frame *down;
} frame;

#ifndef __STACK_C__
extern frame *stack;
#endif

frame *pushs(char *s);
frame *pushi(int i);
void pushf(frame * f);
void pop();

#endif
