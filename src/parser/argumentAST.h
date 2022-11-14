#ifndef ARGUMENT_AST_H
#define ARGUMENT_AST_H

#include <stdlib.h>
#include "../util/arraylist.h"
#include "exprlist.h"

enum ArgASTType {
  ARG_UNDEFINED,
  ARG_EXPR,
  ARG_IDENTIFIER,
};

struct IdentifierArg {
  // type info
  const char* identifier;
};

struct ExprArgList {
  struct ExprList* args;
};

DEFINE_ARRAYLIST(IdentifierArgList, struct IdentifierArg*);

struct ArgAST {
  enum ArgASTType type;
  union {
    struct ExprArgList exprargs;
    struct IdentifierArgList idargs;
  } as;
};

#endif
