#ifndef _STRINGS_SYM_
#define _STRINGS_SYM_

const int EOF_RETURN = -1; 

int   strlen_(const char *str);

int   strnlen_(char *str, size_t maxLen);

char *strcpy_(char *to, const char* from);

char *strncpy_(char *to, const char* from, size_t n);

char *strcat_(char *to, const char *from);

char *strncat_(char *to, char* from, size_t n);

int  strcmp_(void *str1, void *str2);

int  strcmp_reverse(void *str1, void *str2);

char  strncmp_(const char * s1, const char* s2, size_t n);

char *strchr_(const char * str, char c);

char *strnchr_(char *str, char c, size_t maxLen);

char *strdup_(const char *str);

void  puts_(const char *str, FILE *fp);

int   getline_(char *line, size_t max, FILE *fp);

bool  is_russian_letter_vowel(char c);

int   number_syllables(const char * str);

#endif