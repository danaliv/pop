#include <stdio.h>
#include <string.h>

#include "exec.h"
#include "stack.h"

int runv(cunit * cu, uint8_t * body, size_t len) {
	int res;
	frame *f;

	for (size_t i = 0; i < len; i++) {
		switch (body[i]) {
		case OP_NONE:
			break;

			// there's no good reason to implement the stack manipulation primitives this way rather
			// than as CALLC ops, but the latter way seems somehow... distasteful?
		case OP_POP:
			if (!stack) {
				return E_EMPTY;
			}
			pop();
			break;
		case OP_SWAP:
			if (!stack && !stack->down) {
				return E_TOOFEW;
			}
			f = stack;
			stack = stack->down;
			f->down = stack->down;
			stack->down = f;
			break;
		case OP_DUP:
			if (!stack) {
				return E_EMPTY;
			}
			switch (stack->tp) {
			case F_STR:
				f = pushs(stack->s);
				break;
			case F_INT:
				f = pushi(stack->i);
				break;
			}
			if (!f) {
				return E_OOM;
			}
			break;

			// pushes, on the other hand, can't be done as function calls.
		case OP_PUSHS:
			if (i > len - 2) {
				return E_NO_VAL;
			}
			if (!pushs((char *) &body[i + 1])) {
				return E_OOM;
			}
			while (body[i]) {
				i++;
			}
			break;
		case OP_PUSHI:
			if (i > len - (sizeof(int) + 1)) {
				return E_NO_VAL;
			}
			if (!pushi(*(int *) &body[i + 1])) {
				return E_OOM;
			}
			i += sizeof(int);
			break;

		case OP_CALLI:
			if (i > len - 2) {
				return E_NO_VAL;
			}
			for (size_t j = 0; j < cu->ndefs; j++) {
				if (strcmp(cu->defs[j].name, (const char *) &body[i + 1]) == 0) {
					int res = runv(cu, cu->defs[j].body, cu->defs[j].bodylen);
					if (res) {
						return res;
					}
					goto callok;
				}
			}
			return E_UNDEF;
		  callok:
			while (body[i]) {
				i++;
			}
			break;
		case OP_CALLC:
			if (i > len - (sizeof(callable *) + 1)) {
				return E_NO_VAL;
			}
			res = (*(callable **) (&body[i + 1])) ();
			if (res != E_OK) {
				return res;
			}
			i += sizeof(callable *);
			break;
		}
	}

	return E_OK;
}

int run(cunit * cu) {
	return runv(cu, cu->main, cu->mainlen);
}

void prerror(int err) {
	switch (err) {
	case E_EMPTY:
		fprintf(stderr, "Stack is empty\n");
		break;
	case E_NO_VAL:
		fprintf(stderr, "Malformed bytecode (internal error)\n");
		break;
	case E_OOM:
		fprintf(stderr, "Out of memory\n");
		break;
	case E_UNDEF:
		fprintf(stderr, "Unknown word\n");
		break;
	case E_TOOFEW:
		fprintf(stderr, "Not enough items on the stack\n");
		break;
	case E_TYPE:
		fprintf(stderr, "Wrong type(s) on stack\n");
		break;
	case E_DIV0:
		fprintf(stderr, "Division by zero\n");
		break;
	case E_RANGE:
		fprintf(stderr, "Integer value out of range\n");
		break;
	}
}
