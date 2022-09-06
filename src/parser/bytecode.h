#ifndef BYTECODE_H
#define BYTECODE_H

#include <stddef.h>

enum OpCode {
  OP_UNDEFINED,
  OP_SET,

  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_UNM,

  OP_BAND,
  OP_BOR,
  OP_BXOR,
  OP_BNOT,
  OP_LSH,
  OP_RSH,

  OP_LT,
  OP_EQ,
  OP_GT,

  OP_AND,
  OP_OR,
  OP_NOT,

  OP_JMP,
  OP_CALL,
  OP_RET,
};

struct Instruction {
  enum OpCode op;
  size_t* operands;
};

struct Instructions {

};

#endif
