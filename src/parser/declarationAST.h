#ifndef DECLARATION_AST_H
#define DECLARATION_AST_H

#include "stmtlist.h"
#include "argumentAST.h"
#include "statementAST.h"

enum DeclarationTypes {
  DECLARATION_UNDEFINED,
  DECLARATION_STRUCT,
  DECLARATION_UNION,
  DECLARATION_ENUM,
  DECLARATION_SUM,
  DECLARATION_INTERFACE,
  DECLARATION_IMPL,
  DECLARATION_FUNCTION,
  DECLARATION_VARIABLE,
  DECLARATION_INCLUDE
};

// Includes struct, union, enum, sum, interface
struct StructureAST {
  struct ArgAST* fields;
  struct ArgAST* generics;
};

struct FunctionAST {
  struct ArgAST* args;
  struct TypeAST* returns;
  struct ArgAST* contracts;
  struct ArgAST* generics;
  struct StmtList* body;
};

DEFINE_ARRAYLIST(DeclarationList, struct DeclarationAST*);

struct ImplAST {
  struct TypeAST* trait;
  struct TypeAST* type;
  struct DeclarationList* body;
};

struct DeclarationAST {
  enum DeclarationTypes type;
  const char* name;
  bool isexternal;
  union {
    struct StructureAST structure;
    struct DeclarationList* interface;
    struct ImplAST impl;
    struct FunctionAST function;
    struct StmtAST* vardecl;
  } as;
  struct ArgAST* fields;
};

struct DeclarationAST* allocNewStruct(const char*, struct ArgAST*,
    struct ArgAST*, bool);
struct DeclarationAST* allocNewUnion(const char*, struct ArgAST*,
    struct ArgAST*, bool);
struct DeclarationAST* allocNewEnum(const char*, struct ArgAST*,
    struct ArgAST*, bool);
struct DeclarationAST* allocNewSum(const char*, struct ArgAST*,
    struct ArgAST*, bool);
struct DeclarationAST* allocNewInterface(const char*, struct DeclarationList*,
    bool);
struct DeclarationAST* allocNewImpl(struct TypeAST*, struct TypeAST*,
    struct DeclarationList*, bool);
struct DeclarationAST* allocNewVarDeclDecl(struct StmtAST*, bool);
struct DeclarationAST* allocNewFunction(const char*, struct ArgAST*,
    struct TypeAST*, struct ArgAST*, struct ArgAST*, struct StmtList*, bool);
struct DeclarationAST* allocNewInclude(const char*);

void freeDeclarationAST(struct DeclarationAST*);

void printDeclarationAST(const struct DeclarationAST*, size_t);

#endif
