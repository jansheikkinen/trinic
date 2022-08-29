/* parser.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include <string.h>

// bc there's a man page entry for this but no definition >:(
// this is not up to standard
void* reallocarray(void* ptr, size_t nmemb, size_t size) {
  return realloc(ptr, nmemb * size);
}

char* substring(const char* __restrict__ string, size_t start, size_t offset) {
  char* str = calloc(offset + 1, sizeof(*str));
  return strncpy(str, string + start, offset);
}

enum TokenType {
  UNDEFINED_TOKEN, // Just so null tokens don't think they're the token below

  // Unambiguous single character tokens
  LEFT_PAREN,    // (
  RIGHT_PAREN,   // )
  LEFT_BRACKET,  // [
  RIGHT_BRACKET, // ]
  COMMA,    // ,
  DOT,      // .
  QUESTION, // ?
  AT,       // @
  HASHTAG,  // #
  TILDE,    // ~
  COLON,    // :

  // Ambiguous single and multi-character tokens
  LEFT_DIAMOND,     // <
  BIT_LEFT,         // <<
  BIT_LEFT_ASSIGN,  // <<=
  LESS_EQ,          // <=

  RIGHT_DIAMOND,    // >
  BIT_RIGHT,        // >>
  BIT_RIGHT_ASSIGN, // >>=
  GREATER_EQ,       // >=

  EQUAL,            // ==
  ASSIGN,           // =
  MATCH_ARROW,      // =>

  ADD,              // +
  ADD_ASSIGN,       // +=

  MINUS,            // -
  MINUS_ASSIGN,     // -=

  STAR,             // *
  STAR_ASSIGN,      // *=

  SLASH,            // /
  SLASH_ASSIGN,     // /=

  // Comment,       // //
  // Long comment,  // /* */

  MOD,              // %
  MOD_ASSIGN,       // %=

  BANG,             // !
  NOT_EQ,           // !=

  BIT_AND,          // &=
  BIT_AND_ASSIGN,   // &

  BIT_OR,           // |=
  BIT_OR_ASSIGN,    // |

  BIT_XOR,          // ^=
  BIT_XOR_ASSIGN,   // ^

  // Literals
  STRING_LITERAL,  // "xyz"
  CHAR_LITERAL,    // 'x'
  INTEGER_LITERAL, // 69
  FLOAT_LITERAL,   // 420.69
  BOOLEAN_LITERAL, // true or false

  // Keywords
  SINT8,   // i8
  SINT16,  // i16
  SINT32,  // i32
  SINT64,  // i64
  ISIZE,   // isize
  UINT8,   // u8
  UINT16,  // u16
  UINT32,  // u32
  UINT64,  // u64
  USIZE,   // usize
  FLOAT16, // f16
  FLOAT32, // f32
  FLOAT64, // f64
  CHAR,
  VOID,
  BOOL,

  ENUM,
  STRUCT,
  // Not entirely sure about union

  FUNCTION,
  INLINE,

  FOR,
  IN,
  IF,
  ELSEIF,
  ELSE,
  WHILE,
  LOOP,
  CONTINUE,
  PASS,
  BREAK,
  MATCH,
  DO,
  END,

  INCLUDE,
  PUB,

  AND,
  OR,
  NOT,

  UNDEFINED,
  MUT,

  // Delete the following
  PRINT,
};


const char* tokenTypeStrings[] = {
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
  "SINT8",
  "SINT16",
  "SINT32",
  "SINT64",
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
  "PRINT",
};

#define getTokenName(tokentype) tokenTypeStrings[tokentype]

struct Token {
  enum TokenType type;
  const char* literal;
  size_t row;
  size_t col;
};

struct Token newToken(enum TokenType type, const char* literal,
    size_t row, size_t col) {
  struct Token token;

  token.type = type;
  token.literal = literal;
  token.row = row;
  token.col = col;

  return token;
}

void freeToken(struct Token* token) {
  free(token);
}


struct TokenArray {
  struct Token* tokens;
  size_t capacity;
  size_t length;
};

#define START_CAP 4
struct TokenArray* initialiseTokenArray(void) {
  struct TokenArray* tokenArray = malloc(sizeof(*tokenArray));

  tokenArray->capacity = START_CAP;
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
  free(array->tokens);
  free(array);
}


struct TokeniserData {
  struct TokenArray* tokens;
  const char* program;
  size_t index;
  size_t row;
  size_t col;
  size_t tokenStart;
};

