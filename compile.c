#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "compile.h"
#include "exec.h"
#include "builtins.h"

cunit *newcunit() {
	cunit *cu = malloc(sizeof(cunit));
	if (cu) {
		cu->indef = false;
		cu->mainlen = 0;
		cu->main = calloc(1, 128);
		if (!cu->main) {
			free(cu);
			return NULL;
		}
		cu->ndefs = 0;
	}
	return cu;
}

void freecunit(cunit * cu) {
	for (size_t i = 0; i < cu->ndefs; i++) {
		free(cu->defs[i].body);
	}
	free(cu->main);
	free(cu);
}

bool strkeq(const char *k, char *s, size_t slen) {
	if (slen != strlen(k)) {
		return false;
	}

	while (*k) {
		if (*k != *s) {
			return false;
		}
		k++;
		s++;
	}

	return true;
}

int addopwopnd(uint8_t op, void *opnd, size_t opndlen, uint8_t ** dstp, size_t * lenp) {
	uint8_t *dst = *dstp;

	dst = realloc(dst, *lenp + 1 + opndlen);
	if (!dst) {
		return C_OOM;
	}
	*dstp = dst;

	dst[*lenp] = op;
	++*lenp;
	if (opnd) {
		memcpy(&dst[*lenp], opnd, opndlen);
		*lenp += opndlen;
	}

	return C_OK;
}

int addop(uint8_t op, uint8_t ** dstp, size_t * lenp) {
	return addopwopnd(op, NULL, 0, dstp, lenp);
}

char *parsestr(char *tk, size_t len) {
	char *s = malloc(len);
	if (s) {
		size_t ti, si;
		for (ti = 1, si = 0; ti < len; ti++, si++) {
			if (tk[ti] == '"') {
				break;
			}
			if (tk[ti] == '\\') {
				ti++;
			}
			s[si] = tk[ti];
		}
		s[si] = 0;
	}
	return s;
}

bool parseint(char *tk, size_t len, int *n) {
    // TODO: make this not dumb
    size_t i = 0;
    int sign = 1;
    int place = 1;

    switch (*tk) {
    case '-':
        sign = -1;
    case '+':
        if (len == 1) {
            return false;
        }
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

int addtoken(cunit ** cup, char *tk, size_t len) {
	cunit *cu = *cup;

	// word definitions
	if (strkeq(":", tk, len)) {
		if (cu->indef) {
			return C_IN_DEF;
		}

		cu = realloc(cu, sizeof(cunit) + sizeof(struct cunitdef) * (cu->ndefs + 1));
		if (!cu) {
			return C_OOM;
		}
		*cup = cu;

		cu->defs[cu->ndefs].name = NULL;
		cu->defs[cu->ndefs].bodylen = 0;
		cu->defs[cu->ndefs].body = calloc(1, 128);
		if (!cu->defs[cu->ndefs].body) {
			return C_OOM;
		}
		cu->indef = true;
		cu->ndefs++;
		return C_OK;
	}

	if (strkeq(";", tk, len)) {
		if (!cu->indef) {
			return C_NOT_IN_DEF;
		}
		cu->indef = false;
		return C_OK;
	}

	if (cu->indef && !cu->defs[cu->ndefs - 1].name) {
		// TODO: verify name legality
		cu->defs[cu->ndefs - 1].name = malloc(len + 1);
		if (!cu->defs[cu->ndefs - 1].name) {
			return C_OOM;
		}
		memcpy(cu->defs[cu->ndefs - 1].name, tk, len);
		cu->defs[cu->ndefs - 1].name[len] = 0;
		return C_OK;
	}

	// is the instruction in main or a word?
	uint8_t **dstp;
	size_t *lenp;
	if (cu->indef) {
		dstp = &cu->defs[cu->ndefs - 1].body;
		lenp = &cu->defs[cu->ndefs - 1].bodylen;
	}
	else {
		dstp = &cu->main;
		lenp = &cu->mainlen;
	}

	// string pushes
	if (*tk == '"') {
		char *s = parsestr(tk, len);
		if (!s) {
			return C_OOM;
		}

		int res = addopwopnd(OP_PUSHS, s, strlen(s) + 1, dstp, lenp);
		free(s);
		return res;
	}

	// integer pushes
	int n;
	if (parseint(tk, len, &n)) {
		return addopwopnd(OP_PUSHI, &n, sizeof(n), dstp, lenp);
	}

	// simple builtins
#define SIMPLE_OP(k, op) if (strkeq(k, tk, len)) return addop(op, dstp, lenp)

	SIMPLE_OP("pop", OP_POP);
	SIMPLE_OP("swap", OP_SWAP);
	SIMPLE_OP("dup", OP_DUP);

	// builtins implemented by C functions
#define CALLC_OP(k, f) if (strkeq(k, tk, len)) { \
    callable *fp = &f; \
    return addopwopnd(OP_CALLC, &fp, sizeof(fp), dstp, lenp); \
}

	CALLC_OP("puts", builtin_puts);
	CALLC_OP("+", builtin_add);
	CALLC_OP("-", builtin_sub);
	CALLC_OP("*", builtin_mul);
	CALLC_OP("/", builtin_div);
	CALLC_OP("DEBUG_stack", builtin_DEBUG_stack);

	// calls to user-defined words
	for (size_t i = 0; i < cu->ndefs; i++) {
		if (strkeq(cu->defs[i].name, tk, len)) {
			return addopwopnd(OP_CALLI, cu->defs[i].name, strlen(cu->defs[i].name) + 1, dstp, lenp);
		}
	}

	return C_UNK;
}

void skipspace(char **s, size_t * len) {
	while (*len && isspace(**s)) {
		--*len;
		++*s;
	}
}

void findspace(char **s, size_t * len) {
	while (*len && !isspace(**s)) {
		--*len;
		++*s;
	}
}

bool findquote(char **s, size_t * len) {
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

int compile(cunit ** cup, char *s, size_t len) {
	while (len) {
		skipspace(&s, &len);
		if (!len) {
			break;
		}

		char *tk = s;
		if (*s == '"') {
			if (!findquote(&s, &len)) {
				return C_UNTERM_STR;
				break;
			}
			if (len && !isspace(*s)) {
				return C_STR_SPACE;
				break;
			}
		}
		else {
			findspace(&s, &len);
		}

		int res = addtoken(cup, tk, s - tk);
		if (res != C_OK) {
			return res;
		}
	}

	return C_OK;
}

void pcerror(int err) {
	switch (err) {
	case C_IN_DEF:
		fprintf(stderr, "Can't use : inside a word definition\n");
		break;
	case C_OOM:
		fprintf(stderr, "Out of memory\n");
		break;
	case C_UNK:
		fprintf(stderr, "Unrecognized word\n");
		break;
	case C_NOT_IN_DEF:
		fprintf(stderr, "Can't use ; outside a word definition\n");
		break;
	case C_UNTERM_STR:
		fprintf(stderr, "String has no end quote\n");
		break;
	case C_STR_SPACE:
		fprintf(stderr, "No space after string\n");
		break;
	}
}
