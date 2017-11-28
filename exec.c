#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "exec.h"
#include "memory.h"
#include "stack.h"

exctx *newexctx() {
	exctx *ctx = xmalloc(sizeof(exctx));
	ctx->varsv = newvec(4, sizeof(value *), (void **) &ctx->vars);
	return ctx;
}

void freeexctx(exctx *ctx) {
	for (size_t i = 0; i < ctx->varsv->len; i++) {
		release(ctx->vars[i]);
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

	vecbk *defbodyv = *(vecbk **) &body[*ip + 1];
	int    res = runv(cu, defbodyv, ctx);
	(*ip) += sizeof(vecbk *);

	return res;
}

static int op_callix(cunit *cu, vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - 2) bcabort();

	const uint8_t *body = *bodyv->itemsp;

	vecbk *defbodyv = *(vecbk **) &body[*ip + 1];
	exctx *ctx = *(exctx **) &body[*ip + 1 + sizeof(vecbk *)];
	int    res = runv(cu, defbodyv, ctx);
	(*ip) += sizeof(vecbk *);
	(*ip) += sizeof(exctx *);

	return res;
}

static int op_fetch(exctx *ctx) {
	STACK_HAS_1(TVAR);
	push(ctx->vars[popvar()]);
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
	STACK_HAS_1(TINT);

	int n = popint();
	if (n) {
		(*ip) += sizeof(size_t);
		return E_OK;
	}

	return op_jp(bodyv, ip);
}

static int op_pushi(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(int) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	pushint(*(int *) &body[*ip + 1]);
	(*ip) += sizeof(int);

	return E_OK;
}

static int op_pushvar(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - (sizeof(size_t) + 1)) {
		bcabort();
	}

	const uint8_t *body = *bodyv->itemsp;

	pushvar(*(size_t *) &body[*ip + 1]);
	(*ip) += sizeof(size_t);

	return E_OK;
}

static int op_pushs(vecbk *bodyv, size_t *ip) {
	if (*ip > bodyv->len - 2) bcabort();

	const uint8_t *body = *bodyv->itemsp;

	pushstr((char *) &body[*ip + 1]);
	while (body[*ip]) {
		++*ip;
	}

	return E_OK;
}

static int op_store(exctx *ctx) {
	STACK_HAS_1(TVAR);
	if (!stack->down) return E_UNDERFLOW;

	size_t var = popvar();
	release(ctx->vars[var]);
	ctx->vars[var] = pop();

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
		case OP_CALLIX:
			res = op_callix(cu, bodyv, &i);
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
		case OP_PUSHV:
			res = op_pushvar(bodyv, &i);
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
	bool ctxistmp = !ctx;
	if (ctxistmp) ctx = newexctx();

	// grow context's variable array if necessary
	if (ctx->varsv->len < cu->varsv->len) {
		size_t oldlen = ctx->varsv->len;
		vaddn(ctx->varsv, cu->varsv->len - ctx->varsv->len);
		for (size_t i = oldlen; i < ctx->varsv->len; i++) {
			ctx->vars[i] = newint(0);
		}
	}

	// run main
	int res = runv(cu, cu->mainv, ctx);
	if (ctxistmp) freeexctx(ctx);
	return res;
}

void prerror(int err) {
	switch (err) {
	case E_UNDERFLOW:
		fputs("Stack underflow\n", stderr);
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
