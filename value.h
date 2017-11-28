#ifndef __VALUE_H__
#define __VALUE_H__

#include <stddef.h>

typedef enum {
	TSTR,
	TINT,
	TVAR,
	TREF,
	TOPT,
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
#define OPT(v) ((value *) (v)->val.p)

value *newstr(char *);
value *newint(int);
value *newvar(size_t);
value *newref(void *, destructor *);
value *newopt(value *);

value *retain(value *);
void   release(value *);

char *vtos(value *);

#endif
