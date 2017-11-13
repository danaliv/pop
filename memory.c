#include <stdlib.h>
#include <sysexits.h>

#include "memory.h"

void *xmalloc(size_t size) {
	void *ptr = malloc(size);
	if (!ptr) {
		exit(EX_OSERR);
	}
	return ptr;
}

void *xcalloc(size_t count, size_t size) {
	void *ptr = calloc(count, size);
	if (!ptr) {
		exit(EX_OSERR);
	}
	return ptr;
}

void *xrealloc(void **ptr, size_t size) {
	*ptr = realloc(*ptr, size);
	if (!*ptr) {
		exit(EX_OSERR);
	}
	return *ptr;
}
