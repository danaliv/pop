#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#ifdef MTRACE
#define xmalloc malloc
#define xcalloc calloc
#define xrealloc realloc
#define xstrdup strdup
#else
#define xmalloc xmalloc_
#define xcalloc xcalloc_
#define xrealloc xrealloc_
#define xstrdup xstrdup_
#endif

void *xmalloc_(size_t);
void *xcalloc_(size_t, size_t);
void *xrealloc_(void *, size_t);
char *xstrdup_(char *);

typedef struct {
	size_t len, capacity, itemsize;
	void **itemsp;
} vecbk;

vecbk *newvec(size_t capacity, size_t itemsize, void **itemsp);
void   vfree(vecbk *);

#define vadd(v) vaddn(v, 1)
void *vaddn(vecbk *, size_t);

#endif // MEMORY_H
