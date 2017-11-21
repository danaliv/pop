#include <dlfcn.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fgetln.h"
#include "link.h"
#include "memory.h"

#ifdef __APPLE__
#define SOEXT ".dylib"
#else
#define SOEXT ".so"
#endif

typedef struct lncache {
	char * path;
	size_t refs;

	struct cunit *cu;
	exctx *       ctx;

	void *dl;

	struct lncache *next;
} lncache;

static lncache *cache = NULL;

static char **parsedirs(char *s, size_t *np) {
	if (!s || *s == '\0') return NULL;

	*np = 1;
	for (size_t i = 0; i < strlen(s); i++) {
		if (s[i] == ':') ++*np;
	}

	char **dirs = xmalloc(*np * sizeof(char *));
	size_t dirsi = 0;

	for (size_t i = 0, j = 0; i <= strlen(s); i++) {
		if (s[i] == ':' || s[i] == '\0') {
			char *path = xmalloc(i - j + 1);
			strncpy(path, s + j, i - j);
			path[i - j] = '\0';

			dirs[dirsi] = realpath(path, NULL);
			if (dirs[dirsi]) {
				dirsi++;
			} else {
				--*np;
			}

			j = i + 1;
		}
	}

	return dirs;
}

static bool isanchored(char *path) {
	return *path == '/' || strncmp("./", path, 2) == 0 || strncmp("../", path, 3) == 0;
}

static char *sjoin(char *s1, ...) {
	va_list args1, args2;
	va_start(args1, s1);
	va_copy(args2, args1);

	size_t len = strlen(s1);
	while (true) {
		char *sn = va_arg(args1, char *);
		if (!sn) break;
		len += strlen(sn);
	}
	va_end(args1);

	char *s = xmalloc(len + 1);
	strcpy(s, s1);
	while (true) {
		char *sn = va_arg(args2, char *);
		if (!sn) break;
		strcat(s, sn);
	}
	va_end(args2);

	return s;
}

static char *find(char *tgt, char *rel) {
	static char **dirs = NULL;
	static size_t numdirs = 0;

	if (!dirs) {
		dirs = parsedirs(getenv("POPPATH"), &numdirs);
	}

	char **paths;
	size_t numpaths;
	size_t i;

	if (isanchored(tgt)) {
		numpaths = 2;
		paths = xmalloc(sizeof(char *) * 2);
		if (*tgt == '.') {
			paths[0] = sjoin(rel, "/", tgt, ".pop", NULL);
			paths[1] = sjoin(rel, "/", tgt, SOEXT, NULL);
		} else {
			paths[0] = sjoin(tgt, ".pop", NULL);
			paths[1] = sjoin(tgt, SOEXT, NULL);
		}
	} else {
		numpaths = (numdirs + 1) * 2;
		paths = xmalloc(sizeof(char *) * numpaths);
		for (i = 0; i < numdirs; i++) {
			paths[i * 2] = sjoin(dirs[i], "/", tgt, ".pop", NULL);
			paths[i * 2 + 1] = sjoin(dirs[i], "/", tgt, SOEXT, NULL);
		}
		paths[i * 2] = sjoin(rel, "/", tgt, ".pop", NULL);
		paths[i * 2 + 1] = sjoin(rel, "/", tgt, SOEXT, NULL);
	}

	char *found = NULL;
	for (i = 0; i < numpaths; i++) {
		if (!found) found = realpath(paths[i], NULL);
		free(paths[i]);
	}
	free(paths);

	return found;
}

static struct link *linkpop(char *path, struct link *ln) {
	FILE *file = fopen(path, "r");
	if (!file) goto lpfail1;

	char *pathcopy = xstrdup(path);
	ln->cu = newcunit(dirname(pathcopy));
	free(pathcopy);

	ln->ctx = newexctx();

	size_t lineno = 1;
	int    res;

	while (true) {
		char * line;
		size_t len;

		line = fgetln(file, &len);
		if (!line && feof(file)) break;
		if (!line && ferror(file)) goto lpfail2;

		res = compile(ln->cu, line, len);
		if (res != C_OK) goto lpfail3;

		lineno++;
	}

	res = closecunit(ln->cu);
	if (res != C_OK) goto lpfail3;

