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
  ARG_ASSIGN,
  ARG_SUMARG,
  ARG_SUMARGTYPE,
  ARG_GENERIC,
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
  struct ExprAST* rval;
};

enum SumArgTypes {
  SUMARG_UNDEFINED,
  SUMARG_IDENTIFIER,
  SUMARG_TYPE
};

struct SumArgType {
  enum SumArgTypes type;
  union {
    struct TypeAST* type;
    const char* identifier;
  } as;
};

struct SumArg {
  const char* name;
  struct ArgAST* fields;
};

DEFINE_ARRAYLIST(IdentifierArgList, struct IdentifierArg*);
DEFINE_ARRAYLIST(AssignArgList, struct AssignArg*);
DEFINE_ARRAYLIST(GenericList, const char*);
DEFINE_ARRAYLIST(SumArgList, struct SumArg*);
DEFINE_ARRAYLIST(SumArgTypeList, struct SumArgType*);

struct ArgAST {
  enum ArgASTType type;
  union {
    struct ExprArgList exprargs;
    struct IdentifierArgList idargs;
    struct AssignArgList assignargs;
    struct GenericList generics;
    struct SumArgList sums;
    struct SumArgTypeList sumtypes;
  } as;
};

struct IdentifierArg* allocNewIdentifierArg(const char*, struct TypeAST*);
struct AssignArg* allocNewAssignArg(const char*, struct ExprAST*);
struct SumArg* allocNewSumArg(const char*, struct ArgAST*);
struct SumArgType* allocNewSumArgTypeType(struct TypeAST*);
struct SumArgType* allocNewSumArgTypeStr(const char*);

struct ArgAST* allocNewExprArgList(struct ExprList* args);
struct ArgAST* allocNewIdentifierArgList(void);
struct ArgAST* allocNewAssignArgList(void);
struct ArgAST* allocNewGenericList(void);
struct ArgAST* allocNewSumArgList(void);
struct ArgAST* allocNewSumArgTypeList(void);

void freeArgAST(struct ArgAST*);
void printArgAST(const struct ArgAST*);

#endif
