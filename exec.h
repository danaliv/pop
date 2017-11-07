#ifndef __EXEC_H__
#define __EXEC_H__

#include "compile.h"

enum {
	OP_NONE = 0,
	OP_POP,
	OP_SWAP,
	OP_DUP,
	OP_CALL,
	OP_PUSHS,
	OP_PUSHI,
	OP_PUTS
};

enum {
	E_OK = 0,
	E_EMPTY,
	E_NO_VAL,
	E_OOM,
	E_UNDEF
};

int run(cunit *);

void prerror(int);

#endif
