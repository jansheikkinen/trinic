#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

extern const char* ASTErrorMessages[];
extern const char* LexerErrorMessages[];

enum ASTErrorType {
  ASTERR_UNDEFINED,
  ASTERR_FUNC_NO_LEFT_PAREN,
  ASTERR_UNCLOSED_PAREN,
  ASTERR_STMT_END,
  ASTERR_UNDEFINED_NODE,
  ASTERR_UNDEFINED_COND_NODE,
  ASTERR_UNTERMINATED_IF,
  ASTERR_UNTERMINATED_LOOP,
  ASTERR_UNTERMINATED_WHILE,
};

struct ASTError {
  enum ASTErrorType type;
  const char* tokenName;
  size_t row;
  size_t col;
};

struct ASTError newASTError(enum ASTErrorType, const char*, size_t, size_t);
void printASTError(const char*, const struct ASTError*);

enum LexerErrorType {
  LEXERR_UNDEFINED,
  LEXERR_UNTERMINATED_STRING,
  LEXERR_UNTERMINATED_CHAR,
};

struct LexError {
  enum LexerErrorType type;
  // No token name, since we're trying to *generate* the tokens
  size_t row;
  size_t col;
};

struct LexError newLexError(enum LexerErrorType, size_t, size_t);
void printLexError(const char*, const struct LexError*);

#endif
