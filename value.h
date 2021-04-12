#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>

typedef enum {
	TSTR,
	TINT,
	TVAR,
	TREF,
	TOPT,
	TARY,
	TMRK,
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
	void *p2;
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
value *newary();
value *newmrk();

value *retain(value *);
void   release(value *);

size_t arylen(value *);
value *aryget(value *, size_t);
void   aryadd(value *ary, value *v);

char *vtos(value *);
char *inspect(value *, char **);

#endif // VALUE_H
