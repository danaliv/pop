#include <stdbool.h>
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

static void destroyary(vecbk *valuesbk) {
	value **values = *(valuesbk->itemsp);
	for (size_t i = 0; i < valuesbk->len; i++) {
		release(values[i]);
	}
	vfree(valuesbk);
}

value *newary() {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TARY;
	v->onfree = (destructor *) destroyary;
	v->refs = 1;
	v->val.p = newvec(16, sizeof(value *), &v->p2);
	return v;
}

value *newmrk() {
	value *v = xcalloc(1, sizeof(value));
	v->tp = TMRK;
	v->refs = 1;
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

size_t arylen(value *v) {
	return ((vecbk *) v->val.p)->len;
}

value *aryget(value *v, size_t i) {
	if (i >= arylen(v)) return NULL;
	return ((value **) v->p2)[i];
}

void aryadd(value *ary, value *v) {
	vadd(ary->val.p);
	((value **) ary->p2)[arylen(ary) - 1] = retain(v);
}

char *vtos(value *v) {
	static char s[80];
	switch (v->tp) {
	case TSTR:
		return STR(v);
	case TINT:
		sprintf(s, "%d", INT(v));
		break;
	case TOPT:
		if (OPT(v)) {
			return vtos(OPT(v));
		} else {
			return NULL;
		}
	default:
		return NULL;
	}
	return s;
}

char *inspectstr(char *s) {
	size_t len = 2;
	size_t i, j;

	for (i = 0; s[i]; i++) {
		switch (s[i]) {
		case '"':
		case '\n':
		case '\\':
			len++;
		default:
			len++;
		}
	}

	char *s2 = xmalloc(len + 1);
	*s2 = '"';

	for (i = 0, j = 1; s[i]; i++, j++) {
		switch (s[i]) {
		case '\n':
			s2[j++] = '\\';
			s2[j] = 'n';
			break;
		case '"':
		case '\\':
			s2[j++] = '\\';
		default:
			s2[j] = s[i];
		}
	}
	s2[j++] = '"';
	s2[j] = '\0';

	return s2;
}

char *inspect(value *v, char **vars) {
	char * s;
	size_t i;

	switch (v->tp) {
	case TSTR:
		s = inspectstr(STR(v));
		break;
	case TINT:
		s = xmalloc(64);
		sprintf(s, "%d", INT(v));
		break;
	case TVAR:
		s = xmalloc(5 + strlen(vars[VAR(v)]));
		sprintf(s, "VAR#%s", vars[VAR(v)]);
		break;
	case TREF:
		s = xmalloc(64);
		sprintf(s, "REF#%lx", (uintptr_t) REF(v));
		break;
	case TOPT:
		if (OPT(v)) {
			char *s2 = inspect(OPT(v), vars);
			s = xmalloc(5 + strlen(s2));
			sprintf(s, "OPT#%s", s2);
			free(s2);
		} else {
			s = xmalloc(9);
			strcpy(s, "OPT#none");
		}
		break;
	case TARY:
		s = xmalloc(3);
		strcpy(s, "[");
		for (i = 0; i < arylen(v); i++) {
			char *s2 = inspect(aryget(v, i), vars);
			s = xrealloc(s, strlen(s) + strlen(s2) + 3);
			strcat(s, s2);
			strcat(s, ", ");
			free(s2);
		}
		if (i) s[strlen(s) - 2] = 0;
		strcat(s, "]");
		break;
	case TMRK:
		s = xstrdup("{");
		break;
	default:
		s = NULL;
	}

	return s;
}
