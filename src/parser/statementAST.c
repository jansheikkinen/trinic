/* statementAST.c */

#include <stdio.h>

#include "../error/panic.h"
#include "statementAST.h"

static struct StmtAST newExpression(struct ExprAST* expr) {
  struct StmtAST stmt;

  struct ExprStmt expression;
  expression.expression = expr;

  stmt.type = STMT_EXPRESSION;
  stmt.as.expression = expression;

  return stmt;
}

static struct StmtAST newBuiltin(enum BuiltinType type, struct ExprAST* parameter) {
  struct StmtAST stmt;

  struct BuiltinStmt builtin;
  builtin.type = type;
  builtin.parameter = parameter;

  stmt.type = STMT_BUILTIN;
  stmt.as.builtin = builtin;

  return stmt;
}

// I promise VarDecl and Assign are(will be) different
static struct StmtAST newVarDecl(const char* identifier, struct ExprAST* expr) {
  struct StmtAST stmt;

  struct VarDeclStmt vardecl;
  vardecl.identifier = identifier;
  vardecl.value = expr;

  stmt.type = STMT_VARDECL;
  stmt.as.vardecl = vardecl;

  return stmt;
}

static struct StmtAST newAssign(const char* identifier, struct ExprAST* expr) {
  struct StmtAST stmt;

  struct VarAssignStmt assign;
  assign.identifier = identifier;
  assign.value = expr;

  stmt.type = STMT_VARASSIGN;
  stmt.as.assignment = assign;

  return stmt;
}

static struct StmtAST newConditional(struct ExprAST* condition,
    struct StmtList* body, enum ConditionalElseTypes type,
    union ConditionalElse elseBranch) {
  struct StmtAST stmt;

  struct ConditionalStmt cond;
  cond.condition = condition;
  cond.body = body;
  cond.type = type;
  cond.elseBranch = elseBranch;

  stmt.type = STMT_CONDITIONAL;
  stmt.as.conditional = cond;

  return stmt;
}

// I initially wanted to put the entire function declaration in the macro,
// but that requires two separate VA_ARGS :(
#define ALLOC_NODE(func, ...) \
  struct StmtAST* ptr = malloc(sizeof(*ptr)); \
  if(ptr) *ptr = func(__VA_ARGS__); \
  return ptr \

struct StmtAST* allocNewExpression(struct ExprAST* expr) {
  ALLOC_NODE(newExpression, expr);
}

struct StmtAST* allocNewBuiltin(enum BuiltinType type, struct ExprAST* expr) {
  ALLOC_NODE(newBuiltin, type, expr);
}

struct StmtAST* allocNewVarDecl(const char* identifier,
    struct ExprAST* expr) {
  ALLOC_NODE(newVarDecl, identifier, expr);
}

struct StmtAST* allocNewAssign(const char* identifier, struct ExprAST* expr) {
  ALLOC_NODE(newAssign, identifier, expr);
}

struct StmtAST* allocNewConditional(struct ExprAST* condition,
    struct StmtList* body, enum ConditionalElseTypes type,
    union ConditionalElse elseBranch) {
  ALLOC_NODE(newConditional, condition, body, type, elseBranch);
}

void printStmtAST(const struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED:
      printf("(STMT_UNDEFINED)"); break;
    case STMT_EXPRESSION:
      printf("(EXPR ");
      printExprAST(stmt->as.expression.expression);
      printf(")"); break;
    case STMT_BUILTIN:
      printf("(BUILTIN_%d ", stmt->as.builtin.type);
      printExprAST(stmt->as.builtin.parameter);
      printf(")"); break;
    case STMT_VARDECL:
      printf("(VARDECL (%s) ", stmt->as.vardecl.identifier);
      printExprAST(stmt->as.vardecl.value);
      printf(")"); break;
    case STMT_VARASSIGN:
      printf("VARASSIGN %s (", stmt->as.assignment.identifier);
      printExprAST(stmt->as.assignment.value);
      printf("))"); break;
    case STMT_CONDITIONAL:
      printf("(IF (");
      printExprAST(stmt->as.conditional.condition);
      printf(") (");
      for(size_t i = 0; i < stmt->as.conditional.body->size; i++) {
        printStmtAST(stmt->as.conditional.body->stmts[i]);
      }
      printf(") (");
      switch(stmt->as.conditional.type) {
        case CONDELSE_UNDEFINED:
          panic(1, "Undefined else branch in conditional node"); break;
        case CONDELSE_ELSE:
          for(size_t i = 0;
              i < stmt->as.conditional.elseBranch.elseBody->size; i++) {
            printStmtAST(stmt->as.conditional.elseBranch.elseBody->stmts[i]);
          } break;
        case CONDELSE_ELSEIF:
          printStmtAST(stmt->as.conditional.elseBranch.elseif); break;
        case CONDELSE_NONE: break;
      }
      printf("))");
      break;
  }
}

void freeStmtNode(struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED: break;
    case STMT_EXPRESSION:
      freeExprNode(stmt->as.expression.expression); break;
    case STMT_BUILTIN:
      freeExprNode(stmt->as.builtin.parameter); break;
    case STMT_VARDECL: // uhh do i free const char*s; dont think so lol
      freeExprNode(stmt->as.vardecl.value); break;
    case STMT_VARASSIGN:
      freeExprNode(stmt->as.assignment.value); break;
    case STMT_CONDITIONAL:
      freeStmtList(stmt->as.conditional.body);
      switch(stmt->as.conditional.type) {
        case CONDELSE_UNDEFINED:
          panic(1, "Undefined else branch in conditional node"); break;
        case CONDELSE_ELSE:
          freeStmtList(stmt->as.conditional.elseBranch.elseBody); break;
        case CONDELSE_ELSEIF:
          freeStmtNode(stmt->as.conditional.elseBranch.elseif); break;
        case CONDELSE_NONE: break;
      }
      freeExprNode(stmt->as.conditional.condition); break;
  }

  free(stmt);
}
