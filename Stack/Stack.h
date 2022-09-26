#ifndef __STACK_SYM__
#define __STACK_SYM__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "../Strings/_Strings.h"

enum ErrorCodes
{
    NO_ERROR                = 0,
    NULL_STACK_POINTER      = 1,
    MEMORY_ALLOCATION_ERROR = 2,
    WRONG_SIZE              = 4,
    WRONG_CAPACITY          = 8,
    SIZE_BIGGER_CAPACITY    = 16,
    NULL_DATA               = 32,
    ERROR_LOGS_OPEN         = 64,
    LEFT_BORDER_DAMAGED     = 128,
    RIGHT_BORDER_DAMAGED   = 256,
    LEFT_BORDER_VIOLATED    = 128,
    RIGHT_BORDER_VIOLATED   = 256,
};

const char ERROR_DESCRIPTION[][150] = {{"Pointer to stack = nullptr\n"},
                                       {"Error during memmory allocation\n"},
                                       {"Negative or poison size\n"},
                                       {"Negative or poison capacity\n"},
                                       {"Size is bigger then capacity\n"},
                                       {"Pointer to stack.data = nullptr\n"},
                                       {"Error during open logs file\n"},
                                       {"The left boundary element is damaged. Other data in the structure may have been changed\n"},
                                       {"The right boundary element is damaged. Other data in the structure may have been changed\n"}};

const char   LOGS[]     = "StackLogs.txt";
const int    DUMP_LEVEL = 2;

const double FOR_RESIZE = 1.618; 
const int    POISON     = 2147483647;
const void*  POISON_PTR = (void*)13;
const size_t KENAR      = 0xAAAAAAAAAAAAAAAA;

size_t LogPrintf(FILE* fp, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    #ifdef LOGS_TO_CONSOLE
        vprintf(format, args);
    #endif

    #ifdef LOGS_TO_FILE
        if (fp == nullptr)
            return ERROR_LOGS_OPEN;
        vfprintf(fp, format, args);
    #endif

    va_end(args);
}


//!------------------------------
//!status = true  if stack is ready to be used
//!status = false if stack wasn`t constructed or was destructed
//!
//!------------------------------
typedef struct LogInfo
{
    char name[100]     = "(null)";
    char function[100] = "(null)";
    char file[100]     = "(null)";
    int  line          = POISON;
    bool status        = false;
} LogInfo;

typedef struct Stack
{
    size_t left_border = KENAR;

    LogInfo debug = {};

    Elem *data    = (Elem*)POISON_PTR;
    int   size     = POISON;
    int   capacity = POISON - 1;

    size_t right_border = KENAR ^ 1;
} Stack;


//!------------------------------
//!deep = 1 - dump without printing stack elements
//!deep = 2 - 10 elements from start and 10 elements from end of the stack will be printed 
//!deep > 2 - all elements of the stack will be printed
//!
//!-----------------------------
void DumpStack(Stack *stk, int deep, const char function[], const char file[], int line)
{
    FILE* fp = fopen(LOGS, "a");
    
    LogPrintf(fp, "%s at %s(%d):\n", function, file, line);
    if (stk == nullptr)
    {
        LogPrintf(fp, "Null pointer to stack\n");
        fclose(fp);
        return;
    }
    LogPrintf(fp, "Stack[%p] \"%s\" at %s at %s(%d):\n", stk, stk->debug.name, stk->debug.function, stk->debug.file, stk->debug.line);

    LogPrintf(fp, "Left border  = %llu\n", stk->left_border);
    LogPrintf(fp, "Rigth border = %llu\n", stk->right_border);
    LogPrintf(fp, "{\n");
    LogPrintf(fp, "\tsize     = %d\n", stk->size);
    LogPrintf(fp, "\tcapacity = %d\n", stk->capacity);

    LogPrintf(fp, "\tdata[%p]\n", stk->data);
    LogPrintf(fp, "\t{\n");

    if (deep > 1 && stk->data != nullptr && stk->size != POISON && stk->capacity != POISON && stk->size <= stk->capacity && stk->capacity >= 0 && stk->size >= 0)
        {
        LogPrintf(fp, "\t\tLeftCan  = %llu\n", ((size_t*)stk->data)[-1]);
        LogPrintf(fp, "\t\tRightCan = %llu\n", *(size_t*)((char*)stk->data + sizeof(Elem)*stk->capacity));
        int i = 0;
        if (deep > 2 || stk->capacity <= 20)
        {
            for(i = 0; i < stk->size; i++)
                LogPrintf(fp, "\t\t*[%d] = %d\n", i, stk->data[i]);

            for(i; i < stk->capacity; i++)
                LogPrintf(fp, "\t\t[%d] = %d\n", i, stk->data[i]);
        }
        else
        {
            int out = 10;
            for(int i = 0; i < out; i++)
            {
                LogPrintf(fp, "\t\t");
                if (stk->size > i)
                    LogPrintf(fp, "*");
                LogPrintf(fp, "[%d] = %d\n", i, stk->data[i]);
            }
            LogPrintf(fp, "\t\t...\n");
            for(int i = stk->capacity - out; i < stk->capacity; i++)
            {
                LogPrintf(fp, "\t\t");
                if (stk->size > i)
                    LogPrintf(fp, "*");
                LogPrintf(fp, "[%d] = %d\n", i, stk->data[i]);
            }
        }
    }

    LogPrintf(fp, "\t}\n}\n\n");

    fclose(fp);
}

