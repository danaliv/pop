#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifdef MTRACE
#define xmalloc malloc
#define xcalloc calloc
#define xrealloc realloc
#define xstrdup strdup
#else
#define xmalloc _xmalloc
#define xcalloc _xcalloc
#define xrealloc _xrealloc
#define xstrdup _xstrdup
#endif

void *_xmalloc(size_t);
void *_xcalloc(size_t, size_t);
void *_xrealloc(void *, size_t);
char *_xstrdup(char *);

typedef struct {
	size_t len, capacity, itemsize;
	void **itemsp;
} vecbk;

vecbk *newvec(size_t capacity, size_t itemsize, void **itemsp);
void   vfree(vecbk *);

#define vadd(v) vaddn(v, 1)
void *vaddn(vecbk *, size_t);

#endif
