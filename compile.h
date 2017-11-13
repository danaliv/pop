#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	enum {
		CS_MAIN,
		CS_DEF_NAME,
		CS_DEF_BODY,
		CS_VAR_NAME,
	} state;
	bool     incomment;
	size_t   mainlen;
	uint8_t *main;
	size_t   nvars;
	char **  vars;
	size_t   ndefs;
	struct cunitdef {
		char *   name;
		size_t   bodylen;
		uint8_t *body;
	} defs[];
} cunit;

enum {
	C_OK = 0,
	C_IN_DEF,
	C_UNK,
	C_NOT_IN_DEF,
	C_UNTERM_STR,
	C_STR_SPACE,
};

cunit *newcunit();
void   freecunit(cunit *);

int addtoken(cunit **, char *tk, size_t len);
int compile(cunit **, char *s, size_t len);

void pcerror(int);

#endif
