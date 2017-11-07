#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	bool indef;
	size_t mainlen;
	uint8_t *main;
	size_t ndefs;
	struct cunitdef {
		char *name;
		size_t bodylen;
		uint8_t *body;
	} defs[];
} cunit;

enum {
	C_OK = 0,
	C_IN_DEF,
	C_OOM,
	C_UNK,
	C_NOT_IN_DEF,
	C_UNTERM_STR,
	C_STR_SPACE
};

cunit *newcunit();
void freecunit(cunit *);

int addtoken(cunit **, char *tk, size_t len);
int compile(cunit **, char *s, size_t len);

void pcerror(int);

#endif
