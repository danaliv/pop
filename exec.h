#ifndef __EXEC_H__
#define __EXEC_H__

#include "compile.h"

enum {
	OP_NONE = 0,
	OP_POP,
	OP_SWAP,
	OP_DUP,
	OP_PUSHS,
	OP_PUSHI,
	OP_CALLI,
	OP_CALLC
};

enum {
	E_OK = 0,
	E_EMPTY,
	E_NO_VAL,
	E_OOM,
	E_UNDEF,
	E_TOOFEW,
	E_TYPE,
	E_DIV0,
	E_RANGE
};

typedef int callable(void);

int run(cunit *);

void prerror(int);

#endif
