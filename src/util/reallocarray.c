/* reallocarray.c */

#include "reallocarray.h"

// bc there's a man page entry for this but no definition >:(
// this is not up to standard
void* reallocarray(void* ptr, size_t nmemb, size_t size) {
  return realloc(ptr, nmemb * size);
}
