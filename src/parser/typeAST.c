/* typeAST.c */

#include <stdio.h>

#include "argumentAST.h"
#include "token.h"
#include "typeAST.h"

struct TypeAST* allocNewBaseType(enum BaseTypes type, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_BASE;
  ast->ismutable = mutable;
  ast->as.base.type = BTT_TYPE;
  ast->as.base.as.type = type;

  return ast;
}

struct TypeAST* allocNewBaseTypeStr(const char* name, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_BASE;
  ast->ismutable = mutable;
  ast->as.base.type = BTT_IDENTIFIER;
  ast->as.base.as.identifier = name;

  return ast;
}

struct TypeAST* allocNewVariadicType(struct TypeAST* base, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_VARIADIC;
  ast->ismutable = mutable;
  ast->as.variadic.type = base;

  return ast;
}

struct TypeAST* allocNewPointerType(struct TypeAST* base, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_PTR;
  ast->ismutable = mutable;
  ast->as.pointer.type = base;

  return ast;
}

struct TypeAST* allocNewArrayType(struct TypeAST* type, struct ExprAST* size,
    bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_ARRAY;
  ast->ismutable = mutable;
  ast->as.array.type = type;
  ast->as.array.size = size;

  return ast;

}

struct TypeAST* allocNewStructType(const char* name, enum StructTypes type,
    struct ArgAST* generics, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_STRUCT;
  ast->ismutable = mutable;
  ast->as.structure.name = name;
  ast->as.structure.type = type;
  ast->as.structure.generics = generics;

  return ast;
}

struct TypeAST* allocNewFunctionType(struct ArgAST* params,
    struct TypeAST* returns, struct ArgAST* generics, bool mutable) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_FUNCTION;
  ast->ismutable = mutable;
  ast->as.function.params = params;
  ast->as.function.returns = returns;
  ast->as.function.generics = generics;

  return ast;
}

void freeTypeAST(struct TypeAST* ast) {
  switch(ast->type) {
    case TYPE_UNDEFINED:
    case TYPE_BASE: break;
    case TYPE_VARIADIC:
      freeTypeAST(ast->as.variadic.type); break;
    case TYPE_STRUCT:
      if(ast->as.structure.generics) freeArgAST(ast->as.structure.generics);
      break;
    case TYPE_PTR: freeTypeAST(ast->as.pointer.type); break;
    case TYPE_ARRAY:
      freeTypeAST(ast->as.array.type);
      if(ast->as.array.size) freeExprNode(ast->as.array.size);
      break;
    case TYPE_FUNCTION:
      if(ast->as.function.params) freeArgAST(ast->as.function.params);
      if(ast->as.function.returns) freeTypeAST(ast->as.function.returns);
      if(ast->as.function.generics) freeArgAST(ast->as.function.generics);
  }

  free(ast);
}

void printTypeAST(const struct TypeAST* ast) {
  if(ast->ismutable) printf("mut ");
  switch(ast->type) {
    case TYPE_UNDEFINED:
      printf("(UNDEFINED TYPE)"); break;
    case TYPE_BASE:
      switch(ast->as.base.type) {
        case BTT_UNDEFINED:
          printf("UNDEFINED BASE TYPE"); break;
        case BTT_IDENTIFIER:
          printf("%s", ast->as.base.as.identifier); break;
        case BTT_TYPE:
          printf("%s", getTokenName(TOKEN_FALSE + ast->as.base.as.type)); break;
      } break;
    case TYPE_VARIADIC:
      printf(".."); printTypeAST(ast->as.variadic.type); break;
    case TYPE_PTR:
      printf("*");
      printTypeAST(ast->as.pointer.type);
      break;
    case TYPE_ARRAY:
      printf("[");
      if(ast->as.array.size) printExprAST(ast->as.array.size);
      printf("]");
      printTypeAST(ast->as.array.type); break;
    case TYPE_STRUCT:
      switch(ast->as.structure.type) {
        case STRUCT_UNDEFINED: printf("UNDEFINED STRUCT"); break;
        case STRUCT_STRUCT:
          printf("struct %s", ast->as.structure.name);
          if(ast->as.structure.generics) {
            printf("(");
            printArgAST(ast->as.structure.generics);
            printf(")");
          }
          break;
        case STRUCT_UNION:
          printf("union %s", ast->as.structure.name);
          if(ast->as.structure.generics) {
            printf("(");
            printArgAST(ast->as.structure.generics);
            printf(")");
          }
          break;
        case STRUCT_ENUM:
          printf("enum %s", ast->as.structure.name);
          if(ast->as.structure.generics) {
            printf("(");
            printArgAST(ast->as.structure.generics);
            printf(")");
          }
          break;
        case STRUCT_SUM:
          printf("sum %s", ast->as.structure.name);
          if(ast->as.structure.generics) {
            printf("(");
            printArgAST(ast->as.structure.generics);
            printf(")");
          }
          break;
        case STRUCT_INTERFACE:
          printf("trait %s", ast->as.structure.name); break;
      }
      break;
    case TYPE_FUNCTION:
      printf("(");
      if(ast->as.function.params) printArgAST(ast->as.function.params);
      else printf("void");
      printf(") -> ");
      if(ast->as.function.returns) printTypeAST(ast->as.function.returns);
      else printf("void");
      if(ast->as.function.generics) {
        printf(" for "); printArgAST(ast->as.function.generics);
      }
  }
}
