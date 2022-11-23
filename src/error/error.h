#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

extern const char* ASTErrorMessages[];
extern const char* LexerErrorMessages[];

enum ASTErrorType {
  ASTERR_UNDEFINED,
  ASTERR_UNDEFINED_NODE,

  ASTERR_UNCLOSED_BRACKET,
  ASTERR_UNCLOSED_PAREN,

  ASTERR_UNTERMINATED_ARGS,

  ASTERR_UNEXPECTED_EOF,

  ASTERR_EXPECTED_PAREN,
  ASTERR_EXPECTED_BRACKET,
  ASTERR_EXPECTED_ARROW_FUNCTION,
  ASTERR_EXPECTED_ARROW_MATCH,
  ASTERR_EXPECTED_COLON,
  ASTERR_EXPECTED_STMT_END,
  ASTERR_EXPECTED_BLOCK,
  ASTERR_EXPECTED_END,
  ASTERR_EXPECTED_EXPRESSION,
  ASTERR_EXPECTED_TYPE,
  ASTERR_EXPECTED_IDENTIFIER,
  ASTERR_EXPECTED_ASSIGN,
  ASTERR_EXPECTED_FOR_IMPL,
  ASTERR_EXPECTED_IN_FOR,
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
