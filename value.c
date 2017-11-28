#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "value.h"

value *newstr(char *s) {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TSTR;
	v->onfree = free;
	v->refs = 1;
	v->val.p = xstrdup(s);
	return v;
}

value *newint(int i) {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TINT;
	v->refs = 1;
	v->val.i = i;
	return v;
}

value *newvar(size_t var) {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TVAR;
	v->refs = 1;
	v->val.u = var;
	return v;
}

value *newref(void *p, destructor *onfree) {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TREF;
	v->onfree = onfree;
	v->refs = 1;
	v->val.p = p;
	return v;
}

value *newopt(value *v1) {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TOPT;
	v->onfree = v1 ? (destructor *) release : NULL;
	v->refs = 1;
	v->val.p = v1 ? retain(v1) : NULL;
	return v;
}

value *retain(value *v) {
	v->refs++;
	return v;
}

void release(value *v) {
	if (--v->refs == 0) {
		if (v->onfree) v->onfree(v->val.p);
		free(v);
	}
}

char *vtos(value *v) {
	static char s[80];
	switch (v->tp) {
	case TSTR:
		return STR(v);
	case TINT:
		sprintf(s, "%d", INT(v));
		break;
	case TVAR:
		sprintf(s, "VAR#%lu", VAR(v));
		break;
	case TREF:
		sprintf(s, "REF#%lx", (uintptr_t) REF(v));
		break;
	case TOPT:
		if (v->val.p) {
			sprintf(s, "OPT#some");
		} else {
			strcpy(s, "OPT#none");
		}
		break;
	default:
		return NULL;
	}
	return s;
}
