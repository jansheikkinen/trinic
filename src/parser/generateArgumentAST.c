/* generateArgumentAST.c */

#include "../error/error.h"
#include "../util/arraylist.h"
#include "argumentAST.h"
#include "generateStatementAST.h"
#include "generateTypeAST.h"
#include "generateExpressionAST.h"
#include "stmtlist.h"

// Differentiating these two in a single function as I have in expression or
// statement generation is actually a bit more involved to implement than I
// would like. Luckily, these two can never appear in the same spot, so I can
// just call the relevant function when it is needed. :)

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) (GET_CURRENT_TOKEN(ctx).type == token)

struct ArgAST* generateExpressionArgumentsWithFirst(struct ASTContext* ctx,
    struct ExprAST* arg) {
  struct ArgAST* ast = NULL;

  struct ExprList* args = malloc(sizeof(*args));
  NEW_ARRAYLIST(args);
  APPEND_ARRAYLIST(args, arg);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      APPEND_ARRAYLIST(args, generateExpression(ctx));
    } else return allocNewExprArgList(args);
  }

  return ast;
}

struct ArgAST* generateExpressionArguments(struct ASTContext* ctx) {
  struct ArgAST* ast = NULL;

  struct ExprAST* expr = generateExpression(ctx);
  struct ExprList* args = malloc(sizeof(*args));
  NEW_ARRAYLIST(args);
  APPEND_ARRAYLIST(args, expr);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      expr = generateExpression(ctx);
      APPEND_ARRAYLIST(args, expr);
    } else return allocNewExprArgList(args);
  }

  if(ctx->index >= ctx->tokens->length) {
    APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_ARGS);
    FREE_SELF_AND_MEMBERS(args, freeExprNode);
  }

  return ast;
}

static struct IdentifierArg* generateIdentifierArg(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_COLON)) {
      ctx->index += 1;

      struct TypeAST* type = generateType(ctx);
      return allocNewIdentifierArg(identifier, type);
    } else return allocNewIdentifierArg(identifier, NULL);
  } else {
    APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER);
  }
  return NULL;
}

struct ArgAST* generateIdentifierArguments(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    struct ArgAST* ast = allocNewIdentifierArgList();

    struct IdentifierArg* arg = generateIdentifierArg(ctx);
    APPEND_ARRAYLIST(&ast->as.idargs, arg);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;

        arg = generateIdentifierArg(ctx);
        APPEND_ARRAYLIST(&ast->as.idargs, arg);
      } else return ast;
   }

    if(ctx->index >= ctx->tokens->length) {
      APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_ARGS);
      freeArgAST(ast);
    }
  }

  return NULL;
}

static struct AssignArg* generateAssignArg(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_ASSIGN)) {
      ctx->index += 1;

      struct ExprAST* rval = generateExpression(ctx);
      return allocNewAssignArg(identifier, rval);
    } else return allocNewAssignArg(identifier, NULL);
  }

  return NULL;
}

struct ArgAST* generateAssignmentArguments(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    struct ArgAST* ast = allocNewAssignArgList();

    struct AssignArg* arg = generateAssignArg(ctx);
    APPEND_ARRAYLIST(&ast->as.assignargs, arg);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;
        arg = generateAssignArg(ctx);
        APPEND_ARRAYLIST(&ast->as.assignargs, arg);
      } else return ast;
    }
  }

  return NULL;
}

struct ArgAST* generateGenericArguments(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    struct ArgAST* ast = allocNewGenericList();

    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    APPEND_ARRAYLIST(&ast->as.generics, identifier);
    ctx->index += 1;

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;

        if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
          identifier = GET_CURRENT_TOKEN(ctx).literal;
          ctx->index += 1;
          APPEND_ARRAYLIST(&ast->as.generics, identifier);
        }
      } else return ast;
    }
  }

  return NULL;
}

