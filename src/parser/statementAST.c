/* statementAST.c */

#include <stdio.h>

#include "../error/panic.h"
#include "statementAST.h"

const char* builtinNames[] = {
  "UNDEFINED BUILTIN",
  "print", "return", "continue", "break"
};

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

static struct StmtAST newVarDecl(struct ArgAST* lvalue, struct ArgAST* rvalue) {
  struct StmtAST stmt;

  struct VarDeclStmt vardecl;
  vardecl.lvalue = lvalue;
  vardecl.rvalue = rvalue;

  stmt.type = STMT_VARDECL;
  stmt.as.vardecl = vardecl;

  return stmt;
}

static struct StmtAST newAssign(enum TokenType op, struct ArgAST* lvalue,
    struct ArgAST* rvalue) {
  struct StmtAST stmt;

  struct VarAssignStmt assign;
  assign.op = op;
  assign.lvalue = lvalue;
  assign.rvalue = rvalue;

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

static struct StmtAST newLoop(struct StmtList* body) {
  struct StmtAST stmt;

  struct LoopStmt loop;
  loop.body = body;

  stmt.type = STMT_LOOP;
  stmt.as.loop = loop;

  return stmt;
}

static struct StmtAST newWhile(struct ExprAST* condition,
    struct StmtList* body) {
  struct StmtAST stmt;

  struct WhileStmt loop;
  loop.condition = condition;
  loop.body = body;

  stmt.type = STMT_WHILE;
  stmt.as.whileloop = loop;

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

struct StmtAST* allocNewVarDecl(struct ArgAST* lvalue, struct ArgAST* rvalue) {
  ALLOC_NODE(newVarDecl, lvalue, rvalue);
}

struct StmtAST* allocNewAssign(enum TokenType op, struct ArgAST* lvalue,
    struct ArgAST* rvalue) {
  ALLOC_NODE(newAssign, op, lvalue, rvalue);
}

struct StmtAST* allocNewConditional(struct ExprAST* condition,
    struct StmtList* body, enum ConditionalElseTypes type,
    union ConditionalElse elseBranch) {
  ALLOC_NODE(newConditional, condition, body, type, elseBranch);
}

struct StmtAST* allocNewLoop(struct StmtList* body) {
  ALLOC_NODE(newLoop, body);
}

struct StmtAST* allocNewWhile(struct ExprAST* condition,
    struct StmtList* body) {
  ALLOC_NODE(newWhile, condition, body);
}

void printStmtAST(const struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED:
      printf("STMT_UNDEFINED"); break;
    case STMT_EXPRESSION:
      printExprAST(stmt->as.expression.expression); break;
    case STMT_BUILTIN:
      printf("%s", builtinNames[stmt->as.builtin.type]);
      if(stmt->as.builtin.parameter) {
        printf("("); printExprAST(stmt->as.builtin.parameter); printf(")");
      } break;
    case STMT_VARDECL:
      printf("let ");
      printArgAST(stmt->as.vardecl.lvalue);
      printf(" = ");
      printArgAST(stmt->as.vardecl.rvalue);
      break;
    case STMT_VARASSIGN:
      printArgAST(stmt->as.assignment.lvalue);
      printf(" %s ", getTokenName(stmt->as.assignment.op));
      printArgAST(stmt->as.assignment.rvalue);
      break;
    case STMT_CONDITIONAL:
      printf("if ");
      printExprAST(stmt->as.conditional.condition);
      printf(" do ");
      printStmtList(stmt->as.conditional.body);
      switch(stmt->as.conditional.type) {
        case CONDELSE_UNDEFINED:
          panic(1, "Undefined else branch in conditional node"); break;
        case CONDELSE_ELSE:
          printf("else ");
          printStmtList(stmt->as.conditional.elseBranch.elseBody);
          printf("end"); break;
        case CONDELSE_ELSEIF:
          printf("else ");
          printStmtAST(stmt->as.conditional.elseBranch.elseif); break;
        case CONDELSE_NONE: printf("end"); break;
      }
      break;
    case STMT_LOOP:
      printf("loop ");
      printStmtList(stmt->as.loop.body);
      printf("end"); break;
    case STMT_WHILE:
      printf("while ");
      printExprAST(stmt->as.whileloop.condition);
      printf(" do ");
      printStmtList(stmt->as.whileloop.body);
      printf("end"); break;
  }
}

void freeStmtNode(struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED: break;
    case STMT_EXPRESSION:
      freeExprNode(stmt->as.expression.expression); break;
    case STMT_BUILTIN:
      if(stmt->as.builtin.parameter)
        freeExprNode(stmt->as.builtin.parameter);
      break;
    case STMT_VARDECL:
      freeArgAST(stmt->as.vardecl.lvalue);
      freeArgAST(stmt->as.vardecl.rvalue); break;
    case STMT_VARASSIGN:
      freeArgAST(stmt->as.assignment.lvalue);
      freeArgAST(stmt->as.assignment.rvalue); break;
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
    case STMT_LOOP:
      freeStmtList(stmt->as.loop.body); break;
    case STMT_WHILE:
      freeExprNode(stmt->as.whileloop.condition);
      freeStmtList(stmt->as.whileloop.body); break;
  }

  free(stmt);
}
