/* statementAST.c */

#include <stdio.h>

#include "../util/printIndent.h"
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

static struct StmtAST newFor(struct ArgAST* itervar, struct ExprAST* iterator,
    struct StmtList* body) {
  struct StmtAST stmt;

  struct ForStmt forloop;
  forloop.itervar = itervar;
  forloop.iterator = iterator;
  forloop.body = body;

  stmt.type = STMT_FOR;
  stmt.as.forloop = forloop;

  return stmt;
}

static struct StmtAST newMatch(struct ExprAST* expr, struct ArgAST* body) {
  struct StmtAST stmt;

  struct MatchStmt match;
  match.expr = expr;
  match.body = body;

  stmt.type = STMT_MATCH;
  stmt.as.match = match;

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

struct StmtAST* allocNewFor(struct ArgAST* itervar, struct ExprAST* iterator,
    struct StmtList* body) {
  ALLOC_NODE(newFor, itervar, iterator, body);
}

struct StmtAST* allocNewMatch(struct ExprAST* expr, struct ArgAST* body) {
  ALLOC_NODE(newMatch, expr, body);
}

void printStmtAST(const struct StmtAST* stmt, size_t indent) {
  PRINT_INDENT(indent); printf("Stmt ");

  switch(stmt->type) {
    case STMT_UNDEFINED:
      printf("UNDEFINED\n"); break;
    case STMT_EXPRESSION:
      printf("EXPRESSION\n");
      printExprAST(stmt->as.expression.expression, indent + 1);
      break;
    case STMT_BUILTIN:
      printf("BUILTIN ");
      printf("%s\n", builtinNames[stmt->as.builtin.type]);

      if(stmt->as.builtin.parameter)
        printExprAST(stmt->as.builtin.parameter, indent + 1);
      break;
    case STMT_VARDECL:
      printf("VARDECL\n");
      printArgAST(stmt->as.vardecl.lvalue, indent + 1);
      printArgAST(stmt->as.vardecl.rvalue, indent + 1);
      break;
    case STMT_VARASSIGN:
      printf("VARASSIGN %s\n", getTokenName(stmt->as.assignment.op));
      printArgAST(stmt->as.assignment.lvalue, indent + 1);
      printArgAST(stmt->as.assignment.rvalue, indent + 1);
      break;
    case STMT_CONDITIONAL:
      printf("IF\n");
      printExprAST(stmt->as.conditional.condition, indent + 1);
      printStmtList(stmt->as.conditional.body, indent + 1);
      switch(stmt->as.conditional.type) {
        case CONDELSE_UNDEFINED:
          PRINT_INDENT(indent + 1); printf("UNDEFINED\n"); break;
        case CONDELSE_ELSE:
          printStmtList(stmt->as.conditional.elseBranch.elseBody, indent + 1);
          break;
        case CONDELSE_ELSEIF:
          printStmtAST(stmt->as.conditional.elseBranch.elseif, indent + 1);
          break;
        case CONDELSE_NONE: PRINT_INDENT(indent + 1); printf("NONE\n"); break;
      } break;
    case STMT_LOOP:
      printf("LOOP\n");
      printStmtList(stmt->as.loop.body, indent + 1);
      break;
    case STMT_WHILE:
      printf("WHILE\n");
      printExprAST(stmt->as.whileloop.condition, indent + 1);
      printStmtList(stmt->as.whileloop.body, indent + 1);
      break;
    case STMT_FOR:
      printf("FOR\n");
      printArgAST(stmt->as.forloop.itervar, indent + 1);
      printExprAST(stmt->as.forloop.iterator, indent + 1);
      printStmtList(stmt->as.forloop.body, indent + 1);
      break;
    case STMT_MATCH:
      printf("MATCH\n");
      printExprAST(stmt->as.match.expr, indent + 1);
      printArgAST(stmt->as.match.body, indent + 1);
      break;
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
    case STMT_FOR:
      freeArgAST(stmt->as.forloop.itervar);
      freeExprNode(stmt->as.forloop.iterator);
      freeStmtList(stmt->as.forloop.body); break;
    case STMT_MATCH:
      freeExprNode(stmt->as.match.expr);
      freeArgAST(stmt->as.match.body); break;
  }

  free(stmt);
}
