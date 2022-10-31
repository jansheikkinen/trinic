/* error.c */

/* Code for handling errors encountered during compilation */

#include <stdio.h>
#include "error.h"

const char* errorMessages[] = {
  "Undefined Error",
  "Missing '(' after function call",
  "Unclosed parentheses",
  "Expected end of statement",
};

struct ASTError newASTError(enum ASTErrorType type, const char* tokenName,
    size_t row, size_t col) {
  struct ASTError error;

  error.type = type;
  error.tokenName = tokenName;
  error.row = row;
  error.col = col;

  return error;
}

// TODO: Filenames
void printASTError(const char* filename, const struct ASTError* error) {
  printf("error @ %s:(%ld, %ld), token \"%s\": %s.\n", filename,
      error->row, error->col, error->tokenName, errorMessages[error->type]);
}
