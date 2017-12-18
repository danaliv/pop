#include <openssl/rand.h>
#include <pop/ext.h>
#include <stdlib.h>
#include <string.h>

#include "ow-crypt.h"

int POP_gensalt() {
	STACK_HAS_1(TINT);

	value *v = pop();
	int    cost = INT(v);
	release(v);

	unsigned char input[16];
	if (RAND_bytes(input, sizeof(input)) != 1) {
		pushopt(NULL);
		return E_OK;
	}

	char *salt = crypt_gensalt("$2a$", cost, (const char *) input, sizeof(input));

	if (salt) {
		value *v = newstr(salt);
		pushopt(v);
		release(v);
	} else {
		pushopt(NULL);
	}

	return E_OK;
}

int POP_encrypt() {
	STACK_HAS_2(TSTR, TSTR);

	value *plaintext = pop();
	value *salt = pop();

	void *ciphertext = NULL;
	int   size = 0;

	char *res = crypt_ra(STR(plaintext), STR(salt), &ciphertext, &size);

	release(plaintext);
	release(salt);

	if (res) {
		char *s = xmalloc(size + 1);
		memcpy(s, ciphertext, size);
		s[size] = 0;
		free(ciphertext);

		value *v = newstr(s);
		pushopt(v);
		release(v);
	} else {
		pushopt(NULL);
	}

	return E_OK;
}
