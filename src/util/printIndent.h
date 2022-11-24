#ifndef PRINT_INDENT_H
#define PRINT_INDENT_H

#include <stdlib.h>
#include <stdio.h>

#define PRINT_INDENT(indent) \
  for(size_t i = 0; i < indent; i++) { \
    printf("  "); \
  } \


#endif
