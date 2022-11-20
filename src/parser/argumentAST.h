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
  ARG_GENERIC_DEFL,
  ARG_GENERIC_DEFR,
  ARG_GENERIC_DEFS
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

struct GenericDef {
  struct ArgAST* lvalue;
  struct ArgAST* rvalue;
};

// x, y: int8, z: isize
DEFINE_ARRAYLIST(IdentifierArgList, struct IdentifierArg*);
// TEST = 3 + 4, TEST2 = 3 + 5, TEST3
DEFINE_ARRAYLIST(AssignArgList, struct AssignArg*);
// T, V
DEFINE_ARRAYLIST(GenericList, const char*);
// TEST(T, int8), TEST2
DEFINE_ARRAYLIST(SumArgList, struct SumArg*);
// T, int8
DEFINE_ARRAYLIST(SumArgTypeList, struct SumArgType*);
// T + V
DEFINE_ARRAYLIST(GenericDefLeft, const char*);
// interface Divide + interface Display
DEFINE_ARRAYLIST(GenericDefRight, struct TypeAST*);
// T + V: interface Divide + interface Display
// T + V: interface Divide + interface Display, Q: interface Add
DEFINE_ARRAYLIST(GenericDefs, struct GenericDef*);

struct ArgAST {
  enum ArgASTType type;
  union {
    struct ExprArgList exprargs;
    struct IdentifierArgList idargs;
    struct AssignArgList assignargs;
    struct GenericList generics;
    struct SumArgList sums;
    struct SumArgTypeList sumtypes;
    struct GenericDefLeft genleft;
    struct GenericDefRight genright;
    struct GenericDefs gendefs;
  } as;
};

struct IdentifierArg* allocNewIdentifierArg(const char*, struct TypeAST*);
struct AssignArg* allocNewAssignArg(const char*, struct ExprAST*);
struct SumArg* allocNewSumArg(const char*, struct ArgAST*);
struct SumArgType* allocNewSumArgTypeType(struct TypeAST*);
struct SumArgType* allocNewSumArgTypeStr(const char*);
struct GenericDef* allocNewGenericDef(struct ArgAST*, struct ArgAST*);

struct ArgAST* allocNewExprArgList(struct ExprList* args);
struct ArgAST* allocNewIdentifierArgList(void);
struct ArgAST* allocNewAssignArgList(void);
struct ArgAST* allocNewGenericList(void);
struct ArgAST* allocNewSumArgList(void);
struct ArgAST* allocNewSumArgTypeList(void);
struct ArgAST* allocNewGenericDefLeft(void);
struct ArgAST* allocNewGenericDefRight(void);
struct ArgAST* allocNewGenericDefs(void);

void freeArgAST(struct ArgAST*);
void printArgAST(const struct ArgAST*);

#endif
