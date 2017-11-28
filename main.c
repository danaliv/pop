#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#ifdef MTRACE
#include <mcheck.h>
#endif

#include "compile.h"
#include "exec.h"
#include "fgetln.h"
#include "stack.h"

void usage() {
	fputs("Usage:\n", stderr);
	fputs("  pop program-file         runs code in program-file\n", stderr);
	fputs("  pop -e program-code      runs program-code\n", stderr);
	fputs("  pop                      (with piped input) runs program in input\n", stderr);
	fputs("  pop                      (with terminal input) runs REPL\n", stderr);
}

void printstack(cunit *cu) {
	frame *f[4];
	f[3] = stack;
	f[2] = f[3] ? f[3]->down : NULL;
	f[1] = f[2] ? f[2]->down : NULL;
	f[0] = f[1] ? f[1]->down : NULL;

	putchar('(');
	if (stack) putchar(' ');
	if (f[0]) printf("...");

	for (int i = 1; i < 4; i++) {
		if (f[i - 1]) putchar(' ');
		if (f[i]) {
			char *s = inspect(f[i]->v, cu->vars);
			printf("%s", s);
			free(s);
		}
	}

	if (stack) putchar(' ');
	putchar(')');
}

int repl() {
	cunit *cu = newcunit(".");
	exctx *ctx = newexctx();

	while (1) {
		char * line;
		size_t len;

		switch (cu->state) {
		case CS_DEF_NAME:
			printf("  : ");
			break;
		case CS_DEF_BODY:
			printf("  : %s > ", cu->defs[cu->defsv->len - 1].name);
			break;
		default:
			if (cu->incomment) {
				printf("  ( > ");
			} else if (cu->ifsv->len) {
				printf("  if > ");
			} else {
				printstack(cu);
				printf(" > ");
			}
		}

		line = fgetln(stdin, &len);
		if (!line && feof(stdin)) break;
		if (!line && ferror(stdin)) {
			perror(NULL);
			return EX_IOERR;
		}

		int res = compile(cu, line, len);
		if (res == C_OK) {
			if (isrunnable(cu)) {
				res = run(cu, ctx);
				cu->mainv->len = 0;
				if (res != E_OK) prerror(res);
			}
		} else {
			pcerror(res);
			cu->mainv->len = 0;
		}
	}

	return EX_OK;
}

int evalfile(FILE *file, char *name, char *dir) {
	cunit *cu = compilefile(file, name, dir);
	if (!cu) return EX_DATAERR;

	int res = run(cu, NULL);
	if (res != E_OK) {
		prerror(res);
		return EX_DATAERR;
	}

	return EX_OK;
}

int evalstr(char *str) {
	cunit *cu = newcunit(".");

	int res = compile(cu, str, strlen(str));
	if (res == C_OK) res = closecunit(cu);
	if (res != C_OK) {
		pcerror(res);
		return EX_DATAERR;
	}

	res = run(cu, NULL);
	if (res != E_OK) {
		prerror(res);
		return EX_DATAERR;
	}

	while (stack) release(pop());
	freecunit(cu);
	return EX_OK;
}

int main(int argc, char *argv[]) {
#ifdef MTRACE
	mtrace();
#endif

	// REPL or piped input
	if (argc == 1) {
		if (isatty(fileno(stdin))) {
			return repl();
		}
		return evalfile(stdin, "(stdin)", ".");
	}

	// file input
	if (argc == 2) {
		FILE *file = fopen(argv[1], "r");
		if (!file) {
			perror(argv[1]);
			return EX_NOINPUT;
		}
		return evalfile(file, argv[1], argv[1]);
	}

	// one-liner with -e option
	if (argc == 3 && strcmp("-e", argv[1]) == 0) {
		return evalstr(argv[2]);
	}

	usage();
	return EX_USAGE;
}
