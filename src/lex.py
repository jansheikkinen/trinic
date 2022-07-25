#!/bin/python

## UNHOLY C LEXER ##

# The first of multiple tools in the Unholy C compiler toolchain.
# The lexer generates an intermediate representation of the Unholy C language
# from a given source code. Despite being called the lexer, this program
# performs the function of both the lexer and parser. Thus, the next tools
# in the UCC toolchain should be the optimiser, interpreter, or compiler.

import sys
from typing import List
from enum import Enum, auto


## LEXER ##

class TokenType(Enum):
    ## SYMBOLS ##
    LEFT_PARENTHESIS  = auto() # (
    RIGHT_PARENTHESIS = auto() # )
    LEFT_BRACKET  = auto()     # [
    RIGHT_BRACKET = auto()     # ]

    COMMA    = auto() # ,
    DOT      = auto() # .
    QUESTION = auto() # ?
    COLON    = auto() # :

    LEFT_DIAMOND  = auto() # <     wtf is this called
    RIGHT_DIAMOND = auto() # >     just naming it after diamond operator

    PLUS  = auto() # +
    MINUS = auto() # -
    STAR  = auto() # *
    SLASH = auto() # /
    MOD   = auto() # %

    PLUS_ASSIGN  = auto() # +=
    MINUS_ASSIGN = auto() # -=
    STAR_ASSIGN  = auto() # *=
    SLASH_ASSIGN = auto() # /=
    MOD_ASSIGN   = auto() # %=

    BANG_NOT = auto() # !

    BIT_AND = auto() # &
    BIT_OR  = auto() # |
    BIT_NOT = auto() # ~
    BIT_XOR = auto() # ^
    BIT_LEFT  = auto() # <<
    BIT_RIGHT = auto() # >>

    BIT_AND_ASSIGN = auto() # &=
    BIT_OR_ASSIGN  = auto() # |=
    BIT_XOR_ASSIGN = auto() # ^=     no ~ bc thats unary
    BIT_LEFT_ASSIGN  = auto() # <<=
    BIT_RIGHT_ASSIGN = auto() # >>=

    EQUAL      = auto() # ==
    LESS_EQ    = auto() # <=
    GREATER_EQ = auto() # >=
    NOT_EQ     = auto() # !=

    ASSIGNMENT  = auto() # =

    MATCH_ARROW = auto() # =>
    ARROW       = auto() # ->
    AT          = auto() # @

    LENGTH      = auto() # #
    ARRAY       = auto() # []

    ## LITERALS ##
    IDENTIFIER_LITERAL = auto() # like variables and function names
    STRING_LITERAL     = auto() # "FUCK BALLS"
    CHAR_LITERAL       = auto() # 'A'
    INT_LITERAL        = auto() # 69
    FLOAT_LITERAL      = auto() # 420.69 or 69f

    ## KEYWORDS ##
    INT8  = auto() # Basically all just their names
    INT16 = auto()
    INT32 = auto()
    INT64 = auto()
    ISIZE = auto()
    UINT8  = auto()
    UINT16 = auto()
    UINT32 = auto()
    UINT64 = auto()
    USIZE  = auto()
    FLOAT16 = auto()
    FLOAT32 = auto()
    FLOAT64 = auto()
    CHAR   = auto()
    STRING = auto()
    VOID = auto()

    BOOL  = auto()
    TRUE  = auto() # Technically a literal
    FALSE = auto() # This too

    ENUM   = auto()
    UNION  = auto()
    STRUCT = auto()
    ANON   = auto()
    TAGGED = auto()

    FUNCTION = auto()
    INLINE   = auto()

    FOREACH = auto()
    FOR     = auto()
    IN      = auto()
    IF      = auto()
    ELSEIF  = auto() # BC i dont wanna parse "else if"
    ELSE    = auto()
    WHILE   = auto()
    LOOP    = auto()
    MATCH   = auto()
    DO  = auto()
    END = auto()

    INCLUDE = auto()
    EXTERN  = auto()

    AND = auto()
    OR  = auto()
    NOT = auto()

    MUT       = auto()
    UNDEFINED = auto()

    PASS     = auto()
    CONTINUE = auto()
    BREAK    = auto()

    PRINT = auto() # TEMPORARY!! DELETE THIS


class Token:
    def __init__(self, token_type: TokenType, lexeme: str,
            literal, line: int):
        self.token_type = token_type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line

    def __repr__(self):
        return (f"({self.token_type}: \"{self.lexeme}\" | " +
            (f"Literal: {self.literal}, " if self.literal != None else " ") +
            f"Line: {self.line})")


