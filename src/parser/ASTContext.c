/* ASTContext.c */

#include "../util/reallocarray.h"
#include "ASTContext.h"

static void attachASTErrorList(struct ASTContext* ctx) {
  struct ASTErrorList* errors = malloc(sizeof(*errors));

  errors->capacity = 4;
  errors->size = 0;

  errors->errors = calloc(errors->capacity, sizeof(*errors->errors));

  ctx->errors = errors;
}

void appendASTError(struct ASTContext* ctx, struct ASTError error) {
  struct ASTErrorList* errors = ctx->errors;

  if(errors->capacity >= errors->size) {
    errors->capacity *= 1.5;

    errors->errors =
      reallocarray(errors->errors, errors->capacity, sizeof(*errors->errors));
  }

  errors->errors[errors->size++] = error;
}

struct ASTContext newASTContext(struct TokenArray* tokens) {
  struct ASTContext ctx;

  attachASTErrorList(&ctx);
  ctx.tokens = tokens;
  ctx.index = 0;

  return ctx;
}
