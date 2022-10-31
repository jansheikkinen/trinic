#ifndef ASTCONTEXT_H
#define ASTCONTEXT_H

#include <stdlib.h>
#include "token.h"
#include "../error/error.h"

struct ASTErrorList {
  struct ASTError* errors;
  size_t size;
  size_t capacity;
};

struct ASTContext {
  const char* filename;
  struct ASTErrorList* errors;
  struct TokenArray* tokens;
  size_t index;
};

struct ASTContext newASTContext(struct TokenArray*);

void appendASTError(struct ASTContext*, struct ASTError);

#define APPEND_ASTERROR(ctx, err)                                \
  appendASTError(ctx,                                            \
      newASTError(                                               \
        err, getTokenName(GET_CURRENT_TOKEN(ctx).type),          \
        GET_CURRENT_TOKEN(ctx).row, GET_CURRENT_TOKEN(ctx).col))

#endif
