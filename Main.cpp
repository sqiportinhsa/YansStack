#include <stdio.h>

typedef int Elem;
#define LOGS_TO_FILE
#include "Stack\Stack.h"

int main()
{
    printf("KENAR = %lld", KENAR);
    system("pause");
    FILE* fp = fopen(LOGS, "w");
    fclose(fp);

    Stack stk1 = {};
    StackCtor(&stk1, 10);
    for(int i = 0; i < 10; i++)
        StackPush(&stk1, i);
    StackPush(&stk1, 120);
    StackPush(&stk1, 120);

    for(int i = 0; i < 20; i++)
        printf("%d \n", StackPop(&stk1));

    printf("End pop\n");

    for(int i = 0; i < 1000; i++)
    {
        StackPush(&stk1, i*i);
        printf("%d Push\n", i);
    }

    for(int i = 0; i < 1010; i++)
        printf("%d\n", StackPop(&stk1));

    StackDtor(&stk1);
    DUMP_STACK(&stk1);
}