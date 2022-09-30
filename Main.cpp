#include <stdio.h>

struct stc
{
    long long a[100] = {};
};

typedef int Elem;
#define LOGS_TO_FILE
//#define LOGS_TO_CONSOLE
#include "Stack\Stack.h"

int main()
{
    Stack stk = {};
    int err = 0;
    err += StackCtor(&stk, 8);
    for (int i = 0; i < 10; ++i) {
        StackPush(&stk, i);
    }
    stk.data[3] = 14;
    stk.left_border = 14;
    DUMP_STACK(stk);
    for (int i = 9; i >= 0; --i) {
        printf("popped: %d", StackPop(&stk));
    }
    err += StackDtor(&stk);
    DUMP_STACK(stk);
    return 0;
}