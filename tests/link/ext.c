#include <pop/ext.h>
#include <string.h>

int POP_test() {
    STACK_HAS_1(F_STR);

    int sum = 0;
    for (size_t i = 0; i < strlen(stack->s); i++) {
        sum += stack->s[i];
    }

    pop();
    pushi(sum);

    return E_OK;
}
