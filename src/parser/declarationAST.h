#ifndef DECLARATION_AST_H
#define DECLARATION_AST_H

#include "argumentAST.h"

enum DeclarationTypes {
  DECLARATION_UNDEFINED,
  DECLARATION_STRUCT,
  DECLARATION_UNION,
  DECLARATION_ENUM,
  DECLARATION_SUM,
  DECLARATION_INTERFACE,
  DECLARATION_FUNCTION,
};

// Includes struct, union, enum, sum, interface
struct StructureAST {
  struct ArgAST* fields;
};

struct FunctionAST {
  struct ArgAST* args;
  struct TypeAST* returns;
  struct ArgAST* contracts;
  struct ArgAST* generics;
};

struct DeclarationAST {
  enum DeclarationTypes type;
  const char* name;
  union {
    struct StructureAST structure;
    struct FunctionAST function;
  } as;
  struct ArgAST* fields;
};

struct DeclarationAST* allocNewStruct(const char*, struct ArgAST*);
struct DeclarationAST* allocNewUnion(const char*, struct ArgAST*);
struct DeclarationAST* allocNewEnum(const char*, struct ArgAST*);
struct DeclarationAST* allocNewSum(const char*, struct ArgAST*);
struct DeclarationAST* allocNewInterface(const char*, struct ArgAST*);

struct DeclarationAST* allocNewFunction(const char*, struct ArgAST*,
    struct TypeAST*, struct ArgAST*, struct ArgAST*);

void freeDeclarationAST(struct DeclarationAST*);

void printDeclarationAST(const struct DeclarationAST*);

#endif
