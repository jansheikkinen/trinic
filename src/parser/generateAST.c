/* generateAST.c */

#include <stdio.h>

#include "ASTContext.h"
#include "generateDeclarationAST.h"
#include "generateAST.h"

struct DeclarationList* generateAST(const char* filename,
    struct TokenArray* tokens) {
  struct ASTContext* ctx = malloc(sizeof(*ctx));
  *ctx = newASTContext(tokens);

  struct DeclarationList* decls = malloc(sizeof(*decls));
  NEW_ARRAYLIST(decls);

  while(ctx->index < ctx->tokens->length) {
    APPEND_ARRAYLIST(decls, generateDeclaration(ctx));
  }

  struct ASTErrorList* errors = ctx->errors;
  if(errors->size > 0) {
    for(size_t i = 0; i < errors->size; i++) {
      printASTError(filename, &errors->errors[i]);
    }

    exit(1);
  }

  free(errors);
  free(ctx);
  return decls;
}
