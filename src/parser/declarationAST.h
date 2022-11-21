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
  DECLARATION_FUNCTION,
  DECLARATION_VARIABLE,
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

struct DeclarationAST {
  enum DeclarationTypes type;
  const char* name;
  union {
    struct StructureAST structure;
    struct DeclarationList* interface;
    struct FunctionAST function;
    struct StmtAST* vardecl;
  } as;
  struct ArgAST* fields;
};

struct DeclarationAST* allocNewStruct(const char*, struct ArgAST*,
    struct ArgAST*);
struct DeclarationAST* allocNewUnion(const char*, struct ArgAST*,
    struct ArgAST*);
struct DeclarationAST* allocNewEnum(const char*, struct ArgAST*,
    struct ArgAST*);
struct DeclarationAST* allocNewSum(const char*, struct ArgAST*,
    struct ArgAST*);
struct DeclarationAST* allocNewInterface(const char*, struct DeclarationList*);
struct DeclarationAST* allocNewVarDeclDecl(struct StmtAST*);

struct DeclarationAST* allocNewFunction(const char*, struct ArgAST*,
    struct TypeAST*, struct ArgAST*, struct ArgAST*, struct StmtList*);

void freeDeclarationAST(struct DeclarationAST*);

void printDeclarationAST(const struct DeclarationAST*);

#endif
