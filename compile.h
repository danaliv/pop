#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "memory.h"

typedef struct {
	enum {
		CS_MAIN,
		CS_DEF_NAME,
		CS_DEF_BODY,
		CS_VAR_NAME,
	} state;
	bool incomment;

	vecbk *  mainv;
	uint8_t *main;

	vecbk *varsv;
	char **vars;

	vecbk *defsv;
	struct cunitdef {
		char *   name;
		vecbk *  bodyv;
		uint8_t *body;
	} * defs;

	vecbk *ifsv;
	struct cunitif {
		size_t ifi, elsei;
		bool   haselse;
	} * ifs;
} cunit;

enum {
	C_OK = 0,
	C_IN_DEF,
	C_UNK,
	C_NOT_IN_DEF,
	C_UNTERM_STR,
	C_STR_SPACE,
	C_STRAY_ELSE,
	C_STRAY_THEN,
	C_UNTERM_DEF,
	C_UNTERM_IF,
	C_UNTERM_COMMENT,
	C_UNTERM_VAR,
};

cunit *newcunit();
void   freecunit(cunit *);

int  compile(cunit *, char *s, size_t len);
int  closecunit(cunit *);
bool isrunnable(cunit *);

void pcerror(int);

#endif
