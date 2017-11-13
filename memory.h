#ifndef __MEMORY_H__
#define __MEMORY_H__

// identical to malloc, calloc, and realloc, except that they exit(EX_OSERR) on failure instead of
// returning NULL.
void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void **, size_t);

#endif