def read_file(filepath: str) -> str:
    with open(filepath, "r") as file:
        return file.read()

lexer_error = False
def lex_program(program: str) -> List[Token]:
    global lexer_error
    lexeme_start = 0
    current_char = 0
    current_line = 1

    tokens: List[Token] = []

    def add_token(token_type: TokenType, literal) -> Token:
        nonlocal lexeme_start
        tokens.append(Token(
            token_type,
            program[lexeme_start:current_char + 1],
            literal, current_line))
        lexeme_start = current_char + 1

    def match_char(current: chr, expected: chr) -> bool:
        nonlocal current_char

        if current_char == proglen: return False
        if current != expected: return False

        current_char += 1
        return True

    def print_error():
        global lexer_error
        print(f"{current_line}, {current_char} [LEXER ERROR]: Invalid token {program[current_char]}")
        lexer_error = True

    def is_digit(char: chr) -> bool:
        return char in "0123456789"

    def is_alpha(char: chr) -> bool:
        return char in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

    def is_alphanumeric(char: chr) -> bool:
        return is_digit(char) or is_alpha(char) or char == '_'

    proglen = len(program)
    while(current_char < proglen):
        char = program[current_char]

        if (current_char + 1) >= proglen: peek = '\0'
        else: peek = program[current_char + 1]

        if (current_char + 2) >= proglen: over = '\0'
        else: over = program[current_char + 2]

        match char:
            ## UNAMBIGUOUS SINGLE CHAR TOKENS ##
            case '(': add_token(TokenType.LEFT_PARENTHESIS, None)
            case ')': add_token(TokenType.RIGHT_PARENTHESIS, None)
            case ']': add_token(TokenType.RIGHT_BRACKET, None)
            case ',': add_token(TokenType.COMMA, None)
            case '.': add_token(TokenType.DOT, None)
            case '?': add_token(TokenType.QUESTION, None)
            case '@': add_token(TokenType.AT, None)
            case '#': add_token(TokenType.LENGTH, None)
            case '~': add_token(TokenType.BIT_NOT, None)
            case ':': add_token(TokenType.COLON, None)

            ## AMBIGIOUS SINGLE OR MULTI CHAR OPERATOR TOKENS ##
            case '[':
                if match_char(peek, ']'): add_token(TokenType.ARRAY, None)
                else: add_token(TokenType.LEFT_BRACKET, None)
            case '<':
                if match_char(peek, '<'):
                    if match_char(over, '='):
                        add_token(TokenType.BIT_LEFT_ASSIGN, None)
                    else: add_token(TokenType.BIT_LEFT, None)
                elif match_char(peek, '='):
                        add_token(TokenType.LESS_EQ, None)
                else: add_token(TokenType.LEFT_DIAMOND, None)
            case '>':
                if match_char(peek, '>'):
                    if match_char(over, '='):
                        add_token(TokenType.BIT_RIGHT_ASSIGN, None)
                    else: add_token(TokenType.BIT_RIGHT, None)
                elif match_char(peek, '='):
                    add_token(TokenType.GREATER_EQ, None)
                else: add_token(TokenType.RIGHT_DIAMOND, None)
            case '=':
                if match_char(peek, '='):
                    add_token(TokenType.EQUAL, None)
                elif match_char(peek, '>'):
                    add_token(TokenType.MATCH_ARROW, None)
                else: add_token(TokenType.ASSIGNMENT, None)
            case '+':
                if match_char(peek, '='):
                    add_token(TokenType.PLUS_ASSIGN, None)
                else: add_token(TokenType.PLUS, None)
            case '-':
                if match_char(peek, '='):
                    add_token(TokenType.MINUS_ASSIGN, None)
                elif match_char(peek, '>'):
                    add_token(TokenType.ARROW, None)
                else: add_token(TokenType.MINUS, None)
            case '*':
                if match_char(peek, '='):
                    add_token(TokenType.STAR_ASSIGN, None)
                else: add_token(TokenType.STAR, None)
            case '/':
                if match_char(peek, '='):
                    add_token(TokenType.SLASH_ASSIGN, None)
                elif match_char(peek, '/'):
                    comment_char = current_char + 2
                    while(comment_char < proglen):
                        if(program[comment_char] == '\n'):
                            current_char = comment_char
                            lexeme_start = current_char + 1
                            current_line += 1
                            break
                        else: comment_char += 1
                else: add_token(TokenType.SLASH, None)
            case '%':
                if match_char(peek, '='):
                    add_token(TokenType.MOD_ASSIGN, None)
                else: add_token(TokenType.MOD, None)
            case '!':
                if match_char(peek, '='):
                    add_token(TokenType.NOT_EQ, None)
                else: add_token(TokenType.BANG_NOT, None)
            case '&':
                if match_char(peek, '='):
                    add_token(TokenType.BIT_AND_ASSIGN, None)
                else: add_token(TokenType.BIT_AND, None)
            case '|':
                if match_char(peek, '='):
                    add_token(TokenType.BIT_OR_ASSIGN, None)
                else: add_token(TokenType.BIT_OR, None)
            case '^':
                if match_char(peek, '='):
                    add_token(TokenType.BIT_XOR_ASSIGN, None)
                else: add_token(TokenType.BIT_XOR, None)

            ## LITERAL TOKENS ##
            case '"':
                str_char = current_char + 2
                while(str_char < proglen):
                    if(program[str_char] == '\n'):
                        current_line += 1
                        str_char += 1
                    elif(program[str_char] == '"'):
                        add_token(
                            TokenType.STRING_LITERAL,
                            program[lexeme_start + 1:str_char])
                        current_char = str_char
                        lexeme_start = current_char + 1
                        break
                    else: str_char += 1
                else:
                    print("[LEXER ERROR]: Unterminated string")
                    print_error()
            case "'":
                chr_char = current_char
                if(chr_char >= proglen or
                    chr_char + 1 >= proglen or
                    chr_char + 2 >= proglen):
                    print("[LEXER ERROR]: Unterminated char: EOF")
                    print_error()

                if(program[chr_char + 2] == "'"):
                    add_token(TokenType.CHAR_LITERAL,
                            program[lexeme_start + 1:chr_char + 2])
                else:
                    print("[LEXER ERROR]: Unterminated char")
                    print_error()

                current_char += 3
                lexeme_start = current_char + 1
            case char if is_digit(char):
                # TODO: Numbers in any base, like Smalltalk's YYrXXXX
                # or even just 0bXXXX 0oXXXX 0xXXXX for binary, octal, and hex
                # TODO: 69f for float
                num_char = current_char
                while(num_char < proglen):
                    while(is_digit(program[num_char])):
                        num_char += 1

                    if(program[num_char] == "." and
                       is_digit(program[num_char + 1])):
                        num_char += 1

                    while(is_digit(program[num_char])):
                        num_char += 1

                    num = program[lexeme_start:num_char]
                    if('.' in num):
                        add_token(TokenType.FLOAT_LITERAL, float(num))
                    else: add_token(TokenType.INT_LITERAL, int(num))

                    #if(program[num_char] == '.'):
                    num_char -= 1

                    current_char = num_char
                    lexeme_start = current_char + 1
                    break

            ## RESERVED KEYWORD TOKENS ##
            case char if is_alphanumeric(char):
                key_char = current_char
                while(is_alphanumeric(program[key_char])):
                    key_char += 1

                keyword = program[lexeme_start:key_char]

                match keyword:
                    case "i8":  add_token(TokenType.INT8,  None)
                    case "i16": add_token(TokenType.INT16, None)
                    case "i32": add_token(TokenType.INT32, None)
                    case "i64": add_token(TokenType.INT64, None)
                    case "u8":  add_token(TokenType.UINT8,  None)
                    case "u16": add_token(TokenType.UINT16, None)
                    case "u32": add_token(TokenType.UINT32, None)
                    case "u64": add_token(TokenType.UINT64, None)
                    case "isize": add_token(TokenType.ISIZE, None)
                    case "usize": add_token(TokenType.USIZE, None)
                    case "f16": add_token(TokenType.FLOAT16, None)
                    case "f32": add_token(TokenType.FLOAT32, None)
                    case "f64": add_token(TokenType.FLOAT64, None)
                    case "char": add_token(TokenType.CHAR, None)
                    case "void": add_token(TokenType.VOID, None)

                    case "bool":  add_token(TokenType.BOOL,  None)
                    case "true":  add_token(TokenType.TRUE,  True)
                    case "false": add_token(TokenType.FALSE, False)

                    case "enum":   add_token(TokenType.ENUM,   None)
                    case "union":  add_token(TokenType.UNION,  None)
                    case "struct": add_token(TokenType.STRUCT, None)
                    case "anon":   add_token(TokenType.ANON,   None)
                    case "tagged": add_token(TokenType.TAGGED, None)

                    case "function": add_token(TokenType.FUNCTION, None)
                    case "inline":   add_token(TokenType.INLINE,   None)

                    case "foreach": add_token(TokenType.FOREACH, None)
                    case "for":     add_token(TokenType.FOR,     None)
                    case "in":      add_token(TokenType.IN,      None)
                    case "if":      add_token(TokenType.IF,      None)
                    case "elseif":  add_token(TokenType.ELSEIF,  None)
                    case "else":    add_token(TokenType.ELSE,    None)
                    case "while":   add_token(TokenType.WHILE,   None)
                    case "loop":    add_token(TokenType.LOOP,    None)
                    case "match":   add_token(TokenType.MATCH,   None)
                    case "do":  add_token(TokenType.DO,  None)
                    case "end": add_token(TokenType.END, None)

                    case "include": add_token(TokenType.INCLUDE, None)
                    case "extern":  add_token(TokenType.EXTERN,  None)

                    case "and": add_token(TokenType.AND, None)
                    case "or":  add_token(TokenType.OR,  None)
                    case "not": add_token(TokenType.NOT, None)

                    case "undefined": add_token(TokenType.UNDEFINED, None)
                    case "mut":       add_token(TokenType.MUT,       None)

                    case "continue": add_token(TokenType.CONTINUE, None)
                    case "break":    add_token(TokenType.BREAK,    None)
                    case "pass":     add_token(TokenType.PASS,     None)

                    case "print": add_token(TokenType.PRINT, None) # DELET THIS

                    case _: add_token(TokenType.IDENTIFIER_LITERAL, keyword)

                current_char = key_char - 1
                lexeme_start = current_char + 1

            ## WHITESPACE AND ERROR ##
            case '\n': current_line += 1; lexeme_start = current_char + 1
            case ' ' | '\t' | '\r': lexeme_start = current_char + 1
            case _: print_error()

        current_char += 1

    return tokens


