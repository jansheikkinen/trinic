#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

enum TokenType {
  TOKEN_UNDEFINED_TOKEN, // Just so null tokens don't think they're the token below

  // Unambiguous single character tokens
  TOKEN_LEFT_PAREN,    // (
  TOKEN_RIGHT_PAREN,   // )
  TOKEN_LEFT_BRACKET,  // [
  TOKEN_RIGHT_BRACKET, // ]
  TOKEN_LEFT_CURLY,    // {
  TOKEN_RIGHT_CURLY,   // }
  TOKEN_COMMA,    // ,
  TOKEN_QUESTION, // ?
  TOKEN_AT,       // @
  TOKEN_HASHTAG,  // #
  TOKEN_TILDE,    // ~
  TOKEN_COLON,    // :
  TOKEN_SEMICOLON,// ;

  // Ambiguous single and multi-character tokens
  TOKEN_LEFT_DIAMOND,     // <
  TOKEN_BIT_LEFT,         // <<
  TOKEN_BIT_LEFT_ASSIGN,  // <<=
  TOKEN_LESS_EQ,          // <=

  TOKEN_RIGHT_DIAMOND,    // >
  TOKEN_BIT_RIGHT,        // >>
  TOKEN_BIT_RIGHT_ASSIGN, // >>=
  TOKEN_GREATER_EQ,       // >=

  TOKEN_EQUAL,            // ==
  TOKEN_ASSIGN,           // =
  TOKEN_MATCH_ARROW,      // =>

  TOKEN_ADD,              // +
  TOKEN_ADD_ASSIGN,       // +=

  TOKEN_DOT,              // .
  TOKEN_DOT_DOT,          // ..

  TOKEN_MINUS,            // -
  TOKEN_MINUS_ASSIGN,     // -=
  TOKEN_ARROW,            // ->

  TOKEN_STAR,             // *
  TOKEN_STAR_ASSIGN,      // *=

  TOKEN_SLASH,            // /
  TOKEN_SLASH_ASSIGN,     // /=

  // Comment,       // //
  // Long comment,  // /* */

  TOKEN_MOD,              // %
  TOKEN_MOD_ASSIGN,       // %=

  TOKEN_BANG,             // !
  TOKEN_NOT_EQ,           // !=

  TOKEN_BIT_AND,          // &=
  TOKEN_BIT_AND_ASSIGN,   // &

  TOKEN_BIT_OR,           // |=
  TOKEN_BIT_OR_ASSIGN,    // |

  TOKEN_BIT_XOR,          // ^=
  TOKEN_BIT_XOR_ASSIGN,   // ^

  // Literals
  TOKEN_STRING_LITERAL,     // "xyz"
  TOKEN_CHAR_LITERAL,       // 'x'
  TOKEN_INTEGER_LITERAL,    // 69
  TOKEN_FLOAT_LITERAL,      // 420.69
  TOKEN_IDENTIFIER_LITERAL, // user-defined names: variables n functions

  // Keywords
  TOKEN_TRUE,
  TOKEN_FALSE,

  TOKEN_INT8,
  TOKEN_INT16,
  TOKEN_INT32,
  TOKEN_INT64,
  TOKEN_ISIZE,
  TOKEN_UINT8,
  TOKEN_UINT16,
  TOKEN_UINT32,
  TOKEN_UINT64,
  TOKEN_USIZE,
  TOKEN_FLOAT8,
  TOKEN_FLOAT16,
  TOKEN_FLOAT32,
  TOKEN_FLOAT64,
  TOKEN_FSIZE,
  TOKEN_BYTE,
  TOKEN_CHAR,
  TOKEN_VOID,
  TOKEN_BOOL,
  TOKEN_ANY,

  TOKEN_ENUM,
  TOKEN_STRUCT,
  TOKEN_UNION, // Not entirely sure about union

  TOKEN_SUM,
  TOKEN_INTERFACE,
  TOKEN_IMPL,

  TOKEN_LET,

  TOKEN_FUNCTION,
  TOKEN_INLINE,
  TOKEN_WHERE,

  TOKEN_FOR,
  TOKEN_IN,
  TOKEN_IF,
  TOKEN_ELSEIF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_LOOP,
  TOKEN_CONTINUE,
  TOKEN_PASS,
  TOKEN_BREAK,
  TOKEN_MATCH,
  TOKEN_DO,
  TOKEN_END,

  TOKEN_INCLUDE,
  TOKEN_EXTERN,

  TOKEN_AS,

  TOKEN_AND,
  TOKEN_OR,
  TOKEN_NOT,

  TOKEN_UNDEFINED,
  TOKEN_MUT,

  TOKEN_RETURN,

  // Delete the following
  TOKEN_PRINT,
};

extern char* tokenTypeStrings[];
#define getTokenName(tokentype) tokenTypeStrings[tokentype]

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
