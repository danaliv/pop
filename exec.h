#ifndef __EXEC_H__
#define __EXEC_H__

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
	OP_PUSHREF,
	OP_CALLI,
	OP_CALLC,
	OP_STORE,
	OP_FETCH,
	OP_JP,
	OP_PJNZ,
};

enum {
	E_OK = 0,
	E_UNDERFLOW,
	E_UNDEF,
	E_TYPE,
	E_DIV0,
	E_RANGE,
};

typedef struct {
	vecbk *varsv;
	frame *vars;
} exctx;

typedef int callable(void);

exctx *newexctx();
void   freeexctx(exctx *);

int run(cunit *, exctx *);

void prerror(int);

#endif