## PARSER ##

class Expr:
    def __init__(self):
        pass

class Grouping(Expr):
    def __init__(self, expr: Expr):
        self.expr = expr

    def __repr__(self):
        return f"(group {self.expr} )"

class Literal(Expr):
    def __init__(self, literal):
        self.literal = literal

    def __repr__(self):
        return f"{self.literal}"

class BinaryExpr(Expr):
    def __init__(self, left: Expr, operator: Token, right: Expr):
        self.left  = left
        self.op    = operator
        self.right = right

    def __repr__(self):
        return f"( {self.op} {self.left} {self.right} )"

class UnaryExpr(Expr):
    def __init__(self, operator: Token, right: Expr):
        self.op    = operator
        self.right = right

    def __repr__(self):
        return f"( {self.op} {self.right} )"

class VarExpr(Expr):
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return f"( var {self.name} )"

class AssignExpr(Expr):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def __repr(self):
        return f"( assign {self.name} {self.value} )"

class LogicExpr(Expr):
    def __init__(self, left: Expr, operator: Token, right: Expr):
        self.left  = left
        self.op    = operator
        self.right = right

    def __repr__(self):
        return f"( {self.op} {self.left} {self.right} )"

class CallExpr(Expr):
    def __init__(self, callee: Expr, args: List[Expr]):
        self.callee = callee
        self.args = args

    def __repr__(self):
        return f"( call {self.callee} {self.args} )"


