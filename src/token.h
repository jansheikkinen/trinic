#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

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
  STRING_LITERAL,     // "xyz"
  CHAR_LITERAL,       // 'x'
  INTEGER_LITERAL,    // 69
  FLOAT_LITERAL,      // 420.69
  BOOLEAN_LITERAL,    // true or false
  IDENTIFIER_LITERAL, // user-defined names: variables n functions

  // Keywords
  INT8,   // i8
  INT16,  // i16
  INT32,  // i32
  INT64,  // i64
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

  RETURN,

  // Delete the following
  PRINT,
};

struct Token {
  enum TokenType type;
  char* literal;
  size_t row;
  size_t col;
};

struct TokenArray {
  struct Token* tokens;
  size_t capacity;
  size_t length;
};

struct Token newToken(enum TokenType, char*, size_t, size_t);
void freeToken(struct Token*);

struct TokenArray* initialiseTokenArray(void);
void appendToTokenArray(struct TokenArray*, struct Token);
void freeTokenArray(struct TokenArray*);


#endif
