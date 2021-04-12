#ifndef EXEC_H
#define EXEC_H

#include "compile.h"
#include "memory.h"
#include "stack.h"

enum {
	OP_NONE = 0,
	OP_POP,
	OP_SWAP,
	OP_DUP,
	OP_PUSHS,
	OP_PUSHI,
	OP_PUSHV,
	OP_CALLI,
	OP_CALLIX,
	OP_CALLC,
	OP_STORE,
	OP_FETCH,
	OP_JP,
	OP_PJZ,
};

enum {
	E_OK = 0,
	E_UNDERFLOW,
	E_TYPE,
	E_DIV0,
	E_RANGE,
};

typedef struct {
	vecbk * varsv;
	value **vars;
} exctx;

typedef int callable();

exctx *newexctx();
void   freeexctx(exctx *);

int run(cunit *, exctx *);

void prerror(int);

#endif // EXEC_H
