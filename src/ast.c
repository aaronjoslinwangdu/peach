#include "ast.h"
#include "memory.h"

#define ALLOCATE_EXPR(type, exprType, line)                                    \
  (type *)allocate_expr(sizeof(type), (exprType), (line))

Expr *allocate_expr(size_t size, ExprType type, int line) {
  Expr *expr = (Expr *)reallocate(NULL, 0, size);
  expr->type = type;
  expr->line = line;
  return expr;
}

Expr *new_group(int line, Expr *expr);
Expr *new_block(int line, Expr **exprs);
Expr *new_call(int line, Expr *name, Expr **args);
Expr *new_unary(int line, TokenType op, Expr *expr);
Expr *new_binary(int line, TokenType op, Expr *left, Expr *right);
Expr *new_var(int line, const char *start, int length);
Expr *new_string(int line, const char *start, int length);
