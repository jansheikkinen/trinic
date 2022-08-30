#ifndef STRUTIL_H
#define STRUTIL_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* substring(const char* __restrict__, size_t, size_t);

int streq(const char* s1, const char* s2);

char* strlower(char* str);

#endif
