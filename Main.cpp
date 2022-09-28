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
    FILE* fp = fopen(LOGS, "w");
    fclose(fp);

    Stack stk1 = {};
    StackCtor(&stk1, 10);

    StackPush(&stk1, 120);
    StackPush(&stk1, 120);

    StackDtor(&stk1);
    DUMP_STACK(stk1);
}