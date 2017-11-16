#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

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

static void bcabort() {
	fputs("Malformed bytecode (internal error/bug)\n", stderr);
	exit(EX_SOFTWARE);
}

static int op_callc(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(callable *) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	int res = (*(callable **) (&body[*ip + 1]))();
	if (res == E_OK) {
		(*ip) += sizeof(callable *);
	}
	return res;
}

static int runv(cunit *, vecbk *, exctx *);

static int op_calli(cunit *cu, exctx *ctx, vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - 2) bcabort();

	const uint8_t *body = *bodyv->itemsp;

	for (size_t j = 0; j < cu->defsv->len; j++) {
		if (strcmp(cu->defs[j].name, (const char *) &body[*ip + 1]) == 0) {
			int res = runv(cu, cu->defs[j].bodyv, ctx);
			if (res == E_OK) {
				while (body[*ip]) ++*ip;
			}
			return res;
		}
	}

	return E_UNDEF;
}

static int op_fetch(exctx *ctx) {
	STACK_HAS_1(F_REF);

	frame *f = copyframe(&ctx->vars[stack->ref]);
	pop();
	f->down = stack;
	stack = f;

	return E_OK;
}

static int op_jp(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(size_t) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	*ip = (*(size_t *) &body[*ip + 1]) - 1;

	return E_OK;
}

static int op_pjz(vecbk *bodyv, size_t *ip) {
	STACK_HAS_1(F_INT);

	if (stack->i) {
		pop();
		(*ip) += sizeof(size_t);
		return E_OK;
	}

	pop();
	return op_jp(bodyv, ip);
}

static int op_pushi(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(int) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	pushi(*(int *) &body[*ip + 1]);
	(*ip) += sizeof(int);

	return E_OK;
}

static int op_pushref(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(size_t) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	pushref(*(size_t *) &body[*ip + 1]);
	(*ip) += sizeof(size_t);

	return E_OK;
}

static int op_pushs(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - 2) bcabort();

	const uint8_t *body = *bodyv->itemsp;

	pushs((char *) &body[*ip + 1]);
	while (body[*ip]) {
		++*ip;
	}

	return E_OK;
}

static int op_store(exctx *ctx) {
	STACK_HAS_1(F_REF);
	if (!stack->down) return E_UNDERFLOW;

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

	return E_OK;
}

static int runv(cunit *cu, vecbk *bodyv, exctx *ctx) {
	int      res;
	uint8_t *body = *bodyv->itemsp;

	for (size_t i = 0; i < bodyv->len; i++) {
		res = E_OK;

		switch (body[i]) {
		case OP_CALLC:
			res = op_callc(bodyv, &i);
			break;
		case OP_CALLI:
			res = op_calli(cu, ctx, bodyv, &i);
			break;
		case OP_FETCH:
			res = op_fetch(ctx);
			break;
		case OP_JP:
			res = op_jp(bodyv, &i);
			break;
		case OP_PJZ:
			res = op_pjz(bodyv, &i);
			break;
		case OP_PUSHI:
			res = op_pushi(bodyv, &i);
			break;
		case OP_PUSHREF:
			res = op_pushref(bodyv, &i);
			break;
		case OP_PUSHS:
			res = op_pushs(bodyv, &i);
			break;
		case OP_STORE:
			res = op_store(ctx);
			break;
		}

		if (res != E_OK) return res;
	}

	return E_OK;
}

int run(cunit *cu, exctx *ctx) {
	if (!ctx) ctx = newexctx();

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
	case E_UNDERFLOW:
		fputs("Stack underflow\n", stderr);
		break;
	case E_UNDEF:
		fputs("Unknown word\n", stderr);
		break;
	case E_TYPE:
		fputs("Wrong type(s) on stack\n", stderr);
		break;
	case E_DIV0:
		fputs("Division by zero\n", stderr);
		break;
	case E_RANGE:
		fputs("Integer value out of range\n", stderr);
		break;
	}
}
