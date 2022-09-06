# Unholy C Bytecode
Unholy C's intermediate representation is a register-based bytecode.
The purpose of the UC bytecode is twofold: to be the storage medium for interpreted UC programs, as well as to be the representation of the program passed between stages of the compiler/interpreter

## Format
Instructions are written in binary in use, but for the sake of documentation readability, to write instruction definitions, this document will use a format similar to Lua's method of writing instructions:
- R(X):  the Xth register
- C(X):  the Xth constant
- RC(X): can be either a constant or register
- PC: the program counter
For example, `ADD A B C | R(A) = RC(B) + RC(C)` is how the `ADD` instruction definition would be written.

## Instructions
The following is the full instruction set
```
CODE | INSTRUCTION | DEFINITION
-----+-------------+----------------------
0x00 | ADD  A B C  | R(A) = RC(B) + RC(C)
0x01 | SUB  A B C  | R(A) = RC(B) - RC(C)
0x02 | MUL  A B C  | R(A) = RC(B) * RC(C)
0x03 | DIV  A B C  | R(A) = RC(B) / RC(C)
0x04 | MOD  A B C  | R(A) = RC(B) % RC(C)
0x05 | UNM  A B    | R(A) = -RC(B)
-----+-------------+----------------------
0x06 | EQ   A B C  | R(A) = RC(B) == RC(C)
0x07 | LE   A B C  | R(A) = RC(B) < RC(C)
0x08 | GT   A B C  | R(A) = RC(B) > RC(C)
-----+-------------+----------------------
0x09 | AND  A B C  | R(A) = RC(B) and RC(C)
0x0A | OR   A B C  | R(A) = RC(B) or  RC(C)
0x0B | NOT  A B    | R(A) = not RC(B)
-----+-------------+----------------------
0x0C | AND  A B C  | R(A) = RC(B) & RC(C)
0x0D | OR   A B C  | R(A) = RC(B) | RC(C)
0x0E | XOR  A B C  | R(A) = RC(B) ^ RC(C)
0x0F | NOT  A B    | R(A) = ~RC(B)
0x10 | LSH  A B C  | R(A) = RC(B) << RC(C)
0x11 | RSH  A B C  | R(A) = RC(B) >> RC(C)
-----+-------------+----------------------
0x12 | JMP  A B    | PC += B
0x13 | CALL A B    | R(A)R(A + 1), ..., R(A+B-1)
0x14 | RET  A B    | return RC(A), ..., RC(A+B-1)
```