void newTokeniser(struct TokeniserData* td, const char* __restrict__ program) {
  td->tokens = initialiseTokenArray();
  td->program = program;
  td->index = 0;
  td->row = 1;
  td->col = 1;
  td->tokenStart = 0;
}

#define get(td)  td->program[td->index]
#define peek(td) td->program[td->index + 1]
#define over(td) td->program[td->index + 2]

// TODO: consume(), as well

void nextToken(struct TokeniserData* td) {
  td->col += 1; td->index += 1; td->tokenStart = td->index;
}

// Whitespace and newline
void parseNonTokens(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '\n': td->row += 1; td->index += 1;
               td->tokenStart = td->index; td->col = 1; break;
    case ' ': nextToken(td); break;
  }
}

// The tokens that don't have any alphanumeric characters.
void newOperator(struct TokeniserData* td, enum TokenType tt) {
  appendToTokenArray(td->tokens, newToken(tt, NULL, td->row, td->col));
  nextToken(td);
}

// Tokens that correspond to a single-character operator that cannot
// be mistaken for any other token. I.e. '#' is unambiguous; it can
// *only* be just '#', but '+' is ambiguous because it could be '+' or '+='.
void parseUnambiguousOperator(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '(': newOperator(td, LEFT_PAREN);     break;
    case ')': newOperator(td, RIGHT_PAREN);    break;
    case '[': newOperator(td, LEFT_BRACKET);   break;
    case ']': newOperator(td, RIGHT_BRACKET);  break;
    case ',': newOperator(td, COMMA);          break;
    case '.': newOperator(td, DOT);            break;
    case '?': newOperator(td, QUESTION);       break;
    case '@': newOperator(td, AT);             break;
    case '#': newOperator(td, HASHTAG);        break;
    case '~': newOperator(td, TILDE);          break;
    case ':': newOperator(td, COLON);          break;
  }
}

// THIS DEPENDS ON THE ENUM ORDERING THE TWO TOKEN TYPES AS:
// OP, OP_ASSIGN,
// DO NOT FUCK THAT UP
void newAmbiguousArithOp(struct TokeniserData* td, enum TokenType tokenType) {
  if(peek(td) == '=') { newOperator(td, tokenType + 1); td->index += 1; }
  else newOperator(td, tokenType);
}

// Tokens that correspond to operators that *are* ambiguous just from
// looking at the first character. Using the example from above, this
// is where '+' and '+=' would go.
void parseAmbiguousOperator(struct TokeniserData* td) {
  switch(td->program[td->index]) {
    case '<':
      if(peek(td) == '<') {
        if(over(td) == '=') {
          newOperator(td, BIT_LEFT_ASSIGN); td->index += 2;
        } else { newOperator(td, BIT_LEFT); td->index += 1; }
      } else if(peek(td) == '=') { newOperator(td, LESS_EQ); td->index += 1; }
      else newOperator(td, LEFT_DIAMOND);
      break;
    case '>':
      if(peek(td) == '>') {
        if(over(td) == '=') {
          newOperator(td, BIT_RIGHT_ASSIGN); td->index += 2;
        } else { newOperator(td, BIT_RIGHT); td->index += 1; }
      } else if(peek(td) == '=') newOperator(td, GREATER_EQ);
      else newOperator(td, RIGHT_DIAMOND);
      break;

    case '=':
      if(peek(td) == '=') newOperator(td, EQUAL);
      else if(peek(td) == '>') newOperator(td, MATCH_ARROW);
      else newOperator(td, ASSIGN);
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


void newLiteralToken(struct TokeniserData* td,
    enum TokenType tt, const char* literal) {
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
void parseLiteral(struct TokeniserData* td) {
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
    if(i == td->index) td->index += 1;
  }

  struct TokenArray* tokens = td->tokens;
  free(td); // Isn't this beautiful?
  return tokens;
}

int main(void) {
  const char* str = "69+42.0";
  struct TokenArray* tokens = tokenise(str);

  printf("\n");
  printf("%02ld / %02ld\n", tokens->length, tokens->capacity);
  for(size_t i = 0; i < tokens->capacity; i++) {
    printf("%02ld: %20s, %s, (%02ld, %02ld)\n", i,
        getTokenName(tokens->tokens[i].type),
        tokens->tokens[i].literal,
        tokens->tokens[i].row,
        tokens->tokens[i].col);
  }

  freeTokenArray(tokens);
  return 0;
}
