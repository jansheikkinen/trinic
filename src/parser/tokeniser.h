#ifndef TOKENISER_H
#define TOKENISER_H

#include <stdlib.h>

#include "../error/error.h"
#include "token.h"

struct LexErrorList {
  struct LexError* errors;
  size_t size;
  size_t capacity;
};

struct LexerContext {
  struct LexErrorList* errors;
  struct TokenArray* tokens;
  const char* program;
  size_t index;
  size_t row;
  size_t col;
  size_t tokenStart;
};

void newTokeniser(struct LexerContext*, const char* __restrict__);
void nextToken(struct LexerContext*);
void appendLexError(struct LexerContext*, struct LexError);

#define get(td)  td->program[td->index]
#define peek(td) td->program[td->index + 1]
#define over(td) td->program[td->index + 2]

#define APPEND_LEXERROR(ctx, err) \
  appendLexError(ctx, newLexError(err, ctx->row, ctx->col))

#endif
