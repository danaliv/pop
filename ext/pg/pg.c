#include <libpq-fe.h>
#include <pop/ext.h>

int POP_connect() {
	STACK_HAS_1(F_STR);

	PGconn *conn = PQconnectdb(stack->s);
	pop();
	pushobj(conn, (destructor *) PQfinish);

	return E_OK;
}
