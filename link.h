#ifndef LINK_H
#define LINK_H

#include <stdbool.h>

#include "exec.h"
#include "memory.h"

struct link {
	char *name;
	char *path;

	struct cunit *cu;
	exctx *       ctx;

	void *dl;
};

struct link *newlink(char *path, char *rel, char *name);
void         freelink(struct link *);

bool linkinv(char *, size_t, char **prefixp, char **namep);

int addlinkcall(struct link *, vecbk *, char *);

#endif // LINK_H
