/* generateStatementAST.c */

#include <stdio.h>

#include "statementAST.h"
#include "generateExpressionAST.h"
#include "generateArgumentAST.h"
#include "generateStatementAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

static struct StmtAST* genBuiltinNode(struct ASTContext* ctx,
    enum BuiltinType builtin) {
  ctx->index += 1;
  struct StmtAST* stmt = NULL;

  struct ExprAST* parameter = NULL;
  if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
    ctx->index += 1;
    parameter = generateExpression(ctx);
    if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) ctx->index += 1;
    else APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_PAREN);
  }

  stmt = allocNewBuiltin(builtin, parameter);
  return stmt;
}

// let x: int, y: float = 69, 42.0. -> int x = 69; float y = 42.0
// let x, y: int = 69, 42.          -> int x = 69; int y = 42
// let x, y: int = 69.              -> int x = 69; int y = 69
static struct StmtAST* genVarDeclNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct ArgAST* lvals = generateIdentifierArguments(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_ASSIGN)) {
    ctx->index += 1;

    struct ArgAST* rvals = generateExpressionArguments(ctx);
    stmt = allocNewVarDecl(lvals, rvals);
  }

  return stmt;
}

static struct StmtAST* genAssignmentNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;

  struct ExprAST* expr = generateExpression(ctx);
  struct ArgAST* lvalue = generateExpressionArgumentsWithFirst(ctx, expr);

  if(MATCH_TOKEN(ctx, TOKEN_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_ADD_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_MINUS_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_STAR_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_SLASH_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_MOD_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_BIT_AND_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_BIT_OR_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_BIT_XOR_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_BIT_LEFT_ASSIGN)
      || MATCH_TOKEN(ctx, TOKEN_BIT_RIGHT_ASSIGN)) {
    enum TokenType op = GET_CURRENT_TOKEN(ctx).type;
    ctx->index += 1;

    struct ArgAST* rvalue = generateExpressionArguments(ctx);
    stmt = allocNewAssign(op, lvalue, rvalue);
  } else return allocNewExpression(expr);

  return stmt;
}

static struct StmtAST* genConditionalNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct ExprAST* condition = generateExpression(ctx);
  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
    ctx->index += 1;
    struct StmtList* body = malloc(sizeof(*body));
    *body = newStmtList();

    enum ConditionalElseTypes type = CONDELSE_NONE;
    union ConditionalElse elseBranch;

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_END)) {
        ctx->index += 1;
        return allocNewConditional(condition, body, type, elseBranch);
      } else if(MATCH_TOKEN(ctx, TOKEN_ELSE)) {
        ctx->index += 1;
        if(MATCH_TOKEN(ctx, TOKEN_IF)) {
          elseBranch.elseif = genConditionalNode(ctx);
          return allocNewConditional(condition, body,
              CONDELSE_ELSEIF, elseBranch);
        } else {
          type = CONDELSE_ELSE;
          elseBranch.elseBody = malloc(sizeof(*elseBranch.elseBody));
          *elseBranch.elseBody = newStmtList();

          while(ctx->index < ctx->tokens->length) {
            if(MATCH_TOKEN(ctx, TOKEN_END)) {
              ctx->index += 1;
              return allocNewConditional(condition, body, type, elseBranch);
            } else {
              appendToStmtList(elseBranch.elseBody, generateStatement(ctx));
            }
          }
        }
      } else {
        appendToStmtList(body, generateStatement(ctx));
      }
    }

    // Missing end token?
    if(ctx->index >= ctx->tokens->length) {
      APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_IF);
      freeStmtList(body);
      if(type == CONDELSE_ELSE) freeStmtList(elseBranch.elseBody);
    }

  }
  return stmt;
}

static struct StmtAST* genLoopNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct StmtList* body = malloc(sizeof(*body));
  *body = newStmtList();

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_END)) {
      ctx->index += 1;
      return allocNewLoop(body);
    } else {
      appendToStmtList(body, generateStatement(ctx));
    }
  }

  if(ctx->index >= ctx->tokens->length) {
    APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_LOOP);
    freeStmtList(body);
  }

  return stmt;
}

static struct StmtAST* genWhileNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct ExprAST* condition = generateExpression(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
    ctx->index += 1;

    struct StmtList* body = malloc(sizeof(*body));
    *body = newStmtList();

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_END)) {
        ctx->index += 1;
        return allocNewWhile(condition, body);
      } else appendToStmtList(body, generateStatement(ctx));
    }

    if(ctx->index >= ctx->tokens->length) {
      APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_WHILE);
      freeStmtList(body);
    }
  }

  return stmt;
}

static struct StmtAST* genMatchNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct ExprAST* expr = generateExpression(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
    ctx->index += 1;

    struct ArgAST* arms = generateMatchArms(ctx);
    if(MATCH_TOKEN(ctx, TOKEN_END)) {
      ctx->index += 1;
      return allocNewMatch(expr, arms);
    }
  }

  return stmt;
}

static struct StmtAST* genStmtNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;

  if(MATCH_TOKEN(ctx, TOKEN_IF)) stmt = genConditionalNode(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_LOOP)) stmt = genLoopNode(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_WHILE)) stmt = genWhileNode(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_MATCH)) stmt = genMatchNode(ctx);
  else {
    // These ones use . or ; as explicit terminator(not sure which yet)
    if(MATCH_TOKEN(ctx, TOKEN_PRINT))
      stmt = genBuiltinNode(ctx, BUILTIN_PRINT);
    else if(MATCH_TOKEN(ctx, TOKEN_RETURN))
      stmt = genBuiltinNode(ctx, BUILTIN_RETURN);
    else if(MATCH_TOKEN(ctx, TOKEN_CONTINUE))
      stmt = genBuiltinNode(ctx, BUILTIN_CONTINUE);
    else if(MATCH_TOKEN(ctx, TOKEN_BREAK))
      stmt = genBuiltinNode(ctx, BUILTIN_BREAK);
    else if(MATCH_TOKEN(ctx, TOKEN_LET))
      stmt = genVarDeclNode(ctx);
    else stmt = genAssignmentNode(ctx); // THIS MUST BE LAST

    if(MATCH_TOKEN(ctx, TOKEN_SEMICOLON)) ctx->index += 1;
    // TODO: Make sure not appending an error is actually ok
    //else APPEND_ASTERROR(ctx, ASTERR_STMT_END);
  }

  return stmt;
}

struct StmtAST* generateStatement(struct ASTContext* ctx) {
  struct StmtAST* stmt = genStmtNode(ctx);

  return stmt;
}