class Stmt:
    def __init__(self):
        pass

class ExprStmt(Stmt):
    def __init__(self, expr: Expr):
        self.expr = expr

    def __repr__(self):
        return f"{self.expr}"

class PrintStmt(Stmt):
    def __init__(self, expr: Expr):
        self.expr = expr

    def __repr__(self):
        return f"( print {self.expr} )"

class VarStmt(Stmt):
    def __init__(self, name, initialiser):
        self.name = name
        self.initialiser = initialiser

    def __repr__(self):
        return f"( initVar {self.name}: {self.initialiser} )"

class BlockStmt(Stmt):
    def __init__(self, stmts):
        self.stmts = stmts

    def __repr__(self):
        return f"( block {self.stmts} )"

class IfStmt(Stmt):
    def __init__(self, condition: Expr, then_branch: BlockStmt,
            elseif_conditions: List[Expr], elseif_branches: List[BlockStmt],
            else_branch: BlockStmt):
        self.condition = condition
        self.then_branch = then_branch
        self.elseif_conditions = elseif_conditions
        self.elseif_branches = elseif_branches
        self.else_branch = else_branch

    def __repr__(self):
        str = f"( if {self.condition} {self.then_branch}"
        if self.elseif_conditions:
            for i in range(len(self.elseif_conditions)):
                str += f" ( elseif {self.elseif_conditions[i]} {self.elseif_branches[i]} )"
        if self.else_branch != None:
            str += f" ( else {self.else_branch} )"
        else:
            str += " )"

        return str

class WhileStmt(Stmt):
    def __init__(self, condition: Expr, branch: BlockStmt):
        self.condition = condition
        self.branch = branch

    def __repr__(self):
        return f"( while {self.condition} {self.branch} )"

