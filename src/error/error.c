/* error.c */

/* Code for handling errors encountered during compilation */

#include <stdio.h>

#include "../util/textcolor.h"
#include "error.h"

const int LOC_COLOR = COL_BLUE;
const int ERROR_COLOR = COL_RED;
const int WARN_COLOR = COL_MAGENTA;

const char* ASTErrorMessages[] = {
  "Undefined Error",
  "Undefined AST Node",

  "Unclosed brackets",
  "Unclosed parentheses",

  "Unterminated argument node",

  "Unexpectedly reached end of file",

  "Expected parentheses",
  "Expected brackets",
  "Expected end of statement",
  "Expected block",
  "Expected end of block",
  "Expected expression",
  "Expected type",
  "Expected identifier",
  "Expected assignment",
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
  setColor(COLATTR_BRIGHT, ERROR_COLOR, COL_DEFAULT);
  printf("Error ");
  resetColor();

  printf("generating AST @ ");

  setColor(COLATTR_BRIGHT, LOC_COLOR, COL_DEFAULT);
  printf("%s:(%ld, %ld)", filename, error->row, error->col);
  resetColor();

  printf(" \"%s\"\n  ", error->tokenName);

  setColor(COLATTR_BRIGHT, ERROR_COLOR, COL_DEFAULT);
  printf("%03d", error->type);
  resetColor();

  printf(": %s.\n", ASTErrorMessages[error->type]);
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
