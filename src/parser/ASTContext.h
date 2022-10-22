#ifndef ASTCONTEXT_H
#define ASTCONTEXT_H

#include <stdlib.h>
#include "token.h"

struct ASTContext {
  struct TokenArray* tokens;
  size_t index;
};

struct ASTContext newASTContext(struct TokenArray*);

#endif
