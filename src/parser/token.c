/* token.c */

#include "../util/reallocarray.h"
#include "token.h"

char* tokenTypeStrings[] = {
  "UNDEFINED",
  "(",
  ")",
  "[",
  "]",
  ",",
  ".",
  "?",
  "@",
  "#",
  "~",
  ":",
  "<",
  "<<",
  "<<=",
  "<=",
  ">",
  ">>",
  ">>=",
  ">=",
  "==",
  "=",
  "=>",
  "+",
  "+=",
  "-",
  "-=",
  "*",
  "*=",
  "/",
  "/=",
  "%",
  "%=",
  "!",
  "!=",
  "&",
  "&=",
  "|",
  "|=",
  "^",
  "^=",
  "STRING",
  "CHAR",
  "INTEGER",
  "FLOAT",
  "IDENTIFIER",
  "TRUE",
  "FALSE",
  "INT8",
  "INT16",
  "INT32",
  "INT64",
  "ISIZE",
  "UINT8",
  "UINT16",
  "UINT32",
  "UINT64",
  "USIZE",
  "FLOAT8",
  "FLOAT16",
  "FLOAT32",
  "FLOAT64",
  "FSIZE",
  "BYTE",
  "CHAR",
  "VOID",
  "BOOL",
  "ENUM",
  "STRUCT",
  "LET",
  "FUNCTION",
  "INLINE",
  "FOR",
  "IN",
  "IF",
  "ELSEIF",
  "ELSE",
  "WHILE",
  "LOOP",
  "CONTINUE",
  "PASS",
  "BREAK",
  "MATCH",
  "DO",
  "END",
  "INCLUDE",
  "PUB",
  "AND",
  "OR",
  "NOT",
  "UNDEFINED",
  "MUT",
  "RETURN",
  "PRINT",
};

struct Token newToken(enum TokenType type, char* literal,
    size_t row, size_t col) {
  struct Token token;

  token.type = type;
  token.literal = literal;
  token.row = row;
  token.col = col;

  return token;
}

void freeToken(struct Token* token) {
  if(token->type != TOKEN_IDENTIFIER_LITERAL
      && token->type != TOKEN_STRING_LITERAL)
    free(token->literal);
}


struct TokenArray* initialiseTokenArray(void) {
  struct TokenArray* tokenArray = malloc(sizeof(*tokenArray));

  tokenArray->capacity = 4;
  tokenArray->length   = 0;
  struct Token* tokens = calloc(tokenArray->capacity, sizeof(*tokens));
  tokenArray->tokens   = tokens;

  return tokenArray;
}

void appendToTokenArray(struct TokenArray* array, struct Token token) {
  if(array->length >= array->capacity) {
    array->capacity *= 2;
    array->tokens =
      reallocarray(array->tokens, array->capacity, sizeof(struct Token));
  }
  array->tokens[array->length++] = token;
}

void freeTokenArray(struct TokenArray* array) {
  for(size_t i = 0; i < array->length; i++) {
    freeToken(array->tokens + i);
  }
  free(array);
}