struct ArgAST* genSumTypes(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewSumArgTypeList();
  const char* identifier;
  struct TypeAST* type;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;
    APPEND_ARRAYLIST(&ast->as.sumtypes, allocNewSumArgTypeStr(identifier));
  } else {
    type = generateType(ctx);
    APPEND_ARRAYLIST(&ast->as.sumtypes, allocNewSumArgTypeType(type));
  }

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;

      if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
        identifier = GET_CURRENT_TOKEN(ctx).literal;
        ctx->index += 1;
        APPEND_ARRAYLIST(&ast->as.sumtypes, allocNewSumArgTypeStr(identifier));
      } else {
        type = generateType(ctx);
        APPEND_ARRAYLIST(&ast->as.sumtypes, allocNewSumArgTypeType(type));
      }
    } else return ast;
  }

  return NULL;
}

struct ArgAST* generateSumArguments(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewSumArgList();

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* name = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
      ctx->index += 1;
      struct ArgAST* fields = genSumTypes(ctx);
      if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
        ctx->index += 1;
        APPEND_ARRAYLIST(&ast->as.sums, allocNewSumArg(name, fields));
      }
    }

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;

        if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
          name = GET_CURRENT_TOKEN(ctx).literal;
          ctx->index += 1;

          if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
            ctx->index += 1;
            struct ArgAST* fields = genSumTypes(ctx);
            if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
              ctx->index += 1;
              APPEND_ARRAYLIST(&ast->as.sums, allocNewSumArg(name, fields));
            }
          } else APPEND_ARRAYLIST(&ast->as.sums, allocNewSumArg(name, NULL));
        }
      } else return ast;
    }
  }

  return NULL;
}

static struct ArgAST* generateGenericDefLeft(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewGenericDefLeft();

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;
    APPEND_ARRAYLIST(&ast->as.genleft, identifier);
  }
  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_ADD)) {
      ctx->index += 1;

      if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
        const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
        ctx->index += 1;
        APPEND_ARRAYLIST(&ast->as.genleft, identifier);
      }
    } else return ast;
  }

  return NULL;
}

static struct ArgAST* generateGenericDefRight(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewGenericDefRight();

  struct TypeAST* type = generateType(ctx);
  APPEND_ARRAYLIST(&ast->as.genright, type);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_ADD)) {
      ctx->index += 1;

      type = generateType(ctx);
      APPEND_ARRAYLIST(&ast->as.genright, type);
    } else return ast;
  }

  return NULL;
}

struct ArgAST* generateGenericDefs(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewGenericDefs();

  struct ArgAST* gendefl = generateGenericDefLeft(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_COLON)) {
    ctx->index += 1;
    struct ArgAST* gendefr = generateGenericDefRight(ctx);
    APPEND_ARRAYLIST(&ast->as.gendefs, allocNewGenericDef(gendefl, gendefr));
  }

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      gendefl = generateGenericDefLeft(ctx);

      if(MATCH_TOKEN(ctx, TOKEN_COLON)) {
        ctx->index += 1;
        struct ArgAST* gendefr = generateGenericDefRight(ctx);
        APPEND_ARRAYLIST(&ast->as.gendefs,
            allocNewGenericDef(gendefl, gendefr));
      }
    } else return ast;
  }

  return NULL;
}

static struct MatchArm* generateMatchArm(struct ASTContext* ctx) {
  struct ArgAST* exprargs = generateExpressionArguments(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_MATCH_ARROW)) {
    ctx->index += 1;

    struct StmtList* body = malloc(sizeof(*body));
    *body = newStmtList();

    if(MATCH_TOKEN(ctx, TOKEN_DO)) {
      ctx->index += 1;

      while(ctx->index < ctx->tokens->length) {
        if(MATCH_TOKEN(ctx, TOKEN_END)) {
          ctx->index += 1;
          return allocNewMatchArm(exprargs, body);
        } else appendToStmtList(body, generateStatement(ctx));
      }
    } else {
      appendToStmtList(body, generateStatement(ctx));
      return allocNewMatchArm(exprargs, body);
    }
  }

  return NULL;
}

struct ArgAST* generateMatchArms(struct ASTContext* ctx) {
  struct ArgAST* ast = allocNewMatchArms();

  struct MatchArm* arm = generateMatchArm(ctx);
  APPEND_ARRAYLIST(&ast->as.matcharms, arm);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      arm = generateMatchArm(ctx);
      APPEND_ARRAYLIST(&ast->as.matcharms, arm);
    } else return ast;
  }

  return NULL;
}
