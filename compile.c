#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "compile.h"
#include "exec.h"
#include "memory.h"

cunit *newcunit() {
	cunit *cu = xmalloc(sizeof(cunit));
	cu->state = CS_MAIN;
	cu->incomment = false;
	cu->mainv = newvec(128, sizeof(uint8_t), (void **) &cu->main);
	cu->varsv = newvec(4, sizeof(char *), (void **) &cu->vars);
	cu->defsv = newvec(4, sizeof(struct cunitdef), (void **) &cu->defs);
	cu->ifsv = newvec(4, sizeof(struct cunitif), (void **) &cu->ifs);
	return cu;
}

void freecunit(cunit *cu) {
	size_t i;

	for (i = 0; i < cu->varsv->len; i++) {
		free(cu->vars[i]);
	}
	vfree(cu->varsv);

	for (i = 0; i < cu->defsv->len; i++) {
		vfree(cu->defs[i].bodyv);
	}
	vfree(cu->defsv);

	vfree(cu->mainv);
	vfree(cu->ifsv);

	free(cu);
}

bool strkeq(const char *k, char *s, size_t slen) {
	if (slen != strlen(k)) return false;

	while (*k) {
		if (*k != *s) return false;
		k++;
		s++;
	}

	return true;
}

int addopwopnd(uint8_t op, void *opnd, size_t opndlen, vecbk *dstv) {
	uint8_t *dst = *dstv->itemsp;

	vaddn(dstv, 1 + opndlen);
	dst[dstv->len - (1 + opndlen)] = op;
	memcpy(dst + dstv->len - opndlen, opnd, opndlen);

	return C_OK;
}

int addop(uint8_t op, vecbk *dstv) {
	uint8_t *dst = *dstv->itemsp;

	vadd(dstv);
	dst[dstv->len - 1] = op;

	return C_OK;
}

char *parsestr(char *tk, size_t len) {
	char * s = xmalloc(len);
	size_t ti, si;
	for (ti = 1, si = 0; ti < len; ti++, si++) {
		if (tk[ti] == '"') break;
		if (tk[ti] == '\\') {
			ti++;
			if (ti < len && tk[ti] == 'n') {
				s[si] = '\n';
				continue;
			}
		}
		s[si] = tk[ti];
	}
	s[si] = 0;
	return s;
}

bool parseint(char *tk, size_t len, int *n) {
	// TODO: make this not dumb
	size_t i = 0;
	int    sign = 1;
	int    place = 1;

	switch (*tk) {
	case '-':
		sign = -1;
	case '+':
		if (len == 1) return false;
		i++;
		break;
	}

	for (size_t j = i + 1; j < len; j++) {
		place *= 10;
	}

	*n = 0;
	while (i < len) {
		if (tk[i] >= '0' && tk[i] <= '9') {
			*n += (tk[i] - '0') * place;
		} else {
			return false;
		}
		i++;
		place /= 10;
	}
	*n *= sign;

	return true;
}

static size_t tmpaddr = -1;

static int addif(cunit *cu, vecbk *dstv) {
	vadd(cu->ifsv);

	struct cunitif *ifp = cu->ifs + cu->ifsv->len - 1;

	ifp->ifi = dstv->len;
	ifp->haselse = false;

	return addopwopnd(OP_PJZ, &tmpaddr, sizeof(size_t), dstv);
}

static int addelse(cunit *cu, vecbk *dstv) {
	if (cu->ifsv->len == 0) return C_STRAY_ELSE;

	struct cunitif *ifp = cu->ifs + cu->ifsv->len - 1;

	if (ifp->haselse) return C_STRAY_ELSE;

	ifp->elsei = dstv->len;
	ifp->haselse = true;

	return addopwopnd(OP_JP, &tmpaddr, sizeof(size_t), dstv);
}

static int addthen(cunit *cu, vecbk *dstv) {
	if (cu->ifsv->len == 0) return C_STRAY_THEN;
	cu->ifsv->len--;

	uint8_t *       body = (uint8_t *) *dstv->itemsp;
	struct cunitif *ifp = cu->ifs + cu->ifsv->len;

	if (ifp->haselse) {
		*(size_t *) &body[ifp->ifi + 1] = ifp->elsei + 1 + sizeof(size_t);
		*(size_t *) &body[ifp->elsei + 1] = dstv->len;
	} else {
		*(size_t *) &body[ifp->ifi + 1] = dstv->len;
	}

	return C_OK;
}

