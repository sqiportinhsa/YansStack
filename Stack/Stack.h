#ifndef __STACK_SYM__
#define __STACK_SYM__

#include <stdlib.h>
#include <assert.h>

#include "../Strings/_Strings.h"

enum ErrorCodes
{
    NO_ERROR                = 0,
    NULL_STACK_POINTER      = 1,
    MEMORY_ALLOCATION_ERROR = 2,
    WRONG_SIZE              = 4,
    WRONG_CAPACITY          = 8,
    SIZE_BIGGER_CAPACITY    = 16,
    NULL_DATA               = 32
};

const char   LOGS[]     = "StackLogs.txt";
const double FOR_RESIZE = 1.618; 
const int    POISON     = 2147483647;
const void*  POISON_PTR = (void*)13;

typedef struct LogInfo
{
    char name[100]     = "";
    char function[100] = "";
    char file[100]     = "";
    int  line          = 0;
} LogInfo;

typedef struct Stack
{
    LogInfo debug = {};

    Elem *data     = nullptr;
    int     size     = 0;
    int     capacity = 0;
} Stack;

void dump(Stack *stk, const char function[], const char file[], int line)
{
    assert(stk);
    FILE* fp = fopen(LOGS, "a");
    if (fp == nullptr)
        return;
    fprintf(fp, "%s at %s(%d):\n", function, file, line);
    
    fprintf(fp, "Stack[%p] \"%s\" at %s at %s(%d):\n", stk, stk->debug.name, stk->debug.function, stk->debug.file, stk->debug.line);

    fprintf(fp, "{\n");
    fprintf(fp, "\tsize     = %d\n", stk->size);
    fprintf(fp, "\tcapacity = %d\n", stk->capacity);

    fprintf(fp, "\tdata[%p]\n", stk->data);
    fprintf(fp, "\t{\n");

    if (stk->data != nullptr && stk->size != POISON && stk->capacity != POISON && stk->size <= stk->capacity)
    {
        int i = 0;
        for(i = 0; i < stk->size; i++)
            fprintf(fp, "\t\t*[%d] = %d\n", i, stk->data[i]);

        for(i; i < stk->capacity; i++)
            fprintf(fp, "\t\t[%d] = %d\n", i, stk->data[i]);
    }

    fprintf(fp, "\t}\n");
    fprintf(fp, "}\n");
    fprintf(fp, "\n");
    fprintf(fp, "End dump\n\n");
    fclose(fp);
}

int StackCheck(Stack* stk)
{
    int error = NO_ERROR;
    if (stk == nullptr)
        error |= NULL_STACK_POINTER;
    if (stk->size < 0 || stk->size == POISON)
        error |= WRONG_SIZE;
    if (stk->capacity < 0|| stk->capacity == POISON)
        error |= WRONG_CAPACITY;
    if (stk->size > stk->capacity)
        error |= SIZE_BIGGER_CAPACITY;
    if (stk->data == nullptr)
        error |= NULL_DATA;
    
    FILE* fp = fopen(LOGS, "a");
    fprintf(fp, "\nChech status = %d\n", error);
    fclose(fp);

    return error;
}

#define DUMP(stk) dump(stk, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define OK_ASSERT(stk) {             \
    DUMP(stk);                        \
    int error = StackCheck(stk);       \
    /*assert(error);*/                  \
}

int StackConstructor(Stack* stk, int capacity, int line, const char function[], const char file[], const char name[]) 
{
    OK_ASSERT(stk);

    *stk = {};
    stk->capacity = capacity;
    
    stk->data = (Elem*)realloc(stk->data, capacity*sizeof(Elem));
    stk->size = 0;

    stk->debug = {};
    strcpy_(stk->debug.file, file);
    strcpy_(stk->debug.function, function);
    strcpy_(stk->debug.name, name);
    stk->debug.line = line;

    OK_ASSERT(stk);
    return NO_ERROR;
}

#define StackCtor(stk, capacity) StackConstructor(stk, capacity, __LINE__, __PRETTY_FUNCTION__, __FILE__, #stk)

int StackDtor(Stack* stk)
{
    OK_ASSERT(stk);
    stk->capacity = POISON - 1;
    stk->size     = POISON;
    free(stk->data);
    stk->data = (Elem*)POISON_PTR;

    stk->debug    = {};

    return NO_ERROR;
}

int StackResizeUp(Stack* stk)
{
    OK_ASSERT(stk);

    if (stk->capacity == 0)
    {
        stk->capacity = 10;
        stk->data = (Elem*)calloc(stk->capacity, sizeof(Elem));

        OK_ASSERT(stk);
        if (stk->data == nullptr)
            return MEMORY_ALLOCATION_ERROR;
        return NO_ERROR;
    }

    stk->capacity = stk->capacity * FOR_RESIZE;
    stk->data = (Elem*)realloc(stk->data, stk->capacity * sizeof(Elem));
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    
    OK_ASSERT(stk);
    return NO_ERROR;
}

int StackPush(Stack* stk, Elem value)
{
    OK_ASSERT(stk);

    if (stk->capacity == stk->size)
    {
        int error = 0;
        if ((error = StackResizeUp(stk)))
        {   
            OK_ASSERT(stk);
            return error;
        }
    }
    stk->data[stk->size++] = value;

    FILE* fp = fopen(LOGS, "a");
    fprintf(fp, "stk.data[%d] = %d\n", stk->size - 1, stk->data[stk->size - 1]);
    fclose(fp);

    OK_ASSERT(stk);
    return NO_ERROR;
}

int StackResizeDown(Stack* stk)
{
    OK_ASSERT(stk);

    if (stk->capacity == 0)
        return NO_ERROR;

    if (stk->capacity/(double)stk->size >= FOR_RESIZE*FOR_RESIZE)
    {
        stk->capacity = stk->capacity / FOR_RESIZE;

        stk->data = (Elem*)realloc(stk->data, stk->capacity * sizeof(Elem));
        if (stk->data == nullptr)
            return MEMORY_ALLOCATION_ERROR;
    }
    
    OK_ASSERT(stk);
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    return NO_ERROR;
}

Elem StackPop(Stack* stk, int *err = nullptr)
{
    OK_ASSERT(stk);

    if (stk->size > 0)
        stk->size--;
    if (stk->size >= 0)
    {

        if (stk->capacity <= 0)
        {
            if (err != nullptr)
                *err = WRONG_CAPACITY;
            return POISON;
        }
        Elem result = stk->data[stk->size];
        stk->data[stk->size] = POISON;

        int now_error = StackResizeDown(stk);
        if (err != nullptr)
            *err = now_error;
        if (now_error != NO_ERROR)
            return POISON;

        return result;
    }
    else
    {
        if (err != nullptr) *err = WRONG_SIZE;
        return POISON;
    }
}

#endif