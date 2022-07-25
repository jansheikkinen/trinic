#lang racket

; TRINI-C INTERPRETER


; ## TOKENS ## ;

(define LEFT_PAREN   0) (define RIGHT_PAREN   1)
(define LEFT_BRACKET 2) (define RIGHT_BRACKET 3)
(define COMMA        4) (define DOT           5)
(define QUESTION     6) (define AT            7)
(define HASHTAG      8) (define TILDE         9)
(define COLON       10)

(define LEFT_ARROW  11) (define RIGHT_ARROW  12)
(define EQUAL       13) (define PLUS         14)
(define MINUS       15) (define STAR         16)
(define SLASH       17) (define PERCENT      18)
(define EXCLAMATION 19) (define AMPERSAND    20)
(define VERT_LINE   21) (define CARET        22)

(define STRING_LIT  23) (define CHAR_LIT     24)
(define INT_LIT     25) (define FLOAT_LIT    26)
(define IDENTIFIER  27)

(define INT8    28) (define INT16   29)
(define INT32   30) (define INT64   31)
(define ISIZE   32)
(define UINT8   33) (define UINT16  34)
(define UINT32  35) (define UINT64  36)
(define USIZE   37)
(define FLOAT16 38) (define FLOAT32 39)
(define FLOAT64 40)
(define CHAR    41) (define STRING  42)
(define VOID    43) (define BOOL    44)
(define TRUE    45) (define FALSE   46)

(define ENUM    47) (define UNION 48) ; UNION might be merged into ENUM
(define STRUCT  49) (define ANON  50) ; ANON and TAGGED would then be useless
(define TAGGED  51)

(define FUNCTION 52) (define INLINE 53)

(define FOR    54) (define FOREACH 55)
(define IN     56) (define IF      57)
(define ELSEIF 58) (define ELSE    59)
(define WHILE  60) (define LOOP    61)
(define MATCH  62) (define DO      63)
(define END    64)

(define INCLUDE 65) (define EXTERN 66)

(define AND 67) (define OR 68)
(define NOT 69)

(define MUT 70) (define UNDEFINED 71)

(define PASS  72) (define CONTINUE 73)
(define BREAK 74)

; Tokens past this point are to be deleted
(define PRINT -1)


; ## TOKENIZER ## ;

(struct Token (type literal line) #:transparent)


; ## PARSER ## ;


; ## INTERPRETER ## ;


(define (main) (void))