#define CALLC_OP(k, f) \
	if (strkeq(k, tk, len)) { \
		callable *fp = &f; \
		return addopwopnd(OP_CALLC, &fp, sizeof(fp), dstv); \
	}

int addinstr(cunit *cu, char *tk, size_t len, vecbk *dstv) {
	// string pushes
	if (*tk == '"') {
		char *s = parsestr(tk, len);
		int   res = addopwopnd(OP_PUSHS, s, strlen(s) + 1, dstv);
		free(s);
		return res;
	}

	// integer pushes
	int n;
	if (parseint(tk, len, &n)) {
		return addopwopnd(OP_PUSHI, &n, sizeof(n), dstv);
	}

	// core VM ops
	if (strkeq("!", tk, len)) return addop(OP_STORE, dstv);
	if (strkeq("@", tk, len)) return addop(OP_FETCH, dstv);

	// branches
	if (strkeq("if", tk, len)) return addif(cu, dstv);
	if (strkeq("else", tk, len)) return addelse(cu, dstv);
	if (strkeq("then", tk, len)) return addthen(cu, dstv);

	// builtins implemented by C functions/callc ops
	CALLC_OP("pop", builtin_pop);
	CALLC_OP("swap", builtin_swap);
	CALLC_OP("dup", builtin_dup);
	CALLC_OP("rot", builtin_rot);
	CALLC_OP("rotate", builtin_rotate);
	CALLC_OP("over", builtin_over);
	CALLC_OP("pick", builtin_pick);
	CALLC_OP("getenv", builtin_getenv);
	CALLC_OP("puts", builtin_puts);
	CALLC_OP(".", builtin_puts);
	CALLC_OP("+", builtin_add);
	CALLC_OP("-", builtin_sub);
	CALLC_OP("*", builtin_mul);
	CALLC_OP("/", builtin_div);
	CALLC_OP("=", builtin_eq);

	CALLC_OP("DEBUG_stack", builtin_DEBUG_stack);
	CALLC_OP("DEBUG_puts_all", builtin_DEBUG_puts_all);

	// calls to user-defined words
	for (size_t i = 0; i < cu->defsv->len; i++) {
		char *name = cu->defs[i].name;
		if (strkeq(name, tk, len)) {
			return addopwopnd(OP_CALLI, name, strlen(name) + 1, dstv);
		}
	}

	// variable reference pushes
	for (size_t i = 0; i < cu->varsv->len; i++) {
		if (strkeq(cu->vars[i], tk, len)) {
			return addopwopnd(OP_PUSHREF, &i, sizeof(i), dstv);
		}
	}

	return C_UNK;
}

int addtoken_MAIN(cunit *cu, char *tk, size_t len) {
	if (strkeq(":", tk, len)) {
		vadd(cu->defsv);
		size_t i = cu->defsv->len - 1;
		cu->defs[i].name = NULL;
		cu->defs[i].bodyv = newvec(128, sizeof(uint8_t), (void **) &cu->defs[i].body);

		cu->state = CS_DEF_NAME;
		return C_OK;
	}

	if (strkeq(";", tk, len)) return C_NOT_IN_DEF;

	if (strkeq(".var", tk, len)) {
		cu->state = CS_VAR_NAME;
		return C_OK;
	}

	return addinstr(cu, tk, len, cu->mainv);
}

int addtoken_DEF_NAME(cunit *cu, char *tk, size_t len) {
	// TODO: verify name legality

	char *name = xmalloc(len + 1);
	memcpy(name, tk, len);
	name[len] = 0;

	cu->defs[cu->defsv->len - 1].name = name;

	cu->state = CS_DEF_BODY;
	return C_OK;
}

