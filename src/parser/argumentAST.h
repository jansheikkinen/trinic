#ifndef ARGUMENT_AST_H
#define ARGUMENT_AST_H

#include <stdlib.h>
#include "../util/arraylist.h"
#include "typeAST.h"
#include "exprlist.h"

enum ArgASTType {
  ARG_UNDEFINED,
  ARG_EXPR,
  ARG_IDENTIFIER,
  ARG_ASSIGN
};

struct IdentifierArg {
  const char* identifier;
  struct TypeAST* type;
};

struct ExprArgList {
  struct ExprList* args;
};

struct AssignArg {
  const char* identifier;
  long value;
};

DEFINE_ARRAYLIST(IdentifierArgList, struct IdentifierArg*);
DEFINE_ARRAYLIST(AssignArgList, struct AssignArg*);

struct ArgAST {
  enum ArgASTType type;
  union {
    struct ExprArgList exprargs;
    struct IdentifierArgList idargs;
    struct AssignArgList assignargs;
  } as;
};

struct ArgAST* allocNewExprArgList(struct ExprList* args);
struct IdentifierArg* allocNewIdentifierArg(const char*, struct TypeAST*);
struct ArgAST* allocNewIdentifierArgList(void);
struct AssignArg* allocNewAssignArg(const char*, long);
struct ArgAST* allocNewAssignArgList(void);
void freeArgAST(struct ArgAST*);
void printArgAST(const struct ArgAST*);

#endif
