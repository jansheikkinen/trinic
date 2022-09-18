# Grammar
A grammar specification of the Unholy C language.

## Format
The grammar specification uses a form of BNF.

`x`       a non-terminal
`"x"`     is a string literal
`x | y`   choose between x or y
`(x y)`   x and y grouped together
`x ::= y` replace x with y
`x*`      repeat x one or more times
`x?`      x zero or one time(s)
`;`       end rule

## Unholy C Grammar

```
basetype    ::=  "int8"   | "int16"   | "int32"   | "int64"   | "isize"
               | "uint8"  | "uint16"  | "uint32"  | "uint64"  | "usize"
               | "float8" | "float16" | "float32" | "float64" | "fsize"
               | bool ;
arraytype   ::= type "[" expression "]" ;
pointertype ::= type "*" ;
structtype  ::= "struct" IDENTIFIER structbody*? ;
structbody  ::= vardecl* "end" ;
type        ::= basetype | arraytype | pointertype | structtype ;

vardecl ::= ;

expression ::= logic ;

assignment ::= IDENTIFIER "=" logic ;

logic      ::= equality ( ("and" | "or")                  equality )*  ;
equality   ::= compare  ( ("==" | "!=")                   compare  )*  ;
compare    ::= bitwise  ( (">" | ">=" | "<" | "<=")       bitwise  )*  ;
bitwise    ::= term     ( ("&" | "|" | "^" | "<<" | ">>") term     )*  ;
term       ::= factor   ( ("+" | "-")                     factor   )*  ;
factor     ::= unary    ( ("*" | "/")                     unary    )*  ;
unary      ::=            ("~" | "!" | "not" | "-")       unary | call ;
call       ::= primary "(" arguments? ")" ;
primary    ::= "(" expression ")" | NUMBER | STRING | IDENTIFIER ;

arguments ::= expression ( "," expression )* ;
```
