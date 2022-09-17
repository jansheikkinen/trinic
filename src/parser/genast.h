#ifndef GENAST_H
#define GENAST_H

#include "ast.h"

struct ASTContext {
  struct TokenArray* tokens;
  size_t index;
};

struct ASTContext* newASTContext(struct TokenArray*);
void freeASTContext(struct ASTContext*);

struct AST* generateAST(struct TokenArray*);


#endif