int addtoken_DEF_BODY(cunit *cu, char *tk, size_t len) {
	if (strkeq(":", tk, len)) return C_IN_DEF;

	if (strkeq(";", tk, len)) {
		cu->state = CS_MAIN;
		return C_OK;
	}

	return addinstr(cu, tk, len, cu->defs[cu->defsv->len - 1].bodyv);
}

int addtoken_VAR_NAME(cunit *cu, char *tk, size_t len) {
	// TODO: verify name legality

	char *name = xmalloc(len + 1);
	memcpy(name, tk, len);
	name[len] = 0;

	vadd(cu->varsv);
	cu->vars[cu->varsv->len - 1] = name;

	cu->state = CS_MAIN;
	return C_OK;
}

int addtoken(cunit *cu, char *tk, size_t len) {
	switch (cu->state) {
	case CS_MAIN:
		return addtoken_MAIN(cu, tk, len);
	case CS_DEF_NAME:
		return addtoken_DEF_NAME(cu, tk, len);
	case CS_DEF_BODY:
		return addtoken_DEF_BODY(cu, tk, len);
	case CS_VAR_NAME:
		return addtoken_VAR_NAME(cu, tk, len);
	}
}

void skipspace(char **s, size_t *len, bool *incomment) {
	while (*len && isspace(**s)) {
		--*len;
		++*s;
	}

	if (*len && (**s == '(' || *incomment)) {
		*incomment = true;
		while (*len && **s != ')') {
			--*len;
			++*s;
		}
		if (*len && **s == ')') {
			--*len;
			++*s;
			*incomment = false;
		}
		skipspace(s, len, incomment);
	}
}

void findspace(char **s, size_t *len) {
	while (*len && !isspace(**s) && **s != '(') {
		--*len;
		++*s;
	}
}

bool findquote(char **s, size_t *len) {
	--*len;
	++*s;
	while (*len) {
		if (**s == '"') {
			--*len;
			++*s;
			return true;
		}
		if (**s == '\\') {
			--*len;
			++*s;
			if (!*len) {
				break;
			}
		}
		--*len;
		++*s;
	}
	return false;
}

int compile(cunit *cu, char *s, size_t len) {
	while (len) {
		skipspace(&s, &len, &cu->incomment);
		if (!len) break;

		char *tk = s;
		if (*s == '"') {
			if (!findquote(&s, &len)) return C_UNTERM_STR;
			if (len && !isspace(*s)) return C_STR_SPACE;
		} else {
			findspace(&s, &len);
		}

		int res = addtoken(cu, tk, s - tk);
		if (res != C_OK) return res;
	}

	return C_OK;
}

int closecunit(cunit *cu) {
	if (cu->incomment) return C_UNTERM_COMMENT;

	switch (cu->state) {
	case CS_DEF_NAME:
	case CS_DEF_BODY:
		return C_UNTERM_DEF;
	case CS_VAR_NAME:
		return C_UNTERM_VAR;
	default:
		if (cu->ifsv->len) return C_UNTERM_IF;
	}

	return C_OK;
}

bool isrunnable(cunit *cu) {
	return cu->state == CS_MAIN && !cu->incomment && !cu->ifsv->len;
}

void pcerror(int err) {
	switch (err) {
	case C_IN_DEF:
		fputs("Can't use : inside a word definition\n", stderr);
		break;
	case C_UNK:
		fputs("Unrecognized word\n", stderr);
		break;
	case C_NOT_IN_DEF:
		fputs("Can't use ; outside a word definition\n", stderr);
		break;
	case C_UNTERM_STR:
		fputs("String has no end quote\n", stderr);
		break;
	case C_STR_SPACE:
		fputs("No space after string\n", stderr);
		break;
	case C_STRAY_ELSE:
		fputs("ELSE with no IF\n", stderr);
		break;
	case C_STRAY_THEN:
		fputs("THEN with no IF\n", stderr);
		break;
	case C_UNTERM_COMMENT:
		fputs("Unterminated comment\n", stderr);
		break;
	case C_UNTERM_IF:
		fputs("IF with no THEN\n", stderr);
		break;
	case C_UNTERM_DEF:
		fputs("Unterminated word definition\n", stderr);
		break;
	case C_UNTERM_VAR:
		fputs(".var with no name\n", stderr);
		break;
	}
}
