#include <stdio.h>
#include <string.h>

#include "exec.h"
#include "stack.h"

int runv(cunit * cu, uint8_t * body, size_t len) {
	int res;

	for (size_t i = 0; i < len; i++) {
		switch (body[i]) {
		case OP_NONE:
			break;
		case OP_POP:
			if (!stack) {
				return E_EMPTY;
			}
			pop();
			break;
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
	}
}
