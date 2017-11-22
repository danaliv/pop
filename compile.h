#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "memory.h"

typedef struct cunit {
	enum {
		CS_MAIN,
		CS_DEF_NAME,
		CS_DEF_BODY,
		CS_VAR_NAME,
		CS_LINK_TGT,
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

	vecbk *loopsv;
	struct cunitloop {
		size_t  begin, _while;
		bool    haswhile;
		vecbk * breaksv;
		size_t *breaks;
	} * loops;

	char *        dir;
	vecbk *       linksv;
	struct link **links;
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
	C_UNTERM_LINK,
	C_TGT_NOT_STR,
	C_LINK_FAIL,
	C_DUP_NAME,
	C_DUP_PREFIX,
	C_STRAY_REPEAT,
	C_STRAY_BREAK,
	C_STRAY_WHILE,
	C_DUP_WHILE,
	C_UNTERM_REPEAT,
};

cunit *newcunit(char *dir);
void   freecunit(cunit *);

int  compile(cunit *, char *s, size_t len);
int  closecunit(cunit *);
bool isrunnable(cunit *);

cunit *compilefile(FILE *, char *name, char *dir);

void pcerror(int);

#endif
