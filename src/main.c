/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <argz.h>
#include "debug/debug.h"
#include "parser/parser.h"

struct Arguments {
  int flags;
  char* argz;
  size_t argz_len;
};

struct Arguments args = { 0, NULL, 0 };

static int parseopt(int key, char* arg, struct argp_state *state) {
  switch(key) {
    case 'd': args.flags |= FLAG_DEBUG; break;
    case 'l': args.flags |= FLAG_LEX;   break;
    case 'a': args.flags |= FLAG_AST;   break;
    case 'b': args.flags |= FLAG_BC;    break;
    case ARGP_KEY_ARG:
      argz_add(&args.argz, &args.argz_len, arg);
      break;
    case ARGP_KEY_INIT:
      args.argz = 0;
      args.argz_len = 0;
      break;
    case ARGP_KEY_END:
      if(args.argz_len < 2) argp_failure(state, 1, 0, "too few arguments");
      break;
  }
  return 0;
}

int main(int argc, char** argv) {
  struct argp_option options[] = {
    { "debug", 'd', NULL, 0, "Print debug information", 0 },
    { "lex", 'l', NULL, 0, "Stop after generating tokens, ast, and bytecode, respectively. -l, -a, and -b are mutually exclusive; the program will exit after the latest given stage. When used in conjunction with -d, it prints the relevant debug info for the stage", 0 },
    { "ast", 'a', NULL, OPTION_ALIAS, NULL, 0 },
    { "bytecode", 'b', NULL, OPTION_ALIAS, NULL, 0 },
    { 0 }
  };

  int arg_count = 2;
  struct argp argp = { options, parseopt, "FILE", "Compiler for Trinic programs", NULL, NULL, NULL };

  if(argp_parse(&argp, argc, argv, 0, NULL, &arg_count) == 0) {
    const char* filename = argz_next(args.argz, args.argz_len, NULL);
    struct DeclarationList* decls = parseProgram(filename, args.flags);
    FREE_SELF_AND_MEMBERS(decls, freeDeclarationAST);
  }
  return 0;
}
