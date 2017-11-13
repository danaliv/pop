#ifndef __MEMORY_H__
#define __MEMORY_H__

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);

typedef struct {
	size_t len, capacity, itemsize;
	void **itemsp;
} vecbk;

vecbk *newvec(size_t capacity, size_t itemsize, void **itemsp);
void   vfree(vecbk *);

#define vadd(v) vaddn(v, 1)
void vaddn(vecbk *, size_t);

#endif
