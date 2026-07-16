#include "ast.h"
#include "memory.h"

#include <string.h>

void init_expr_array(ExprArray *exprs) {
  exprs->count = 0;
  exprs->capacity = 0;
  exprs->entries = NULL;
}

Expr *init_expr(Arena *arena, ExprType type, int line) {
  Expr *expr = arena_allocate(arena, sizeof(*expr));
  expr->type = type;
  expr->line = line;
  return expr;
}

Expr *init_nil(Arena *arena, int line) {
  return init_expr(arena, EXPR_NIL, line);
}

Expr *init_boolean(Arena *arena, int line, bool value) {
  Expr *boolean = init_expr(arena, EXPR_BOOLEAN, line);
  boolean->as.boolean = value;
  return boolean;
}

Expr *init_number(Arena *arena, int line, double value) {
  Expr *number = init_expr(arena, EXPR_NUMBER, line);
  number->as.number = value;
  return number;
}

// TODO: string interning.. here?
Expr *init_string(Arena *arena, int line, char *start, int length) {
  Expr *string = init_expr(arena, EXPR_STRING, line);
  char *value = arena_allocate(arena, length + 1);
  memcpy(value, start, length);
  value[length] = '\0';
  string->as.string = value;
  return string;
}

Expr *init_var(Arena *arena, int line, char *start, int length) {
  Expr *var = init_expr(arena, EXPR_VAR, line);
  char *name = arena_allocate(arena, length + 1);
  memcpy(name, start, length);
  name[length] = '\0';
  var->as.var.name = name;
  return var;
}

Expr *init_unary(Arena *arena, int line, TokenType op, Expr *left) {
  Expr *unary = init_expr(arena, EXPR_UNARY, line);
  unary->as.unary.op = op;
  unary->as.unary.left = left;
  return unary;
}

Expr *init_binary(Arena *arena, int line, TokenType op, Expr *left,
                  Expr *right) {
  Expr *binary = init_expr(arena, EXPR_BINARY, line);
  binary->as.binary.op = op;
  binary->as.binary.left = left;
  binary->as.binary.right = right;
  return binary;
}

Expr *init_grouping(Arena *arena, int line, Expr *expr) {
  Expr *grouping = init_expr(arena, EXPR_GROUPING, line);
  grouping->as.grouping.expr = expr;
  return grouping;
}

Expr *init_block(Arena *arena, int line, ExprArray *exprs) {
  Expr *block = init_expr(arena, EXPR_BLOCK, line);
  block->as.block.exprs = exprs;
  return block;
}

Expr *init_function(Arena *arena, int line, ExprArray *params, Expr *body,
                    int arity) {
  Expr *function = init_expr(arena, EXPR_FUNCTION, line);
  function->as.function.params = params;
  function->as.function.body = body;
  function->as.function.arity = arity;
  return function;
}
