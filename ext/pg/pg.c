#include <libpq-fe.h>
#include <pop/ext.h>

int POP_connect() {
	STACK_HAS_1(TSTR);

	value * v = pop();
	PGconn *conn = PQconnectdb(STR(v));
	release(v);

	if (conn) {
		v = newref(conn, (destructor *) PQfinish);
		pushopt(v);
		release(v);
	} else {
		pushopt(NULL);
	}

	return E_OK;
}
