/* strutil.c */

#include "strutil.h"

char* substring(const char* __restrict__ string, size_t start, size_t offset) {
  char* str = calloc(offset + 1, sizeof(*str));
  return strncpy(str, string + start, offset);
}

int streq(const char* s1, const char* s2) {
  return strcmp(s1, s2) == 0;
}

char* strlower(char* str) {
  for(size_t i = 0; i < strlen(str); i++) {
    str[i] = tolower(str[i]);
  }

  return str;
}
