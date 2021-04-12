#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "memory.h"

static void oom() {
	fputs("Out of memory\n", stderr);
	exit(EX_OSERR);
}

void *xmalloc_(size_t size) {
	void *ptr = malloc(size);
	if (!ptr) oom();
	return ptr;
}

void *xcalloc_(size_t count, size_t size) {
	void *ptr = calloc(count, size);
	if (!ptr) oom();
	return ptr;
}

void *xrealloc_(void *ptr, size_t size) {
	ptr = realloc(ptr, size);
	if (!ptr) oom();
	return ptr;
}

char *xstrdup_(char *s) {
	char *s2 = strdup(s);
	if (!s2) oom();
	return s2;
}

vecbk *newvec(size_t capacity, size_t itemsize, void **itemsp) {
	vecbk *v = xmalloc(sizeof(vecbk));
	v->len = 0;
	v->capacity = capacity;
	v->itemsize = itemsize;
	v->itemsp = itemsp;
	*itemsp = xmalloc(capacity * itemsize);
	return v;
}

void vfree(vecbk *v) {
	free(*v->itemsp);
	free(v);
}

void *vaddn(vecbk *v, size_t count) {
	size_t needcap = v->len + count;

	if (v->capacity < needcap) {
		while (v->capacity < needcap) {
			v->capacity *= 2;
		}
		*v->itemsp = xrealloc(*v->itemsp, v->capacity * v->itemsize);
	}

	v->len += count;

	return *v->itemsp;
}