	res = run(ln->cu, ln->ctx);
	if (res != E_OK) {
		fprintf(stderr, "%s: ", path);
		prerror(res);
		goto lpfail2;
	}

	return ln;

lpfail3:
	fprintf(stderr, "%s:%lu: ", path, lineno);
	pcerror(res);
lpfail2:
	fclose(file);
	freecunit(ln->cu);
lpfail1:
	free(ln->name);
	free(ln);
	return NULL;
}

static struct link *linkso(char *path, struct link *ln) {
	ln->dl = dlopen(path, RTLD_NOW | RTLD_LOCAL);
	if (!ln->dl) {
		free(ln->name);
		free(ln);
		ln = NULL;
	}
	return ln;
}

struct link *newlink(char *tgt, char *rel, char *prefix) {
	char *path = find(tgt, rel);
	if (!path) return NULL;

	struct link *ln = xcalloc(sizeof(struct link), 1);

	if (!prefix) prefix = basename(tgt);
	ln->name = xstrdup(prefix);

	lncache *lc = cache;
	while (lc) {
		if (strcmp(lc->path, path) == 0) {
			ln->path = lc->path;
			ln->cu = lc->cu;
			ln->ctx = lc->ctx;
			ln->dl = lc->dl;
			lc->refs++;
			return ln;
		}
		lc = lc->next;
	}

	if (strcmp(path + strlen(path) - 4, ".pop") == 0) {
		ln = linkpop(path, ln);
	} else {
		ln = linkso(path, ln);
	}

	if (ln) {
		lc = xmalloc(sizeof(lncache));
		lc->path = ln->path = path;
		lc->refs = 1;
		lc->cu = ln->cu;
		lc->ctx = ln->ctx;
		lc->dl = ln->dl;
		lc->next = cache;
		cache = lc;
	}

	return ln;
}

void freelink(struct link *ln) {
	char *lnpath = ln->path;

	free(ln->name);
	free(ln);

	lncache *lc = cache;
	lncache *prev = NULL;
	while (lc) {
		if (lnpath == lc->path) { // pointer equality!
			lc->refs--;
			if (lc->refs != 0) return;
			break;
		}
		prev = lc;
		lc = lc->next;
	}

	free(lc->path);
	if (lc->cu) {
		freecunit(lc->cu);
		freeexctx(lc->ctx);
	}
	if (lc->dl) dlclose(lc->dl);

	if (prev) {
		prev->next = lc->next;
	} else {
		cache = lc->next;
	}

	free(lc);
}

bool linkinv(char *s, size_t len, char **prefixp, char **namep) {
	*prefixp = *namep = NULL;

	for (size_t i = 0; i < len; i++) {
		if (s[i] == '.') {
			*prefixp = xmalloc(i + 1);
			memcpy(*prefixp, s, i);
			(*prefixp)[i] = '\0';

			*namep = xmalloc(len - i);
			memcpy(*namep, s + i + 1, len - i - 1);
			(*namep)[len - i - 1] = '\0';

			return true;
		}
	}

	return false;
}

int addlinkcall(struct link *ln, vecbk *dstv, char *name) {
	uint8_t *dst = *dstv->itemsp;

	if (ln->cu) {
		for (size_t i = 0; i < ln->cu->defsv->len; i++) {
			if (strcmp(name, ln->cu->defs[i].name) == 0) {
				vadd(dstv);
				dst[dstv->len - 1] = OP_CALLIX;

				vaddn(dstv, sizeof(vecbk *));
				*(vecbk **) &dst[dstv->len - sizeof(vecbk *)] = ln->cu->defs[i].bodyv;

				vaddn(dstv, sizeof(exctx *));
				*(exctx **) &dst[dstv->len - sizeof(exctx *)] = ln->ctx;

				return C_OK;
			}
		}
		return C_UNK;
	}

	char *    sym = sjoin("POP_", name, NULL);
	callable *fn = (callable *) dlsym(ln->dl, sym);
	free(sym);
	if (!fn) return C_UNK;

	vadd(dstv);
	dst[dstv->len - 1] = OP_CALLC;

	vaddn(dstv, sizeof(callable *));
	*(callable **) &dst[dstv->len - sizeof(callable *)] = fn;

	return C_OK;
}
