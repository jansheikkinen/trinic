# Grammar
A grammar specification of the Unholy C language.

## Format
The grammar specification uses a form of BNF.

`<x>`     a non-terminal
`"x"`     is a string literal
`x | y`   choose between x or y
`(x y)`   x and y grouped together
`x ::= y` replace x with y
`x*`      repeat x one or more times
`x?`      x zero or one time(s)
`;`       end rule

## Unholy C Grammar

```
<expression> ::= <logic> ;
<logic>       ::= <equality> (("and" | "or") <equality>)* ;
<equality>   ::= <compare> (("==" | "!=") <compare>)* ;
<compare>    ::= <bitwise> ((">" | ">=" | "<" | "<=") <bitwise>)* ;
<bitwise>    ::= <term> (("&" | "|" | "^" | "<<" | ">>") <term>)* ;
<term>       ::= <factor> (("+" | "-") <factor)* ;
<factor>     ::= <unary> (("*" | "/") <unary>)* ;
<unary>      ::= ("~" | "!" | "not" | "-") <unary> ;
<primary>    ::= "(" <expression> ")" | NUMBER | STRING | IDENTIFIER ;
```
