/* typeAST.c */

#include <stdio.h>

#include "token.h"
#include "typeAST.h"

struct TypeAST* allocNewBaseType(enum BaseTypes type) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_BASE;
  ast->as.base = type;

  return ast;
}

struct TypeAST* allocNewPointerType(enum BaseTypes type) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_PTR;
  ast->as.pointer = type;

  return ast;
}

struct TypeAST* allocNewArrayType(struct TypeAST* type, struct ExprAST* size) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_ARRAY;
  ast->as.array.type = type;
  ast->as.array.size = size;

  return ast;

}

struct TypeAST* allocNewStructType(const char* name, enum StructTypes type) {
  struct TypeAST* ast = malloc(sizeof(*ast));

  ast->type = TYPE_STRUCT;
  ast->as.structure.name = name;
  ast->as.structure.type = type;

  return ast;
}

void freeTypeAST(struct TypeAST* ast) {
  switch(ast->type) {
    case TYPE_UNDEFINED:
    case TYPE_BASE:
    case TYPE_STRUCT:
    case TYPE_PTR: break;
    case TYPE_ARRAY:
      freeTypeAST(ast->as.array.type);
      freeExprNode(ast->as.array.size); break;
  }

  free(ast);
}

void printTypeAST(const struct TypeAST* ast) {
  switch(ast->type) {
    case TYPE_UNDEFINED:
      printf("(UNDEFINED TYPE)"); break;
    case TYPE_BASE:
      printf("%s", getTokenName(TOKEN_FALSE + ast->as.base)); break;
    case TYPE_PTR:
      printf("%s*", getTokenName(TOKEN_FALSE + ast->as.pointer)); break;
    case TYPE_ARRAY:
      printTypeAST(ast->as.array.type);
      printf("["); printExprAST(ast->as.array.size); printf("]"); break;
    case TYPE_STRUCT:
      switch(ast->as.structure.type) {
        case STRUCT_UNDEFINED: printf("UNDEFINED STRUCT"); break;
        case STRUCT_STRUCT:
          printf("(struct %s)", ast->as.structure.name); break;
        case STRUCT_UNION:
          printf("(union %s)", ast->as.structure.name); break;
        case STRUCT_ENUM:
          printf("(enum %s)", ast->as.structure.name); break;
        case STRUCT_SUM:
          printf("(sum %s)", ast->as.structure.name); break;
        case STRUCT_INTERFACE:
          printf("(interface %s)", ast->as.structure.name); break;
      }
  }
}
