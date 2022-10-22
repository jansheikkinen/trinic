/* ASTContext.c */

#include "ASTContext.h"

struct ASTContext newASTContext(struct TokenArray* tokens) {
  struct ASTContext ctx;
  ctx.tokens = tokens;
  ctx.index = 0;

  return ctx;
}
