#ifndef TYPEAST_H
#define TYPEAST_H

#include "expressionAST.h"

struct TypeAST;

// used for "int8" or "int8*", depends on its label in the TypeAST
enum BaseTypes {
  BASETYPE_UNDEFINED,
  BASETYPE_INT8,
  BASETYPE_INT16,
  BASETYPE_INT32,
  BASETYPE_INT64,
  BASETYPE_ISIZE,
  BASETYPE_UINT8,
  BASETYPE_UINT16,
  BASETYPE_UINT32,
  BASETYPE_UINT64,
  BASETYPE_USIZE,
  BASETYPE_FLOAT8, // this has got to be the funniest data type i've heard of
  BASETYPE_FLOAT16,
  BASETYPE_FLOAT32,
  BASETYPE_FLOAT63,
  BASETYPE_FSIZE,
  BASETYPE_BYTE,
  BASETYPE_CHAR,
  BASETYPE_VOID,
  BASETYPE_BOOL,
};

// int8[34 + 35]
struct ArrayType {
  struct TypeAST* type;
  struct ExprAST* size;
};

// struct Test
struct StructType {
  const char* name;
};

enum TypeASTTypes {
  TYPE_UNDEFINED,
  TYPE_BASE,
  TYPE_ARRAY,
  TYPE_PTR,
  TYPE_STRUCT
};

struct TypeAST {
  enum TypeASTTypes type;
  union {
    enum BaseTypes base; // the first two are differentiated despite being
    enum BaseTypes pointer; // the same thing
    struct ArrayType array;
    struct StructType structure;
  } as;
};

struct TypeAST* allocNewBaseType(enum BaseTypes);
struct TypeAST* allocNewPointerType(enum BaseTypes);
struct TypeAST* allocNewArrayType(struct TypeAST*, struct ExprAST*);
struct TypeAST* allocNewStructType(const char*);

void freeTypeAST(struct TypeAST*);

void printTypeAST(const struct TypeAST*);

#endif
