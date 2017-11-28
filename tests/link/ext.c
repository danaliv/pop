#include <pop/ext.h>
#include <string.h>

int POP_test() {
    STACK_HAS_1(TSTR);

    value *v = pop();

    int sum = 0;
    for (size_t i = 0; i < strlen(STR(v)); i++) {
        sum += STR(v)[i];
    }
    pushint(sum);

    release(v);

    return E_OK;
}
