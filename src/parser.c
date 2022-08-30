/* parser.c */

#include "strutil.h"
#include "parser.h"

char* tokenTypeStrings[] = {
  "UNDEFINED",
  "LEFT_PAREN",
  "RIGHT_PAREN",
  "LEFT_BRACKET",
  "RIGHT_BRACKET",
  "COMMA",
  "DOT",
  "QUESTION",
  "AT",
  "HASHTAG",
  "TILDE",
  "COLON",
  "LEFT_DIAMOND",
  "BIT_LEFT",
  "BIT_LEFT_ASSIGN",
  "LESS_EQ",
  "RIGHT_DIAMOND",
  "BIT_RIGHT",
  "BIT_RIGHT_ASSIGN",
  "GREATER_EQ",
  "EQUAL",
  "ASSIGN",
  "MATCH_ARROW",
  "ADD",
  "ADD_ASSIGN",
  "MINUS",
  "MINUS_ASSIGN",
  "STAR",
  "STAR_ASSIGN",
  "SLASH",
  "SLASH_ASSIGN",
  "MOD",
  "MOD_ASSIGN",
  "BANG",
  "NOT_EQ",
  "BIT_AND",
  "BIT_AND_ASSIGN",
  "BIT_OR",
  "BIT_OR_ASSIGN",
  "BIT_XOR",
  "BIT_XOR_ASSIGN",
  "STRING_LITERAL",
  "CHAR_LITERAL",
  "INTEGER_LITERAL",
  "FLOAT_LITERAL",
  "BOOLEAN_LITERAL",
  "IDENTIFIER_LITERAL",
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
  "FLOAT16",
  "FLOAT32",
  "FLOAT64",
  "CHAR",
  "VOID",
  "BOOL",
  "ENUM",
  "STRUCT",
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

// Whitespace and newline
static void parseNonTokens(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '\n': td->row += 1; td->index += 1;
               td->tokenStart = td->index; td->col = 1; break;
    case ' ': nextToken(td); break;
  }
}

// The tokens that don't have any alphanumeric characters.
static void newNonLiteral(struct TokeniserData* td, enum TokenType tt) {
  appendToTokenArray(td->tokens, newToken(tt, NULL, td->row, td->col));
  nextToken(td);
}

// Tokens that correspond to a single-character operator that cannot
// be mistaken for any other token. I.e. '#' is unambiguous; it can
// *only* be just '#', but '+' is ambiguous because it could be '+' or '+='.
static void parseUnambiguousOperator(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '(': newNonLiteral(td, LEFT_PAREN);     break;
    case ')': newNonLiteral(td, RIGHT_PAREN);    break;
    case '[': newNonLiteral(td, LEFT_BRACKET);   break;
    case ']': newNonLiteral(td, RIGHT_BRACKET);  break;
    case ',': newNonLiteral(td, COMMA);          break;
    case '.': newNonLiteral(td, DOT);            break;
    case '?': newNonLiteral(td, QUESTION);       break;
    case '@': newNonLiteral(td, AT);             break;
    case '#': newNonLiteral(td, HASHTAG);        break;
    case '~': newNonLiteral(td, TILDE);          break;
    case ':': newNonLiteral(td, COLON);          break;
  }
}

// THIS DEPENDS ON THE ENUM ORDERING THE TWO TOKEN TYPES AS:
// OP, OP_ASSIGN,
// DO NOT FUCK THAT UP
static void newAmbiguousArithOp(struct TokeniserData* td, enum TokenType tokenType) {
  if(peek(td) == '=') { newNonLiteral(td, tokenType + 1); td->index += 1; }
  else newNonLiteral(td, tokenType);
}

