#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "compile.h"
#include "exec.h"
#include "stack.h"

void usage() {
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  pop program-file         runs code in program-file\n");
	fprintf(stderr, "  pop -e program-code      runs program-code\n");
	fprintf(stderr, "  pop                      (with piped input) runs program in input\n");
	fprintf(stderr, "  pop                      (with terminal input) runs REPL\n");
}

int rerrexit(int res) {
	switch (res) {
	case E_OOM:
		return EX_OSERR;
	case E_NO_VAL:
		return EX_SOFTWARE;
	default:
		return EX_DATAERR;
	}
}

void printstack() {
	putchar('(');
	frame *f[4];
	f[3] = stack;
	f[2] = f[3] ? f[3]->down : NULL;
	f[1] = f[2] ? f[2]->down : NULL;
	f[0] = f[1] ? f[1]->down : NULL;
	if (stack) {
		putchar(' ');
	}
	if (f[0]) {
		printf("...");
	}
	for (int i = 1; i < 4; i++) {
		if (f[i - 1]) {
			putchar(' ');
		}
		if (f[i]) {
			switch (f[i]->tp) {
			case F_STR:
				printf("\"%s\"", f[i]->s);
				break;
			case F_INT:
				printf("%d", f[i]->i);
				break;
			case F_REF:
				printf("VAR#%lu", f[i]->ref);
				break;
			}
		}
	}
	if (stack) {
		putchar(' ');
	}
	putchar(')');
}

int repl() {
	exctx *ctx;

	cunit *cu = newcunit();
	if (!cu) {
		perror(NULL);
		return EX_OSERR;
	}

	while (1) {
		char * line;
		size_t len;

		printstack();
		if (cu->state == CS_DEF_NAME || cu->state == CS_DEF_BODY) {
			printf(" :   ");
		} else {
			printf(" > ");
		}
		line = fgetln(stdin, &len);
		if (!line && feof(stdin)) {
			break;
		}
		if (!line && ferror(stdin)) {
			perror(NULL);
			return EX_IOERR;
		}

		int res = compile(&cu, line, len);
		if (res == C_OK) {
			if (cu->state == CS_MAIN) {
				res = run(cu, &ctx);
				cu->mainlen = 0;
				if (res != E_OK) {
					prerror(res);
					res = rerrexit(res);
					if (res != EX_DATAERR) {
						return res;
					}
				}
			}
		} else {
			pcerror(res);
			cu->mainlen = 0;
			if (res == C_OOM) {
				return EX_OSERR;
			}
		}
	}

	return EX_OK;
}

int evalfile(FILE *file) {
	size_t lineno = 1;

	cunit *cu = newcunit();
	if (!cu) {
		perror(NULL);
		return EX_OSERR;
	}

	while (1) {
		char * line;
		size_t len;

		line = fgetln(file, &len);
		if (!line && feof(file)) {
			break;
		}
		if (!line && ferror(file)) {
			perror(NULL);
			return EX_IOERR;
		}

		// ignore hashbang
		if (lineno == 1 && len > 1 && line[0] == '#' && line[1] == '!') {
			lineno++;
			continue;
		}

		int res = compile(&cu, line, len);
		if (res != C_OK) {
			fprintf(stderr, "line %lu: ", lineno);
			pcerror(res);
			return EX_DATAERR;
		}

		lineno++;
	}

	if (cu->state != CS_MAIN) {
		fprintf(stderr, "Program ends abruptly\n");
		return EX_DATAERR;
	}

	int res = run(cu, NULL);
	if (res != E_OK) {
		prerror(res);
		return rerrexit(res);
	}

	return EX_OK;
}

int evalstr(char *str) {
	cunit *cu = newcunit();
	if (!cu) {
		perror(NULL);
		return EX_OSERR;
	}

	int res = compile(&cu, str, strlen(str));
	if (res == C_OK) {
		if (cu->state != CS_MAIN) {
			fprintf(stderr, "Program ends abruptly\n");
			return EX_DATAERR;
		}
		res = run(cu, NULL);
		if (res != E_OK) {
			prerror(res);
			return rerrexit(res);
		}
	} else {
		pcerror(res);
		if (res == C_OOM) {
			return EX_OSERR;
		}
		return EX_DATAERR;
	}

	return EX_OK;
}

int main(int argc, char *argv[]) {
	// REPL or piped input
	if (argc == 1) {
		if (isatty(fileno(stdin))) {
			return repl();
		}
		return evalfile(stdin);
	}

	// file input
	if (argc == 2) {
		FILE *file = fopen(argv[1], "r");
		if (!file) {
			perror(argv[1]);
			return EX_NOINPUT;
		}
		return evalfile(file);
	}

	// one-liner with -e option
	if (argc == 3 && strcmp("-e", argv[1]) == 0) {
		return evalstr(argv[2]);
	}

	usage();
	return EX_USAGE;
}
