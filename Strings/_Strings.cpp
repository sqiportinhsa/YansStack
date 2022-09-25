#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "_Strings.h"
/*!
* \file
* \brief 
* This file contains implementation of standard string functions
*/
int strlen_(const char *str) 
{
    assert(str != nullptr);

    int n = 0;
    while (str[n] != '\0')
        n++;

    return n;
}

char *strcpy_(char *to, const char *from) 
{
    assert(to   != nullptr);
    assert(from != nullptr);
    
    int n = 0;
    while (from[n] != '\0')
    {
        to[n] = from[n];
        n++;
    }
    to[n] = '\0';

    return to;
}

char *strcat_(char *to, const char *from) 
{
    assert(to   != nullptr);
    assert(from != nullptr);

	int to_len = strlen_(to);
    int i = 0;
    while (from[i] != '\0')
    {    
		to[to_len + i] = from[i];
        i++;
	}
    to[to_len + i] = '\0';

    return to;
}

char *strchr_(const char * str, char c) 
{
    assert(str != nullptr);
    
    int i = 0;
    char * answer = nullptr;

    while (str[i] != '\0')
    {
        if (str[i] == c)
            return (char *)(str + i);
        i++;
    }

    return nullptr;
}

int strnlen_(char *str, size_t maxLen) 
{
    assert(str != nullptr);
    
    int n = 0;
    while (str[n] != '\0' && n < maxLen)
        n++;

    return n;
}

char *strncpy_(char *to,const char* from, size_t n) 
{
    assert(to   != nullptr);
    assert(from != nullptr);
    
    int index = 0;
    while (from[index] != '\0' && index < n) 
    {
        to[index] = from[index];
        n++;
    }
    to[index] = '\0';

    return to;
}

char *strncat_(char *to, char *from, size_t n) 
{
    assert(to   != nullptr);
    assert(from != nullptr);
    
    if (n < 0)
        n = 0;

    int len_to = strlen_(to);

    int index = 0;
    while (from[index] != '\0' && index < n)
    {
        to[len_to + index] = from[index];
        n++;
    }
    to[len_to + n] = '\0';

    return to;
}

char *strnchr_(char *str, char c, size_t maxLen) 
{
    assert(str != nullptr);
    
    int i = 0;
    char* answer;
    while (i < maxLen && str[i] != '\0')
    {
        if (str[i] == c)
            return (str + i);
        i++;
    }    

    return nullptr;
}

char *strdup_(const char *str)
{
    assert(str != nullptr);
    
    int str_len = strlen_((char*)str);    
    char *duplicate = (char *)calloc(str_len + 1, sizeof(char));

    for(int i = 0; i < str_len; i++)
        duplicate[i] = str[i];

    duplicate[str_len] = '\0';

    return duplicate;
}
//!------------------------
//!Function works like stundard, but takes into account only letters and numbers 
//!
//!-------------------------
int strcmp_(void *str1, void *str2)
{
    assert(str1 != nullptr);
    assert(str2 != nullptr);
    
    const char * s1 = *(const char**)str1; 
    const char * s2 = *(const char**)str2;

    int n1 = 0;
    int n2 = 0;
    while (s1[n1] != '\0' && s2[n2] != '\0')
    {
        if(!(isalnum((unsigned char)s1[n1])))
        {
            n1++;
            continue;
        }
        if(!(isalnum((unsigned char)s2[n2])))
        {
            n2++;
            continue;
        }
        
        if (s1[n1] != s2[n2])
            return (int)s1[n1] - (int)s2[n2];

        n1++;
        n2++;
    }

    while (!isalnum((unsigned char)s1[n1]) && s1[n1] != '\0')
        n1++;
    while (!isalnum((unsigned char)s2[n1]) && s2[n2] != '\0')
        n2++;
    
    return (int)s1[n1] - (int)s2[n2];
}

//!------------------------
//!Function works like _strcmp, but compares strings from the end
//!
//!-------------------------
int strcmp_reverse(void *str1, void *str2)
{
    assert(str1 != nullptr);
    assert(str2 != nullptr);

    const char *s1 = *(const char**)str1;
    const char *s2 = *(const char**)str2;
    
    const char *s1_start = s1;
    const char *s2_start = s2;

    while (*s1 != '\0')
        s1++;
    while (*s2 != '\0')
        s2++;
    
    while (s1 >= s1_start && s2 >= s2_start)
    {
        if(!isalnum((unsigned char)(*s1)))
        {
            s1--;
            continue;
        }
        if(!isalnum((unsigned char)(*s2)))
        {
            s2--;
            continue;
        }

        if (*s1 != *s2)
            return *s1 - *s2;

        s1--;
        s2--;
    }
    
    while (!isalnum((unsigned char)(*s1)) && s1 >= s1_start)
        s1--;
    while (!isalnum((unsigned char)(*s2)) && s2 >= s2_start)
        s2--;

    if (s1 < s1_start && 
        s2 < s2_start)
        return *s1 - *s2;
    else if (s1 == s1_start)
        return -1;

    return 1;
}


char  strncmp_(const char * s1, const char* s2, size_t n)
{
    assert(s1 != nullptr);
    assert(s2 != nullptr);

    int index = 0;

    while (s1[index] && s2[index] && index < n - 1)
    {
        if (s1[index] > s2[index])
            return 1;
        if (s1[index] < s2[index])
            return -1;
        
        index++;
    }    

    return s1[index] - s2[index];
}

void puts_(const char *str, FILE *fp)
{   
    assert(str != nullptr);
    assert(fp  != nullptr);
    
    if (str == NULL)
    {
        puts_("(null)", fp);
        return;
    }
    assert(fp != NULL);

    int i = 0;
    while(str[i] != '\0')
    {
        putc(str[i], fp);
        i++;
    }
    putc('\n', fp);
}

int getline_(char *line, size_t max, FILE* fp)
{
    assert(line != nullptr);
    assert(fp   != nullptr);

    int n = 0;
    int c = 0;
    while ((c = getc(fp)) != '\n' && c != EOF  && n < max)
    {
        line[n] = c;
        n++;
    }
    
    line[n] = '\0';
    if (c == EOF)
        return EOF_RETURN;
    return n;
}

//!------------------------
//!Function return true if c - russian letter and false otherwise
//!
//!-------------------------
bool is_russian_letter_vowel(char c)
{
    return (c == 'à' ||
            c == 'å' ||
            c == '¸' ||
            c == 'è' ||
            c == 'î' ||
            c == 'ó' ||
            c == 'û' ||
            c == 'ý' ||
            c == 'þ' ||
            c == 'ÿ' ||
            c == 'À' ||
            c == 'Å' ||
            c == '¨' ||
            c == 'È' ||
            c == 'Î' ||
            c == 'Ó' ||
            c == 'Û' ||
            c == 'Ý' ||
            c == 'Þ' ||
            c == 'ß');
}

//!------------------------
//!Function return true number of syllables in the str
//!
//!-------------------------
int number_syllables(const char *str)
{
    assert(str != nullptr);
    
    assert(str != nullptr);
    int number = 0;

    while (*str != '\0')
    {
        if (is_russian_letter_vowel(*str))
            number++;
        str++;
    }

    return number;
}