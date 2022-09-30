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
    RIGHT_BORDER_DAMAGED    = 256,
    STRUCT_HASH_MISMATCH    = 512,
    DATA_HASH_MISMATCH      = 1024,
};

const char ERROR_DESCRIPTION[][150] = {{"Pointer to stack = nullptr\n"},
                                       {"Error during memmory allocation\n"},
                                       {"Negative or poison size\n"},
                                       {"Negative or poison capacity\n"},
                                       {"Size is bigger then capacity\n"},
                                       {"Pointer to stack.data = nullptr\n"},
                                       {"Error during open logs file\n"},
                                       {"The left boundary element is damaged. Other data in the structure may have been changed\n"},
                                       {"The right boundary element is damaged. Other data in the structure may have been changed\n"},
                                       {"Structure of stack was damaged\n"},
                                       {"Stack elements was damaged\n"}};

const char LOGS[]           = "StackLogs.txt";
const int  DUMP_LEVEL       = 10;

#define PROTECTION_LEVEL  3 //! if first bit = 1 canary protection on. if second bit = 1 hash protection on
#define CANARY_PROTECTION 1
#define HASH_PROTECTION   2

const double FOR_RESIZE  = 1.618; 
const Elem   POISON      = 2147483647;
const void*  POISON_PTR  = (void*)13;
const size_t KENAR       = 0xAAAAAAAAADEADDED;
const int    OUTPUT_TYPE = 0;   //!This constant is used to print stack elements to logs in right format
                                //!0 - int
                                //!1 - char
                                //!2 - float
                                //!3 - double
                                //!4 - long long


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

size_t PrintElem(Elem value, FILE *fp = nullptr)
{
    char format[5] = "%";
    switch (OUTPUT_TYPE)
    {
        case 0:
            format[1] = 'd';
        break;
        case 1:
            format[1] = 'c';
        break;
        case 2:
            format[1] = 'f';
        break;
        case 3:
            format[1] = 'l';
            format[2] = 'g';
        break;
        case 4:
            format[1] = 'l';
            format[2] = 'l';
            format[3] = 'u';
        break;
    }
    
    #ifdef LOGS_TO_CONSOLE
        printf(format, value);
    #endif
    #ifdef LOGS_TO_FILE
        if (fp == nullptr)
            return ERROR_LOGS_OPEN;
        fprintf(fp, format, value);
    #endif
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

    Elem *data     = (Elem*)POISON_PTR;
    int   size     = POISON;
    int   capacity = POISON - 1;

    size_t struct_hash = 0;
    size_t data_hash   = 0;

    size_t right_border = KENAR ^ 1;
} Stack;

