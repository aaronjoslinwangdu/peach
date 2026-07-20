#include "ast.h"
#include "memory.h"

#include <string.h>

void init_expr_array(ExprArray *exprs) {
  exprs->count = 0;
  exprs->capacity = 0;
  exprs->entries = NULL;
}

Expr *init_expr(Arena *a, ExprType type, int line) {
  Expr *expr = arena_allocate(a, sizeof(*expr));
  expr->type = type;
  expr->line = line;
  return expr;
}

Expr *init_nil(Arena *a, int line) { return init_expr(a, EXPR_NIL, line); }

Expr *init_boolean(Arena *a, int line, bool value) {
  Expr *boolean = init_expr(a, EXPR_BOOLEAN, line);
  boolean->as.boolean = value;
  return boolean;
}

Expr *init_number(Arena *a, int line, double value) {
  Expr *number = init_expr(a, EXPR_NUMBER, line);
  number->as.number = value;
  return number;
}

// TODO: string interning.. here?
Expr *init_string(Arena *a, int line, char *start, int length) {
  Expr *string = init_expr(a, EXPR_STRING, line);
  char *value = arena_allocate(a, length + 1);
  memcpy(value, start, length);
  value[length] = '\0';
  string->as.string = value;
  return string;
}

Expr *init_var(Arena *a, int line, char *start, int length) {
  Expr *var = init_expr(a, EXPR_VAR, line);
  char *name = arena_allocate(a, length + 1);
  memcpy(name, start, length);
  name[length] = '\0';
  var->as.var.name = name;
  return var;
}

Expr *init_unary(Arena *a, int line, TokenType op, Expr *left) {
  Expr *unary = init_expr(a, EXPR_UNARY, line);
  unary->as.unary.op = op;
  unary->as.unary.left = left;
  return unary;
}

Expr *init_binary(Arena *a, int line, TokenType op, Expr *left, Expr *right) {
  Expr *binary = init_expr(a, EXPR_BINARY, line);
  binary->as.binary.op = op;
  binary->as.binary.left = left;
  binary->as.binary.right = right;
  return binary;
}

Expr *init_grouping(Arena *a, int line, Expr *expr) {
  Expr *grouping = init_expr(a, EXPR_GROUPING, line);
  grouping->as.grouping.expr = expr;
  return grouping;
}

Expr *init_block(Arena *a, int line, ExprArray *exprs) {
  Expr *block = init_expr(a, EXPR_BLOCK, line);
  block->as.block.exprs = exprs;
  return block;
}

Expr *init_function(Arena *a, int line, ExprArray *params, Expr *body,
                    int arity) {
  Expr *function = init_expr(a, EXPR_FUNCTION, line);
  function->as.function.params = params;
  function->as.function.body = body;
  function->as.function.arity = arity;
  return function;
}

Expr *init_call(Arena *a, int line, Expr *callee, ExprArray *args) {
  Expr *call = init_expr(a, EXPR_CALL, line);
  call->as.call.callee = callee;
  call->as.call.args = args;
  return call;
}