class FuncStmt(Stmt):
    def __init__(self, name, params, body):
        self.name = name
        self.params = params
        self.body = body

    def __repr__(self):
        return f"( func decl {self.name} {self.params} ( {self.body} ) )"

## TODO: For loops; wanna get a range function first
## Perhaps follow the guide and desugar into a while loop
## Making the placement of this comment a bit questionable lol
## In fact, I reckon that if I add a loop keyword, while would be
## syntactic sugar for loop(as below). Thus: for -> while -> loop

## for i in range 6 do
##   print i * 2.
## end

## while i < 6 do
##   print i * 2.
##   i = i + 1.
## end

## loop
##   if i >= 6 do break. end
##   print i * 2.
##   i = i + 1.
## end

parser_error = False
def parse_tokens(tokens: List[Token]) -> List[Stmt]:
    index = 0

    def is_token(token_type: TokenType) -> bool:
        nonlocal index
        if(index >= len(tokens)):
            return False
        if(tokens[index].token_type == token_type):
            index += 1
            return True
        return False

    def primary() -> Expr:
        global parser_error
        nonlocal index

        if(is_token(TokenType.TRUE)): return Literal(True)
        elif(is_token(TokenType.FALSE)): return Literal(False)

        if(is_token(TokenType.INT_LITERAL) or
            is_token(TokenType.FLOAT_LITERAL) or
            is_token(TokenType.CHAR_LITERAL) or
            is_token(TokenType.STRING_LITERAL)):
            token = tokens[index - 1]
            return Literal(token.literal)

        if(is_token(TokenType.IDENTIFIER_LITERAL)):
            return VarExpr(tokens[index - 1].literal)

        if(is_token(TokenType.LEFT_PARENTHESIS)):
            expr = expression()
            if(not is_token(TokenType.RIGHT_PARENTHESIS)):
                print("[PARSER ERROR]: Unmatched '('")
                parser_error = True

            return Grouping(expr)

        print(f"[PARSER ERROR]: Expected expression on line {tokens[index - 1].line}")
        parser_error = True
        index += 1

    def call() -> Expr:
        return primary()

    def unary() -> Expr:
        if(is_token(TokenType.BIT_NOT) or
            is_token(TokenType.BANG_NOT) or
            is_token(TokenType.NOT) or
            is_token(TokenType.MINUS)):
            token = tokens[index - 1]
            operator = token.token_type
            right = unary()
            return UnaryExpr(operator, right)
        return call()

    def factor() -> Expr:
        left = unary()

        while(is_token(TokenType.SLASH) or
            is_token(TokenType.STAR) or
            is_token(TokenType.MOD)):
            token = tokens[index - 1]
            operator = token.token_type
            right = unary()
            left = BinaryExpr(left, operator, right)

        return left

    def term() -> Expr:
        left = factor()

        while(is_token(TokenType.MINUS) or
            is_token(TokenType.PLUS)):
            token = tokens[index - 1]
            operator = token.token_type
            right = factor()
            left = BinaryExpr(left, operator, right)

        return left

    def bitmath() -> Expr:
        left = term()

        while(is_token(TokenType.BIT_AND) or
            is_token(TokenType.BIT_OR) or
            is_token(TokenType.BIT_XOR) or
            is_token(TokenType.BIT_LEFT) or
            is_token(TokenType.BIT_RIGHT)):
            token = tokens[index - 1]
            operator = token.token_type
            right = term()
            left = BinaryExpr(left, operator, right)

        return left

    def comparison() -> Expr:
        left = bitmath()

        while(is_token(TokenType.LEFT_DIAMOND) or
            is_token(TokenType.RIGHT_DIAMOND) or
            is_token(TokenType.LESS_EQ) or
            is_token(TokenType.GREATER_EQ)):
            token = tokens[index - 1]
            operator = token.token_type
            right = bitmath()
            left = BinaryExpr(left, operator, right)

        return left

    def equality() -> Expr:
        left = comparison()

        while(is_token(TokenType.NOT_EQ) or
                is_token(TokenType.EQUAL)):
            token = tokens[index - 1]
            operator = token.token_type
            right = comparison()
            left = BinaryExpr(left, operator, right)

        return left

    def logic() -> Expr:
        left = equality()

        while(is_token(TokenType.AND) or
                is_token(TokenType.OR)):
            token = tokens[index - 1]
            operator = token.token_type
            right = equality()
            left = LogicExpr(left, operator, right)

        return left

    def assignment() -> Expr:
        expr = logic()

        while(is_token(TokenType.ASSIGNMENT)):
            equals = tokens[index - 1]
            value = assignment()

            if(type(expr) == VarExpr):
                name = expr.name
                return AssignExpr(name, value)
            print("[PARSER ERROR]: Invalid assignment target")
        return expr

    def expression() -> Expr:
        return assignment()

    # TODO: Statements here are defined to end with an explicit . terminator
    # Add support for implicit termination(\n) while keeping support for
    # multi-line statements
    def printStatement() -> PrintStmt:
        global parser_error
        nonlocal index
        expr = expression()
        while(not is_token(TokenType.DOT)):
            if index >= len(tokens):
                parser_error = True
                print("[PARSER ERROR]: Missing statement terminator")
                break
            index += 1
        return PrintStmt(expr)

    def expressionStatement() -> ExprStmt:
        global parser_error
        nonlocal index
        expr = expression()
        while(not is_token(TokenType.DOT)):
            if index >= len(tokens):
                parser_error = True
                print("[PARSER ERROR]: Missing statement terminator")
                break
            index += 1
        return ExprStmt(expr)


    def ifStatement() -> IfStmt:
        global parser_error
        nonlocal index

        condition = expression()

        if not is_token(TokenType.DO):
            print("[PARSER ERROR]: Expected `do` after condition in if")
            parser_error = True

        then_branch = []
        while((not is_token(TokenType.END)) and
                (not is_token(TokenType.ELSE)) and
                (not is_token(TokenType.ELSEIF))):
            if index >= len(tokens):
                parser_error = True
                print("[PARSER ERROR]: If block does not end")
                break
            then_branch.append(declaration())

        elseif_conditions = []
        elseif_branches = []
        while(tokens[index - 1].token_type == TokenType.ELSEIF):
            elseif_conditions.append(expression())
            elseif_branch = []

            if not is_token(TokenType.DO):
                print("[PARSER ERROR]: Expected `do` after condition in elseif")
                parser_error = True

            while((not is_token(TokenType.END)) and
                    (not is_token(TokenType.ELSE)) and
                    (not is_token(TokenType.ELSEIF))):
                if index >= len(tokens):
                    parser_error = True
                    print("[PARSER ERROR]: Elseif block does not end")
                    break
                elseif_branch.append(declaration())
            elseif_branches.append(BlockStmt(elseif_branch))

        else_branch = []
        if tokens[index - 1].token_type == TokenType.ELSE:
            while(not is_token(TokenType.END)):
                if index >= len(tokens):
                    parser_error = True
                    print("[PARSER ERROR]: Else block does not end")
                    break
                else_branch.append(declaration())

        return IfStmt(condition, BlockStmt(then_branch),
                elseif_conditions, elseif_branches, BlockStmt(else_branch))

    def whileStatement() -> WhileStmt:
        global parser_error
        expr = expression()

        if not is_token(TokenType.DO):
            parser_error = True
            print("[PARSER ERROR]: Missing `do` after while condition")

        branch = []
        while(not is_token(TokenType.END)):
            if index >= len(tokens):
                parser_error = True
                print("[PARSER ERROR]: While statement does not end")
                break
            branch.append(declaration())

        return WhileStmt(expr, BlockStmt(branch))

    def function():
        global parser_error

        if not is_token(TokenType.STRING_LITERAL):
            parser_error = True
            print("[PARSER ERROR]: Expected function name after `function`")

        name = tokens[index - 1].literal

        if not is_token(TokenType.COLON):
            parser_error = True
            print("[PARSER ERROR]: Expected `:` after function name")

        params = []
        if tokens[index - 1] == TokenType.VOID:
            if not is_token(TokenType.ARROW):
                parser_error = True
                print("[PARSER ERROR]: Expected `->` after `void`")

        while(is_token(TokenType.INT32) or # It goes without saying, this
            is_token(TokenType.FLOAT32) or # isn't even close to the full
            is_token(TokenType.CHAR) or    # list of types
            is_token(TokenType.STRING) or  # TODO: Type checker and finish
            is_token(TokenType.BOOL)):     # adding types
            if not is_token(TokenType.IDENTIFIER_LITERAL):
                parser_error = True
                print("[PARSER ERROR]: Expected identifier after type")




    def statement() -> Stmt:
        if is_token(TokenType.PRINT):
            return printStatement()
        if is_token(TokenType.IF):
            return ifStatement()
        if is_token(TokenType.WHILE):
            return whileStatement()
        return expressionStatement()


    def variableDeclaration() -> Stmt:
        global parser_error
        nonlocal index

        if is_token(TokenType.IDENTIFIER_LITERAL):
            name = tokens[index - 1]
        else:
            parser_error = True
            print("[PARSER ERROR]: Expected variable name")

        initialiser = None
        if is_token(TokenType.ASSIGNMENT): # TODO: get `undefined` working
            initialiser = expression()

        while(not is_token(TokenType.DOT)):
            if index >= len(tokens):
                parser_error = True
                print("[PARSER ERROR]: Missing statement terminator")
                break
            index += 1
        return VarStmt(name, initialiser)

    def declaration() -> Stmt:
        while(is_token(TokenType.INT32) or # It goes without saying, this
            is_token(TokenType.FLOAT32) or # isn't even close to the full
            is_token(TokenType.CHAR) or    # list of types
            is_token(TokenType.STRING) or  # TODO: Type checker and finish
            is_token(TokenType.BOOL)):     # adding types
            return variableDeclaration()
        while(is_token(TokenType.FUNCTION)): return function()
        return statement() # TODO: Error handling synchronisation


    stmts = []
    while(index < len(tokens)):
        stmts.append(declaration())
    return stmts