size_t GetHash(void* struct_ptr, size_t size)
{
    if (struct_ptr == nullptr || size < 0 || size == POISON)
        return POISON;

    char* ptr = (char*)struct_ptr;

    size_t hash = 5381;
    for(int i = 0; i < size; i++)
        hash = (size_t)(ptr[0] + hash*33);

    return hash;
}

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
    
    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
    {
        LogPrintf(fp, "Left border  = %llu\n", stk->left_border);
        LogPrintf(fp, "Rigth border = %llu\n", stk->right_border);
    }
    #endif

    LogPrintf(fp, "{\n");
    LogPrintf(fp, "\tsize        = %d\n", stk->size);
    LogPrintf(fp, "\tcapacity    = %d\n", stk->capacity);
    LogPrintf(fp, "\tdata_hash   = %llu\n", stk->data_hash);
    LogPrintf(fp, "\tstruct_hash = %llu\n", stk->struct_hash);

    LogPrintf(fp, "\tdata[%p]\n", stk->data);
    LogPrintf(fp, "\t{\n");

    if (deep > 1 && stk->data != nullptr && stk->size != POISON && stk->capacity != POISON && stk->size <= stk->capacity && stk->capacity >= 0 && stk->size >= 0)
    {
        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        {
            LogPrintf(fp, "\t\tLeftCan  = %llu\n", ((size_t*)stk->data)[-1]);
            LogPrintf(fp, "\t\tRightCan = %llu\n", *(size_t*)((char*)stk->data + sizeof(Elem)*stk->capacity));
        }
        #endif

        int i = 0;
        if (deep > 2 || stk->capacity <= 20)
        {
            for(; i < stk->size; i++)
            {
                LogPrintf(fp, "\t\t*[%d] = ", i);
                PrintElem(stk->data[i], fp);
                LogPrintf(fp, "\n");
            }

            for(; i < stk->capacity; i++)
            {
                LogPrintf(fp, "\t\t[%d] = ", i);
                PrintElem(stk->data[i], fp);
                LogPrintf(fp, "\n");
            }

        }
        else
        {
            int out = 10;
            for(; i < out; i++)
            {
                LogPrintf(fp, "\t\t");
                if (stk->size > i)
                    LogPrintf(fp, "*");
                LogPrintf(fp, "[%d] = ", i);
                PrintElem(stk->data[i], fp);
                LogPrintf(fp, "\n");
            }
            LogPrintf(fp, "\t\t...\n");
            for(i = stk->capacity - out; i < stk->capacity; i++)
            {
                LogPrintf(fp, "\t\t");
                if (stk->size > i)
                    LogPrintf(fp, "*");
                LogPrintf(fp, "[%d] = ", i);
                PrintElem(stk->data[i], fp);
                LogPrintf(fp, "\n");
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
            #if (PROTECTION_LEVEL & HASH_PROTECTION)
            {
                size_t hash = GetHash(stk->data, stk->capacity*sizeof(Elem));
                if (hash != stk->data_hash)
                    error |= DATA_HASH_MISMATCH;
            }
            #endif

            #if (PROTECTION_LEVEL & CANARY_PROTECTION)
            {
                if (((size_t*)stk->data)[-1] != KENAR)
                    error |= LEFT_BORDER_DAMAGED;
                if (stk->capacity >= 0 && stk->capacity != POISON - 1 && 
                    *(size_t*)((char*)stk->data + stk->capacity*sizeof(Elem)) != KENAR ^ 1)
                    error |= RIGHT_BORDER_DAMAGED;
            }
            #endif
        }
        
        #if (PROTECTION_LEVEL & HASH_PROTECTION)
        {
            int old_hash = stk->struct_hash;
            stk->struct_hash = 0;
            int now_hash = GetHash(stk, sizeof(Stack));
            stk->struct_hash = old_hash;
            if (stk->struct_hash != now_hash)
                error |= STRUCT_HASH_MISMATCH;
        }
        #endif
    }
    
    FILE* fp = fopen(LOGS, "a");
    LogPrintf(fp, "Stack = %p\n" "Chech status = %d\n", stk, error);
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

#define DUMP_STACK(stk) DumpStack(&stk, DUMP_LEVEL, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define OK_ASSERT(stk) {               \
    StackCheck(&stk);                  \
    DUMP_STACK(stk);                   \
}

size_t StackConstructor(Stack* stk, int capacity, int line, const char function[], const char file[], const char name[]) 
{
    OK_ASSERT(*stk);
    size_t error = 0;
    *stk = {};
    stk->capacity = capacity;

    size_t new_capacity = stk->capacity*sizeof(Elem);
    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        new_capacity += sizeof(KENAR)*2;
    #endif
    char* mem_block = (char*)calloc(new_capacity, 1);
    if (mem_block == nullptr)
    {
        stk->data = nullptr;
        error |= NULL_DATA | MEMORY_ALLOCATION_ERROR;
    }
    else
    {
        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        {
            *(size_t*)mem_block = KENAR;
            *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = (size_t)(-1)^KENAR;
            stk->data = (Elem*)(mem_block + sizeof(KENAR));
        }
        #else
            stk->data = (Elem*)(mem_block);
        #endif

        #if (PROTECTION_LEVEL & HASH_PROTECTION)
            stk->data_hash = GetHash(stk->data, stk->capacity*sizeof(Elem));
        #endif
    }
    stk->size = 0;

    stk->debug = {};
    strcpy_(stk->debug.file, file);
    strcpy_(stk->debug.function, function);
    strcpy_(stk->debug.name, name);
    stk->debug.line   = line;
    stk->debug.status = true;

    #if (PROTECTION_LEVEL & CANARY_PROTECTION) 
        stk->struct_hash = GetHash(stk, sizeof(stk));
    #endif

    OK_ASSERT(*stk);
    return error;
}

#define StackCtor(stk, capacity) StackConstructor(stk, capacity, __LINE__, __PRETTY_FUNCTION__, __FILE__, #stk)

