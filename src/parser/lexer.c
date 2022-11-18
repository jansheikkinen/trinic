/* lexer.c */

#include <stdio.h>

#include "../debug/debug.h"
#include "../util/strutil.h"
#include "tokeniser.h"
#include "lexer.h"

// Whitespace and newline
static void lexNonTokens(struct LexerContext* td) {
  switch(td->program[td->index]) {
    case '\n': td->row += 1; td->index += 1;
               td->tokenStart = td->index; td->col = 1; break;
    case ' ': nextToken(td); break;
  }
}

// The tokens that don't have any alphanumeric characters.
static void newNonLiteral(struct LexerContext* td, enum TokenType tt) {
  appendToTokenArray(td->tokens, newToken(tt, NULL, td->row, td->col));
  nextToken(td);
}

// Tokens that correspond to a single-character operator that cannot
// be mistaken for any other token. I.e. '#' is unambiguous; it can
// *only* be just '#', but '+' is ambiguous because it could be '+' or '+='.
static void lexUnambiguousOperator(struct LexerContext* td) {
  switch(td->program[td->index]) {
    case '(': newNonLiteral(td, TOKEN_LEFT_PAREN);     break;
    case ')': newNonLiteral(td, TOKEN_RIGHT_PAREN);    break;
    case '[': newNonLiteral(td, TOKEN_LEFT_BRACKET);   break;
    case ']': newNonLiteral(td, TOKEN_RIGHT_BRACKET);  break;
    case ',': newNonLiteral(td, TOKEN_COMMA);          break;
    case '.': newNonLiteral(td, TOKEN_DOT);            break;
    case '?': newNonLiteral(td, TOKEN_QUESTION);       break;
    case '@': newNonLiteral(td, TOKEN_AT);             break;
    case '#': newNonLiteral(td, TOKEN_HASHTAG);        break;
    case '~': newNonLiteral(td, TOKEN_TILDE);          break;
    case ':': newNonLiteral(td, TOKEN_COLON);          break;
  }
}

// THIS DEPENDS ON THE ENUM ORDERING THE TWO TOKEN TYPES AS:
// OP, OP_ASSIGN,
// DO NOT FUCK THAT UP
static void newAmbiguousArithOp(struct LexerContext* td, enum TokenType tokenType) {
  if(peek(td) == '=') { newNonLiteral(td, tokenType + 1); td->index += 1; }
  else newNonLiteral(td, tokenType);
}

// Tokens that correspond to operators that *are* ambiguous just from
// looking at the first character. Using the example from above, this
// is where '+' and '+=' would go.
static void lexAmbiguousOperator(struct LexerContext* td) {
  switch(td->program[td->index]) {
    case '<':
      if(peek(td) == '<') {
        if(over(td) == '=') {
          newNonLiteral(td, TOKEN_BIT_LEFT_ASSIGN); td->index += 2;
        } else { newNonLiteral(td, TOKEN_BIT_LEFT); td->index += 1; }
      } else if(peek(td) == '=') { newNonLiteral(td, TOKEN_LESS_EQ); td->index += 1; }
      else newNonLiteral(td, TOKEN_LEFT_DIAMOND);
      break;
    case '>':
      if(peek(td) == '>') {
        if(over(td) == '=') {
          newNonLiteral(td, TOKEN_BIT_RIGHT_ASSIGN); td->index += 2;
        } else { newNonLiteral(td, TOKEN_BIT_RIGHT); td->index += 1; }
      } else if(peek(td) == '=') newNonLiteral(td, TOKEN_GREATER_EQ);
      else newNonLiteral(td, TOKEN_RIGHT_DIAMOND);
      break;

    case '=':
      if(peek(td) == '=') {
        newNonLiteral(td, TOKEN_EQUAL);
        td->index += 1;
      } else if(peek(td) == '>') {
        newNonLiteral(td, TOKEN_MATCH_ARROW);
        td->index += 1;
      } else newNonLiteral(td, TOKEN_ASSIGN);
      break;

    case '/':
      if(peek(td) == '/') {
        char got = get(td);
        while(got && got != '\n') {
          td->index += 1;
          got = get(td);
        }

        td->row += 1;
        td->index += 1;
        td->tokenStart = td->index;
        td->col = 1;
      } else if(peek(td) == '*') {
        char got = get(td);
        while(got) {
          if(got == '*' && peek(td) == '/') break;
          else if(got == '\n') { td->row += 1; td->col = 1; }
          else td->col += 1;

          td->index += 1;
          got = get(td);
        }

        td->index += 1;
        nextToken(td);
      } else newAmbiguousArithOp(td, TOKEN_SLASH);
      break;

    case '-':
      if(peek(td) == '>') {
        newNonLiteral(td, TOKEN_ARROW);
        td->index += 1;
      } else if(peek(td) == '=') {
        newNonLiteral(td, TOKEN_MINUS_ASSIGN);
        td->index += 1;
      } else newNonLiteral(td, TOKEN_MINUS); break;

    case '+': newAmbiguousArithOp(td, TOKEN_ADD);     break;
    case '*': newAmbiguousArithOp(td, TOKEN_STAR);    break;
    case '%': newAmbiguousArithOp(td, TOKEN_MOD);     break;
    case '!': newAmbiguousArithOp(td, TOKEN_BANG);    break;
    case '&': newAmbiguousArithOp(td, TOKEN_BIT_AND); break;
    case '|': newAmbiguousArithOp(td, TOKEN_BIT_OR);  break;
    case '^': newAmbiguousArithOp(td, TOKEN_BIT_XOR); break;
  }
}


