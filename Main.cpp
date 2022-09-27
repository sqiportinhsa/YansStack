#include <stdio.h>

typedef long long Elem;
#define LOGS_TO_FILE
//#define LOGS_TO_CONSOLE
#include "Stack\Stack.h"

int main()
{
    printf("prot & can =  %d\n", PROTECTION_LEVEL & CANARY_PROTECTION); 
    FILE* fp = fopen(LOGS, "w");
    fclose(fp);

    Stack stk1 = {};
    StackCtor(&stk1, 10);

    for(int i = 0; i < 10; i++)
        StackPush(&stk1, i);

    StackPush(&stk1, 120);
    StackPush(&stk1, 120);

    for(int i = 0; i < 20; i++)
        printf("%llu \n", StackPop(&stk1));

    OK_ASSERT(&stk1);

    StackDtor(&stk1);
    DUMP_STACK(&stk1);
}