size_t StackDtor(Stack* stk)
{
    OK_ASSERT(*stk);
    stk->capacity    = POISON - 1;
    stk->size        = POISON;

    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        free((char*)stk->data - sizeof(KENAR));
    #else
        free(stk->data);
    #endif
    stk->data        = (Elem*)POISON_PTR;

    stk->data_hash   = 0;
    stk->struct_hash = 0;

    stk->debug.status = 0;

    return NO_ERROR;
}

size_t StackResizeUp(Stack* stk)
{
    OK_ASSERT(*stk);

    if (stk->capacity == 0)
    {
        stk->capacity = 10;

        size_t new_capacity = stk->capacity*sizeof(Elem);
        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
            new_capacity += sizeof(KENAR)*2;
        #endif

        char* mem_block = (char*)calloc(new_capacity, 1);

        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        {
            *(size_t*)mem_block = KENAR;
            *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = (size_t)(-1)^KENAR;
            stk->data = (Elem*)(mem_block + sizeof(KENAR));
        }
        #else
            stk->data = (Elem*)(mem_block);
        #endif

        if (stk->data == nullptr)
        {
            OK_ASSERT(*stk);
            return MEMORY_ALLOCATION_ERROR;
        }

        OK_ASSERT(*stk);
        return NO_ERROR;
    }

    stk->capacity = stk->capacity * FOR_RESIZE;
    size_t new_capacity = stk->capacity*sizeof(Elem);
    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        new_capacity += sizeof(KENAR)*2;
    #endif

    char* mem_block = nullptr;
    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
    {
        mem_block = (char*)realloc((char*)stk->data - sizeof(KENAR), new_capacity);
        if (mem_block == nullptr)
            return MEMORY_ALLOCATION_ERROR;
        
        *(size_t*)mem_block = KENAR;
        *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = (size_t)(-1)^KENAR;
        stk->data = (Elem*)(mem_block + sizeof(KENAR));
    }
    #else
    {
        mem_block = (char*)realloc((char*)stk->data, new_capacity);
        if (mem_block == nullptr)
        {
            stk->data = nullptr;
            return MEMORY_ALLOCATION_ERROR;
        }
        stk->data = (Elem*)(mem_block);
    }
    #endif

    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    
    OK_ASSERT(*stk);
    return NO_ERROR;
}

size_t StackPush(Stack* stk, Elem value)
{
    OK_ASSERT(*stk);

    if (stk->capacity == stk->size)
    {
        size_t error = 0;
        if ((error = StackResizeUp(stk)))
        {   
            OK_ASSERT(*stk);
            return error;
        }
    }
    stk->data[stk->size++] = value;

    #if (PROTECTION_LEVEL & HASH_PROTECTION)
    {
        stk->data_hash = GetHash(stk->data, sizeof(Elem)*stk->capacity);

        stk->struct_hash = 0;
        stk->struct_hash = GetHash(stk, sizeof(Stack));
    }
    #endif

    OK_ASSERT(*stk);
    return NO_ERROR;
}

size_t StackResizeDown(Stack* stk)
{
    OK_ASSERT(*stk);

    if (stk->capacity == 0)
        return NO_ERROR;

    if (stk->capacity/(double)stk->size >= FOR_RESIZE*FOR_RESIZE)
    {
        stk->capacity = stk->capacity / FOR_RESIZE;

        size_t new_capacity = stk->capacity*sizeof(Elem);
        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        {
            new_capacity += sizeof(KENAR)*2;
            char* mem_block = (char*)realloc((char*)stk->data - sizeof(KENAR), new_capacity);
            if (mem_block == nullptr)
            {
                stk->data = nullptr;
                return MEMORY_ALLOCATION_ERROR;
            }

            *(size_t*)mem_block = KENAR;
            *(size_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(KENAR)) = (size_t)(-1)^KENAR;
            stk->data = (Elem*)(mem_block + sizeof(KENAR));
        }
        #else
            stk->data = (Elem*)realloc(stk->data, new_capacity);
        #endif
    }
    
    OK_ASSERT(*stk);
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    return NO_ERROR;
}

Elem StackPop(Stack* stk, size_t *err = nullptr)
{
    OK_ASSERT(*stk);

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
        {
            OK_ASSERT(*stk);
            return POISON;
        }

        #if (PROTECTION_LEVEL & HASH_PROTECTION)
        {
            stk->data_hash = GetHash(stk->data, stk->capacity*sizeof(Elem));
            stk->struct_hash = 0;
            stk->struct_hash = GetHash(stk, sizeof(Stack));
        }
        #endif
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