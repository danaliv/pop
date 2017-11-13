#ifndef __EXEC_H__
#define __EXEC_H__

#include "compile.h"
#include "stack.h"

enum {
	OP_NONE = 0,
	OP_POP,
	OP_SWAP,
	OP_DUP,
	OP_PUSHS,
	OP_PUSHI,
	OP_PUSHREF,
	OP_CALLI,
	OP_CALLC,
	OP_STORE,
	OP_FETCH,
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
	E_RANGE,
};

typedef struct {
	size_t  nvars;
	frame **vars;
} exctx;

typedef int callable(void);

int run(cunit *, exctx **);

void prerror(int);

#endif