## INTERPRETER
runtime_error = False
environment = [{}]

def istype(typ, cmp) -> bool:
    return typ == type(cmp)

def evaluate_tree(tree):
    global runtime_error
    global parser_error
    global environment

    if(type(tree) == PrintStmt):
        value = evaluate_tree(tree.expr)
        if istype(str, value):
            i = 0
            while i < len(value):
                if value[i] == '\\':
                    if value[i + 1] == 'n':
                        i += 1
                        print("")
                else:
                    print(value[i], end="")
                i += 1
        else:
            print(value, end="")

    elif(type(tree) == ExprStmt):
        evaluate_tree(tree.expr)
    elif(type(tree) == VarStmt):
        value = None
        if tree.initialiser != None:
            value = evaluate_tree(tree.initialiser)
        environment[-1][tree.name.literal] = value
    elif(type(tree) == BlockStmt):
        environment.append({})

        for stmt in tree.stmts:
            evaluate_tree(stmt)

        environment.pop()
    elif(type(tree) == IfStmt):
        if len(tree.elseif_conditions) != len(tree.elseif_branches):
            print("[RUNTIME ERROR]: Not same number of elseif conditions and branches")
            runtime_error = True

        evaluated = False
        if(evaluate_tree(tree.condition)):
            evaluate_tree(tree.then_branch)
            evaluated = True
        if(tree.elseif_conditions != None and not evaluated):
            for i in range(len(tree.elseif_conditions)):
                if evaluate_tree(tree.elseif_conditions[i]):
                    evaluate_tree(tree.elseif_branches[i])
                    evaluated = True
        if(tree.else_branch != None and not evaluated):
            evaluate_tree(tree.else_branch)
            evaluated = True
    elif(type(tree) == WhileStmt):
        while(evaluate_tree(tree.condition)):
            evaluate_tree(tree.branch)
    elif(type(tree) == Grouping):
        return evaluate_tree(tree.expr)
    elif(type(tree) == VarExpr):
        for env in reversed(environment):
            if tree.name in env:
                return env[tree.name]
        print(f"[RUNTIME ERROR]: Invalid identifier `{tree.name}`")
        runtime_error = True
    elif(type(tree) == AssignExpr):
        value = evaluate_tree(tree.value)
        for env in reversed(environment):
            if tree.name in env:
                env[tree.name] = value
        return value
    elif(type(tree) == UnaryExpr):
        right = evaluate_tree(tree.right)
        match tree.op:
            case TokenType.BANG_NOT | TokenType.NOT:
                if istype(bool, right):
                    return not right
                else:
                    print("[RUNTIME ERROR]: `!` must take a bool")
                    runtime_error = True
            case TokenType.MINUS:
                if istype(int, right) or istype(float, right):
                    return -right
                else:
                    print("[RUNTIME ERROR]: `-` must be an integer or float")
                    runtime_error = True
            case TokenType.NOT:
                if istype(bool, right):
                    return not right
                else:
                    print("[RUNTIME ERROR]: `not` must take a bool")
                    runtime_error = True
            case TokenType.BIT_NOT:
                if istype(int, right):
                    return ~right
                else:
                    print("[RUNTIME ERROR]: `~` must take a bool")
                    runtime_error = True
    elif(type(tree) == LogicExpr):
        left = evaluate_tree(tree.left)
        right = evaluate_tree(tree.right)
        match tree.op:
            case TokenType.OR:
                if(istype(bool, left) and istype(bool, right)):
                    return left or right
                else:
                    print("[RUNTIME ERROR]: `or` takes two bools")
                    runtime_error = True
            case TokenType.AND:
                if(istype(bool, left) and istype(bool, right)):
                    return left and right
                else:
                    print("[RUNTIME ERROR]: `and` takes two bools")
                    runtime_error = True
    elif(type(tree) == BinaryExpr):
        left = evaluate_tree(tree.left)
        right = evaluate_tree(tree.right)
        match tree.op:
            case TokenType.PLUS:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return left + right
                else:
                    print("[RUNTIME ERROR]: `+` takes two numbers")
                    runtime_error = True
            case TokenType.MINUS:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return left - right
                else:
                    print("[RUNTIME ERROR]: `-` takes two numbers")
                    runtime_error = True
            case TokenType.STAR:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return left * right
                else:
                    print("[RUNTIME ERROR]: `*` takes two numbers")
                    runtime_error = True
            case TokenType.SLASH:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    if right == 0:
                        print("[RUNTIME ERROR]: Division by zero")
                        runtime_error = True
                    else:
                        return left / right
                else:
                    print("[RUNTIME ERROR]: `/` takes two numbers")
                    runtime_error = True
            case TokenType.MOD:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return left % right
                else:
                    print("[RUNTIME ERROR]: `%` takes two numbers")
                    runtime_error = True
            case TokenType.EQUAL:
                return bool(left == right)
            case TokenType.NOT_EQ:
                return bool(left != right)
            case TokenType.LEFT_DIAMOND:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return bool(left < right)
                else:
                    print("[RUNTIME ERROR]: `<` takes two numbers")
                    runtime_error = True
            case TokenType.RIGHT_DIAMOND:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return bool(left > right)
                else:
                    print("[RUNTIME ERROR]: `>` takes two numbers")
                    runtime_error = True
            case TokenType.LESS_EQ:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return bool(left <= right)
                else:
                    print("[RUNTIME ERROR]: `<=` takes two numbers")
                    runtime_error = True
            case TokenType.GREATER_EQ:
                if(istype(int, right) or istype(float, right) and
                    istype(int, left) or istype(float, left)):
                    return bool(left >= right)
                else:
                    print("[RUNTIME ERROR]: `>=` takes two numbers")
                    runtime_error = True
            case TokenType.BIT_AND:
                if(istype(int, right) and istype(int, left)):
                    return left & right
                else:
                    print("[RUNTIME ERROR]: `&` takes two integers")
                    runtime_error = True
            case TokenType.BIT_OR:
                if(istype(int, right) and istype(int, left)):
                    return left | right
                else:
                    print("[RUNTIME ERROR]: `|` takes two integers")
                    runtime_error = True
            case TokenType.BIT_XOR:
                if(istype(int, right) and istype(int, left)):
                    return left ^ right
                else:
                    print("[RUNTIME ERROR]: `^` takes two integers")
                    runtime_error = True
            case TokenType.BIT_LEFT:
                if(istype(int, right) and istype(int, left)):
                    return left << right
                else:
                    print("[RUNTIME ERROR]: `<<` takes two integers")
                    runtime_error = True
            case TokenType.BIT_RIGHT:
                if(istype(int, right) and istype(int, left)):
                    return left >> right
                else:
                    print("[RUNTIME ERROR]: `>>` takes two integers")
                    runtime_error = True

    elif(type(tree) == Literal):
        return tree.literal
    else:
        print(f"[RUNTIME ERROR]: Couldn't parse line")
        runtime_error = True


# Main and its helper functions

def help():
    print("USAGE: lex [SOURCE]")
    print("FLAGS:")
    print("  -h --help                Print this message")
    print("  -o                       Specify output file. Default: lex.out")
    print("  -pipe                    Output to stdout for piping")

def main():
    global lexer_error; global parser_error; global runtime_error

    if len(sys.argv) > 2:
        help()
    elif len(sys.argv) == 2:
        with open(sys.argv[1], "r") as file:
            program = file.read()
        tokens = lex_program(program)

        if not lexer_error:
            nodes = parse_tokens(tokens)
            if not parser_error:
                for node in nodes:
                    if node != None:
                        evaluated = evaluate_tree(node)
    else:
        quit = False
        while(not quit):
            lexer_error = False; parser_error = False; runtime_error = False

            inp = input(">>>  ")
            if inp == "quit" or inp == "exit": quit = not quit; break
            tokens = lex_program(inp + " ")
            print(tokens)

            if not lexer_error:
                nodes = parse_tokens(tokens)
                print(nodes)
                if not parser_error:
                    for node in nodes:
                        if node != None:
                            evaluated = evaluate_tree(node)

if __name__ == "__main__":
    main()
