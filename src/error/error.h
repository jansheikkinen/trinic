#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

enum ASTErrorType {
  ASTERR_UNDEFINED,
  ASTERR_FUNC_NO_LEFT_PAREN,
  ASTERR_UNCLOSED_PAREN,
  ASTERR_STMT_END,
};

extern const char* errorMessages[];

struct ASTError {
  enum ASTErrorType type;
  const char* tokenName;
  size_t row;
  size_t col;
};

struct ASTError newASTError(enum ASTErrorType, const char*, size_t, size_t);

void printASTError(const char*, const struct ASTError*);

#endif
