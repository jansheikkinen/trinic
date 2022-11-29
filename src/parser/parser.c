/* parser.c */

#include "../debug/debug.h"
#include "../util/readfile.h"
#include "parser.h"
#include "lexer.h"
#include "generateAST.h"

// TODO: Only output one error per statement, so as to prevent errors
// caused by previous errors

// In the future, this should return the bytecode representation of the code
struct DeclarationList* parseProgram(const char* filename, int flags) {
  char* program = readFile(filename);
  struct TokenArray* tokens = tokenise(filename, program);
  free(program);

  if(HAS_FLAG(flags, FLAG_DEBUG) && HAS_FLAG(flags, FLAG_LEX)) {
    printf("\n### LEXER ###\n");
    for(size_t i = 0; i < tokens->length; i++) {
      printf("%03ld: %12s, %10s, (%03ld, %03ld)\n", i,
          getTokenName(tokens->tokens[i].type),
          tokens->tokens[i].literal,
          tokens->tokens[i].row,
          tokens->tokens[i].col);
    }
  }

  if(GET_STAGE(flags) <= FLAG_LEX)
    exit(0);

  struct DeclarationList* decls = generateAST(filename, tokens);

  if(HAS_FLAG(flags, FLAG_DEBUG) && HAS_FLAG(flags, FLAG_AST)) {
    printf("\n### AST GENERATION ###\n");

    for(size_t i = 0; i < decls->size; i++) {
      printDeclarationAST(decls->members[i], 0);
    }
  }

  if(GET_STAGE(flags) <= FLAG_AST)
    exit(0);

  return decls;
}
