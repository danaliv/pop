#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "compile.h"
#include "exec.h"

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

int addop(uint8_t op, uint8_t ** dstp, size_t * lenp) {
	uint8_t *dst = *dstp;

	dst = realloc(dst, *lenp + 1);
	if (!dst) {
		return C_OOM;
	}
	*dstp = dst;

	dst[*lenp] = op;
	++*lenp;

	return C_OK;
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

		int res = addop(OP_PUSHS, dstp, lenp);
		if (res) {
			free(s);
			return res;
		}

		uint8_t *dst = *dstp;
		dst = realloc(dst, *lenp + strlen(s) + 1);
		if (!dst) {
			free(s);
			--*lenp;
			return C_OOM;
		}
		*dstp = dst;
		memcpy(dst + *lenp, s, strlen(s) + 1);
		*lenp += strlen(s) + 1;

		free(s);
		return C_OK;
	}

	// simple builtins
#define SIMPLE_OP(k, op) if (strkeq(k, tk, len)) return addop(op, dstp, lenp)

	SIMPLE_OP("pop", OP_POP);
	SIMPLE_OP("swap", OP_SWAP);
	SIMPLE_OP("dup", OP_DUP);
	SIMPLE_OP("puts", OP_PUTS);

	// calls to user-defined words
	for (size_t i = 0; i < cu->ndefs; i++) {
		if (strkeq(cu->defs[i].name, tk, len)) {
			int res = addop(OP_CALL, dstp, lenp);
			if (res) {
				return res;
			}

			uint8_t *dst = *dstp;
			dst = realloc(dst, *lenp + strlen(cu->defs[i].name) + 1);
			if (!dst) {
				--*lenp;
				return C_OOM;
			}
			*dstp = dst;
			memcpy(dst + *lenp, cu->defs[i].name, strlen(cu->defs[i].name) + 1);
			*lenp += strlen(cu->defs[i].name) + 1;

			return C_OK;
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
