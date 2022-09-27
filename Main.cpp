#include <stdio.h>

typedef long long Elem;
#define LOGS_TO_FILE
//#define LOGS_TO_CONSOLE
#include "Stack\Stack.h"

int main()
{
    FILE* fp = fopen(LOGS, "w");
    fclose(fp);

    Stack stk1 = {};
    StackCtor(&stk1, 10);

    for(int i = 0; i < 10; i++)
        StackPush(&stk1, i);

    StackPush(&stk1, 120);
    StackPush(&stk1, 120);

    //stk1.data[-1] = 0;
    //OK_ASSERT(&stk1);
    //printf("hash = %llu\n", GetHash(stk1.data, stk1.capacity*sizeof(Elem)));
    //stk1.data[1] = 0;
    //printf("hash = %llu\n", GetHash(stk1.data, stk1.capacity*sizeof(Elem)));
    //OK_ASSERT(&stk1);

    for(int i = 0; i < 20; i++)
        printf("%llu \n", StackPop(&stk1));


    StackDtor(&stk1);
    DUMP_STACK(&stk1);
}