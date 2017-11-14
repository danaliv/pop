#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"
#include "memory.h"
#include "stack.h"

exctx *newexctx() {
	exctx *ctx = xmalloc(sizeof(exctx));
	ctx->varsv = newvec(4, sizeof(frame), (void **) &ctx->vars);
	return ctx;
}

void freeexctx(exctx *ctx) {
	for (size_t i = 0; i < ctx->varsv->len; i++) {
		if (ctx->vars[i].tp == F_STR) {
			free(ctx->vars[i].s);
		}
	}
	vfree(ctx->varsv);
	free(ctx);
}

static void skipop(vecbk *bodyv, size_t *ip) {
	const uint8_t *body = *bodyv->itemsp;

    switch (body[*ip]) {
        case OP_PUSHS:
        case OP_CALLI:
            for (++*ip; *ip < bodyv->len; ++*ip) {
                if (body[*ip] == '\0') break;
            }
            break;
        case OP_PUSHI:
            (*ip) += sizeof(int);
            break;
        case OP_PUSHREF:
            (*ip) += sizeof(size_t);
            break;
        case OP_CALLC:
            (*ip) += sizeof(callable *);
            break;
    }
}

static int op_if(vecbk *bodyv, size_t *ip) {
	if (!stack) return E_EMPTY;
	if (stack->tp != F_INT) return E_TYPE;

    // is the top value nonzero? continue executing normally
	int n = stack->i;
	pop();
	if (n) return E_OK;

    // otherwise, find the matching ELSE or THEN
	const uint8_t *body = *bodyv->itemsp;
	size_t depth = 1;

    for (++*ip; *ip < bodyv->len; ++*ip) {
        switch (body[*ip]) {
        case OP_IF:
            depth++;
            break;
        case OP_ELSE:
            if (depth == 1) return E_OK;
            break;
        case OP_THEN:
            depth--;
            if (depth == 0) return E_OK;
            break;
        default:
            skipop(bodyv, ip);
        }
    }

    return E_NO_THEN;
}

static int op_else(vecbk *bodyv, size_t *ip) {
    // find the matching THEN
	const uint8_t *body = *bodyv->itemsp;
	size_t depth = 1;

    for (++*ip; *ip < bodyv->len; ++*ip) {
        switch (body[*ip]) {
        case OP_IF:
            depth++;
            break;
        case OP_ELSE:
            if (depth == 1) return E_STRAY_ELSE;
            break;
        case OP_THEN:
            depth--;
            if (depth == 0) return E_OK;
            break;
        default:
            skipop(bodyv, ip);
        }
    }

    return E_NO_THEN;
}

int runv(cunit *cu, vecbk *bodyv, exctx *ctx) {
	int      res;
	frame *  f;
	uint8_t *body = *bodyv->itemsp;

	for (size_t i = 0; i < bodyv->len; i++) {
		switch (body[i]) {
		case OP_NONE:
			break;

			// there's no good reason to implement the stack manipulation primitives
			// this way rather than as CALLC ops, but the latter way seems somehow...
			// distasteful?
		case OP_POP:
			if (!stack) {
				return E_EMPTY;
			}
			pop();
			break;
		case OP_SWAP:
			if (!stack || !stack->down) {
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
				pushs(stack->s);
				break;
			case F_INT:
				pushi(stack->i);
				break;
			case F_REF:
				pushref(stack->ref);
				break;
			}
			break;

			// pushes, on the other hand, can't be done as function calls.
		case OP_PUSHS:
			if (i > bodyv->len - 2) {
				return E_NO_VAL;
			}
			pushs((char *) &body[i + 1]);
			while (body[i]) {
				i++;
			}
			break;
		case OP_PUSHI:
			if (i > bodyv->len - (sizeof(int) + 1)) {
				return E_NO_VAL;
			}
			pushi(*(int *) &body[i + 1]);
			i += sizeof(int);
			break;
		case OP_PUSHREF:
			if (i > bodyv->len - (sizeof(size_t) + 1)) {
				return E_NO_VAL;
			}
			pushref(*(size_t *) &body[i + 1]);
			i += sizeof(size_t);
			break;

		case OP_CALLI:
			if (i > bodyv->len - 2) {
				return E_NO_VAL;
			}
			for (size_t j = 0; j < cu->defsv->len; j++) {
				if (strcmp(cu->defs[j].name, (const char *) &body[i + 1]) == 0) {
					int res = runv(cu, cu->defs[j].bodyv, ctx);
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
			if (i > bodyv->len - (sizeof(callable *) + 1)) {
				return E_NO_VAL;
			}
			res = (*(callable **) (&body[i + 1]))();
			if (res != E_OK) {
				return res;
			}
			i += sizeof(callable *);
			break;

		case OP_STORE:
			if (!stack || !stack->down) {
				return E_TOOFEW;
			}
			if (stack->tp != F_REF) {
				return E_TYPE;
			}

			if (ctx->vars[stack->ref].tp == F_STR) {
				free(ctx->vars[stack->ref].s);
			}

			ctx->vars[stack->ref] = *stack->down;
			if (stack->down->tp == F_STR) {
				ctx->vars[stack->ref].s = xmalloc(strlen(stack->down->s) + 1);
				strcpy(ctx->vars[stack->ref].s, stack->down->s);
			}

			pop();
			pop();
			break;
		case OP_FETCH:
			if (!stack) {
				return E_EMPTY;
			}
			if (stack->tp != F_REF) {
				return E_TYPE;
			}

			f = copyframe(&ctx->vars[stack->ref]);
			pop();
			f->down = stack;
			stack = f;
			break;

		case OP_IF:
			res = op_if(bodyv, &i);
			if (res != E_OK) {
				return res;
			}
			break;
		case OP_ELSE:
		    res = op_else(bodyv, &i);
		    if (res != E_OK) {
		        return res;
		    }
		    break;
		}
	}

	return E_OK;
}

int run(cunit *cu, exctx *ctx) {
	if (!ctx) {
		ctx = newexctx();
	}

	// grow context's variable array if necessary
	if (ctx->varsv->len < cu->varsv->len) {
		size_t oldlen = ctx->varsv->len;
		vaddn(ctx->varsv, cu->varsv->len - ctx->varsv->len);
		for (size_t i = oldlen; i < ctx->varsv->len; i++) {
			ctx->vars[i].tp = F_INT;
			ctx->vars[i].i = 0;
		}
	}

	// run main
	return runv(cu, cu->mainv, ctx);
}

void prerror(int err) {
	switch (err) {
	case E_EMPTY:
		fprintf(stderr, "Stack is empty\n");
		break;
	case E_NO_VAL:
		fprintf(stderr, "Malformed bytecode (internal error)\n");
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
	case E_NO_THEN:
	    fprintf(stderr, "Unterminated IF\n");
	    break;
	case E_STRAY_ELSE:
	    fprintf(stderr, "ELSE with no IF\n");
	    break;
	}
}
