# Roadmap

A general list of features likely to be implemented to the project.

The document is split into two lists: one describing language-oriented features,
and the other describing compiler/interpreter-specific features. For example,
the former would include implementation of types of functions, while the latter
would encompass implementation of an AST or optimisation.

## Language

- [x] Expressions
- [x] Statements
- [x] Variable Initialisation and Assignment
- [ ] Variable Scopes
- [x] Control Flow: `if`, `and`, `or`, `not`
- [x] Loops: `loop`, `while`, `for`, `foreach`
- [ ] `break`, `continue`
- [ ] Typechecking of Primitive Types
- [ ] Functions
- [ ] Function Overloading
- [x] Pointers and Arrays
- [x] `struct`
- [ ]`enum`, `union`
- [ ] `include` and `extern`
- [ ] `undefined` and Mutability
- [ ] Pattern Matching
- [ ] Anonymous struct/enum/union
- [ ] Tagged unions(?)
- [ ] Macro system akin to Rust or [Nelua](https://github.com/edubart/nelua-lang)
- [ ] Compiler/interpreter directives / attributes

## Build Tools

- [x] Lexer
- [ ] Parser
  - [ ] AST Generation
- [ ] Code optimisation
- [ ] Compilation to ASM or bytecode
- [ ] Compiler Directives(in source) and Flags

### Compiler-specific

- [ ] Compile to a x86-64 Linux target

### Interpreter-specific

- [ ] Design a bytecode architecture