static void newLiteralToken(struct LexerContext* td,
    enum TokenType tt, char* literal) {
  appendToTokenArray(td->tokens, newToken(tt, literal, td->row, td->col));
  nextToken(td);
}

// Literals: strings, chars, numbers
// Should technically include boolean literals "true" and "false", but its
// easier to include those two with the keywords, instead
static void lexLiteral(struct LexerContext* td) {
  if(get(td) == '\"') {
    nextToken(td);
    while(td->program[td->index++]) {
      if(get(td) == '\"') {
        newLiteralToken(td, TOKEN_STRING_LITERAL,
          substring(td->program, td->tokenStart, td->index - td->tokenStart));
        return;
      } else if(get(td) == '\n') {
        td->row += 1; td->col = 1;
      } else {
        td->col += 1;
      }
    }

    APPEND_LEXERROR(td, LEXERR_UNTERMINATED_STRING);

  } else if(get(td) == '\'') {
    nextToken(td);
    if(peek(td) == '\'') {
      char* literal = calloc(2, 1); // bc null-terminated
      *literal = get(td);
      newLiteralToken(td, TOKEN_CHAR_LITERAL, literal);
      nextToken(td);
    } else APPEND_LEXERROR(td, LEXERR_UNTERMINATED_CHAR);

  // TODO: add support for any radix 1-60
  // Right now it can only handle decimal.
  // Floats must be 0.x, not .x or xf. This is intended.
  } else if(isdigit(get(td))) {
    int isFloat = 0;
    while(td->program[td->index++]) {
      if(isdigit(get(td))) td->col += 1;
      else if(get(td) == '.' && isdigit(peek(td))) {
        td->col += 1; isFloat = 1;
      } else {
        td->col += 1;
        appendToTokenArray(td->tokens,
            newToken(isFloat ? TOKEN_FLOAT_LITERAL : TOKEN_INTEGER_LITERAL,
              substring(td->program, td->tokenStart, td->index - td->tokenStart),
              td->row, td->col));
        return;
      }
    }
  }
}

static int isalnum_(int c) {
  return isalnum(c) || c == '_';
}

// The ones like "int32" or "if" that are alphanumeric
static void lexKeyword(struct LexerContext* td) {
  if(!isalnum_(get(td))) return;

  while(peek(td) && isalnum_(peek(td))) {
    td->index += 1;
    td->col += 1;
  }


  char* keyword =
    substring(td->program, td->tokenStart, td->index - td->tokenStart + 1);

  for(enum TokenType tt = TOKEN_TRUE; tt < TOKEN_PRINT + 1; tt++) { // cursed
    const char* c = getTokenName(tt);
    char* cmpToken = calloc(strlen(c), 1);
    strncpy(cmpToken, c, strlen(c));

    if(streq(keyword, strlower(cmpToken))) {
      newNonLiteral(td, tt);
      free(keyword); free(cmpToken);
      return;
    } else free(cmpToken);
  }

  newLiteralToken(td, TOKEN_IDENTIFIER_LITERAL, keyword);
}

// I could have definitely written the tokeniser more efficiently;
// I could have written the tokeniser as one massive switch statement,
// but, quite frankly, that's ugly and I'd rather modularise it for readability
struct TokenArray* tokenise(const char* filename,
    const char* __restrict__ program) {
#ifdef LEXER_DEBUG
  printf("Generating tokens from source...\n");
#endif

  struct LexerContext* td = malloc(sizeof(*td));;
  newTokeniser(td, program);

  while(get(td)) {
    size_t i = td->index;
    lexNonTokens(td);

    lexUnambiguousOperator(td);
    lexAmbiguousOperator(td);

    lexLiteral(td);

    lexKeyword(td);
    if(i == td->index) td->index += 1;
  }

  struct LexErrorList* errors = td->errors;
  if(errors->size > 0) {
    for(size_t i = 0; i < errors->size; i++) {
      printLexError(filename, &errors->errors[i]);
    }

    exit(2);
  }

  struct TokenArray* tokens = td->tokens;
  free(td); // Isn't this beautiful?

#ifdef LEXER_DEBUG
  printf("Generated %ld tokens\n", tokens->length);
#endif

  return tokens;
}