size_t StackCheck(Stack* stk)
{
    size_t error = NO_ERROR;
    if (stk == nullptr)
        error |= NULL_STACK_POINTER;
    else
    {
        if (stk->size < 0 || stk->size == POISON)
            error |= WRONG_SIZE;
        if (stk->capacity < 0|| stk->capacity == POISON)
            error |= WRONG_CAPACITY;
        if (stk->size > stk->capacity)
            error |= SIZE_BIGGER_CAPACITY;
        if (stk->data == nullptr || stk->data == POISON_PTR)
            error |= NULL_DATA;
        else
        {
            if (((size_t*)stk->data)[-1] != KENAR)
                error |= LEFT_BORDER_DAMAGED;
            if (stk->capacity >= 0 && stk->capacity != POISON - 1 && 
                *(size_t*)((char*)stk->data + stk->capacity*sizeof(Elem)) != KENAR ^ 1)
                error |= RIGHT_BORDER_DAMAGED;
        }
    }
    
    FILE* fp = fopen(LOGS, "a");
    LogPrintf(fp, "\nStack = %p\n" "Chech status = %d\n", stk, error);
    if (error != 0)
    {
        for(int i = 0; i < 32; i++)
            if (error & (1 << i))
                LogPrintf(fp, ERROR_DESCRIPTION[i]);
        LogPrintf(fp, "\n");
    }
    fclose(fp);

    return error;
}

#define DUMP_STACK(stk) DumpStack(stk, DUMP_LEVEL, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define OK_ASSERT(stk) {             \
    StackCheck(stk);                  \
    DUMP_STACK(stk);                   \
}

size_t StackConstructor(Stack* stk, int capacity, int line, const char function[], const char file[], const char name[]) 
{
    OK_ASSERT(stk);
    size_t error = 0;
    *stk = {};
    stk->capacity = capacity;

    size_t new_capacity = stk->capacity*sizeof(Elem) + sizeof(KENAR)*2;
    char* mem_block = (char*)calloc(new_capacity, 1);
    if (mem_block == nullptr)
    {
        stk->data = nullptr;
        error |= NULL_DATA | MEMORY_ALLOCATION_ERROR;
    }
    else
    {
        *(size_t*)mem_block = KENAR;
        *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = KENAR^1;
        stk->data = (Elem*)(mem_block + sizeof(KENAR));
    }
    stk->size = 0;

    stk->debug = {};
    strcpy_(stk->debug.file, file);
    strcpy_(stk->debug.function, function);
    strcpy_(stk->debug.name, name);
    stk->debug.line   = line;
    stk->debug.status = true;  

    OK_ASSERT(stk);
    return error;
}

#define StackCtor(stk, capacity) StackConstructor(stk, capacity, __LINE__, __PRETTY_FUNCTION__, __FILE__, #stk)

size_t StackDtor(Stack* stk)
{
    OK_ASSERT(stk);
    stk->capacity = POISON - 1;
    stk->size     = POISON;
    free((char*)stk->data - sizeof(KENAR));
    stk->data     = (Elem*)POISON_PTR;

    stk->debug.status = 0;

    return NO_ERROR;
}

size_t StackResizeUp(Stack* stk)
{
    OK_ASSERT(stk);

    if (stk->capacity == 0)
    {
        stk->capacity = 10;
        size_t new_capacity = stk->capacity*sizeof(Elem) + sizeof(KENAR)*2;

        char* mem_block = (char*)calloc(new_capacity, 1);
        *(size_t*)mem_block = KENAR;
        *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = KENAR^1;
        stk->data = (Elem*)(mem_block + sizeof(KENAR));

        OK_ASSERT(stk);
        if (stk->data == nullptr)
            return MEMORY_ALLOCATION_ERROR;
        return NO_ERROR;
    }

    stk->capacity = stk->capacity * FOR_RESIZE;
    size_t new_capacity = stk->capacity*sizeof(Elem) + sizeof(KENAR)*2;

    char* mem_block = (char*)realloc((char*)stk->data - sizeof(KENAR), new_capacity);
    *(size_t*)mem_block = KENAR;
    *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = KENAR^1;
    stk->data = (Elem*)(mem_block + sizeof(KENAR));
    
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    
    OK_ASSERT(stk);
    return NO_ERROR;
}

size_t StackPush(Stack* stk, Elem value)
{
    OK_ASSERT(stk);

    if (stk->capacity == stk->size)
    {
        size_t error = 0;
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

size_t StackResizeDown(Stack* stk)
{
    OK_ASSERT(stk);

    if (stk->capacity == 0)
        return NO_ERROR;

    if (stk->capacity/(double)stk->size >= FOR_RESIZE*FOR_RESIZE)
    {
        stk->capacity = stk->capacity / FOR_RESIZE;

        size_t new_capacity = stk->capacity*sizeof(Elem) + sizeof(KENAR)*2;
        char* mem_block = (char*)realloc((char*)stk->data - sizeof(KENAR), new_capacity);
        
        if (mem_block == nullptr)
        {
            stk->data = nullptr;
            return MEMORY_ALLOCATION_ERROR;
        }

        *(size_t*)mem_block = KENAR;
        *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = KENAR^1;
        stk->data = (Elem*)(mem_block + sizeof(KENAR));
    }
    
    OK_ASSERT(stk);
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    return NO_ERROR;
}

Elem StackPop(Stack* stk, size_t *err = nullptr)
{
    OK_ASSERT(stk);

    if (stk->size > 0)
        stk->size--;
    if (stk->size >= 0)
    {
        if (stk->capacity <= 0)
        {
            if (err != nullptr)
                *err |= WRONG_CAPACITY;
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
        if (err != nullptr) 
            *err = WRONG_SIZE;
        return POISON;
    }
}

#endif