// Tokens that correspond to operators that *are* ambiguous just from
// looking at the first character. Using the example from above, this
// is where '+' and '+=' would go.
static void parseAmbiguousOperator(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '<':
      if(peek(td) == '<') {
        if(over(td) == '=') {
          newNonLiteral(td, BIT_LEFT_ASSIGN); td->index += 2;
        } else { newNonLiteral(td, BIT_LEFT); td->index += 1; }
      } else if(peek(td) == '=') { newNonLiteral(td, LESS_EQ); td->index += 1; }
      else newNonLiteral(td, LEFT_DIAMOND);
      break;
    case '>':
      if(peek(td) == '>') {
        if(over(td) == '=') {
          newNonLiteral(td, BIT_RIGHT_ASSIGN); td->index += 2;
        } else { newNonLiteral(td, BIT_RIGHT); td->index += 1; }
      } else if(peek(td) == '=') newNonLiteral(td, GREATER_EQ);
      else newNonLiteral(td, RIGHT_DIAMOND);
      break;

    case '=':
      if(peek(td) == '=') newNonLiteral(td, EQUAL);
      else if(peek(td) == '>') newNonLiteral(td, MATCH_ARROW);
      else newNonLiteral(td, ASSIGN);
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
      } else newAmbiguousArithOp(td, SLASH);
      break;

    case '+': newAmbiguousArithOp(td, ADD);     break;
    case '-': newAmbiguousArithOp(td, MINUS);   break;
    case '*': newAmbiguousArithOp(td, STAR);    break;
    case '%': newAmbiguousArithOp(td, MOD);     break;
    case '!': newAmbiguousArithOp(td, BANG);    break;
    case '&': newAmbiguousArithOp(td, BIT_AND); break;
    case '|': newAmbiguousArithOp(td, BIT_OR);  break;
    case '^': newAmbiguousArithOp(td, BIT_XOR); break;
  }
}


static void newLiteralToken(struct TokeniserData* td,
    enum TokenType tt, char* literal) {
  appendToTokenArray(td->tokens, newToken(tt, literal, td->row, td->col));
  nextToken(td);
}

// Literals: strings, chars, numbers
// Should technically include boolean literals "true" and "false", but its
// easier to include those two with the keywords, instead
//
// TODO: This part can fail, actually. Right now, I just have it set to
// ignore the errors entirely, but ideally they'd be thrown to the calling
// function and handled properly.
static void parseLiteral(struct TokeniserData* td) {
  if(get(td) == '\"') {
    nextToken(td);
    while(td->program[td->index++]) {
      if(get(td) == '\"') {
        newLiteralToken(td, STRING_LITERAL,
          substring(td->program, td->tokenStart, td->index - td->tokenStart));
        return;
      } else if(get(td) == '\n') {
        td->row += 1; td->col = 1;
      } else {
        td->col += 1;
      }
    }

  } else if(get(td) == '\'') {
    nextToken(td);
    if(peek(td) == '\'') {
      char* literal = calloc(2, 1); // bc null-terminated
      *literal = get(td);
      newLiteralToken(td, CHAR_LITERAL, literal);
    }

  // TODO: add support for any radix 1-60
  // Right now it can only handle decimal.
  // Floats must be 0.x, not .x or xf. This is intended.
  } else if(isdigit(get(td))) {
    int isFloat = 0;
    while(td->program[td->index++]) {
      if(isdigit(get(td))) td->col += 1;
      else if(get(td) == '.') { td->col += 1; isFloat = 1; }
      else {
        td->col += 1;
        appendToTokenArray(td->tokens,
            newToken(isFloat ? FLOAT_LITERAL : INTEGER_LITERAL,
              substring(td->program, td->tokenStart, td->index - td->tokenStart),
              td->row, td->col));
        return;
      }
    }
  }
}

// The ones like "int32" or "if" that are alphanumeric
static void parseKeyword(struct TokeniserData* td) {
  if(!isalnum(get(td))) return;

  while(peek(td) && isalnum(peek(td))) {
    td->index += 1;
    td->col += 1;
  }


  char* keyword =
    substring(td->program, td->tokenStart, td->index - td->tokenStart + 1);

  for(enum TokenType tt = INT8; tt < PRINT + 1; tt++) { // cursed
    const char* c = getTokenName(tt);
    char* cmpToken = calloc(strlen(c), 1);
    strncpy(cmpToken, c, strlen(c));

    if(streq(keyword, strlower(cmpToken))) {
      newNonLiteral(td, tt);
      free(keyword); free(cmpToken);
      return;
    } else free(cmpToken);
  }

  newLiteralToken(td, IDENTIFIER_LITERAL, keyword);
}

// I could have definitely written the tokeniser more efficiently;
// I could have written the tokeniser as one massive switch statement,
// but, quite frankly, that's ugly and I'd rather modularise it for readability
struct TokenArray* tokenise(const char* __restrict__ program) {
  struct TokeniserData* td = malloc(sizeof(*td));;
  newTokeniser(td, program);

  while(get(td)) {
    size_t i = td->index;
    parseNonTokens(td);

    parseUnambiguousOperator(td);
    parseAmbiguousOperator(td);

    parseLiteral(td);

    parseKeyword(td);
    if(i == td->index) td->index += 1;
  }

  struct TokenArray* tokens = td->tokens;
  free(td); // Isn't this beautiful?
  return tokens;
}
