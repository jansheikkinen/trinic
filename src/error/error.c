/* error.c */

/* Code for handling errors encountered during compilation */

#include <stdio.h>
#include "error.h"

const char* ASTErrorMessages[] = {
  "Undefined Error",
  "Missing '(' after function call",
  "Unclosed parentheses",
  "Expected end of statement",
  "Undefined AST node",
  "Undefined else branch in conditional node",
  "Unterminated if statement"
};

const char* LexerErrorMessages[] = {
  "Undefined Error",
  "Unterminated string",
  "Unterminated char",
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
  printf("Error generating AST @ %s:(%ld, %ld), token \"%s\"\n  %03d: %s.\n",
      filename, error->row, error->col, error->tokenName, error->type,
      ASTErrorMessages[error->type]);
}

struct LexError newLexError(enum LexerErrorType type, size_t row, size_t col) {
  struct LexError error;

  error.type = type;
  error.row = row;
  error.col = col;

  return error;
}

void printLexError(const char* filename, const struct LexError* error) {
  printf("Error generating tokens @ %s:(%ld, %ld): %s\n", filename,
      error->row, error->col, LexerErrorMessages[error->type]);
}
