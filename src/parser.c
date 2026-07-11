#include "ast.h"
#include "debug.h"
#include "memory.h"
#include "token.h"

typedef struct {
  int current;
  TokenArray *t;
  ExprArray *e;
  bool has_error;
} Parser;

static void init_parser(Parser *p, TokenArray *t, ExprArray *e) {
  p->current = 0;
  p->t = t;
  p->e = e;
  p->has_error = false;
}

static Token peek(Parser *p) { return p->t->entries[p->current]; }

static bool is_end(Parser *p) { return peek(p).type == TOKEN_EOF; }

static Token advance(Parser *p) { return p->t->entries[p->current++]; }

static bool match(Parser *p, TokenType type) { return peek(p).type == type; }

static Expr new_group(int line, Expr *expr) {
  return (Expr){.type = EXPR_GROUP, .line = line, .as.group.expr = expr};
}

static Expr new_boolean(int line, bool val) {
  return (Expr){.type = EXPR_BOOLEAN, .line = line, .as.boolean = val};
}

static Expr new_nil(int line) { return (Expr){.type = EXPR_NIL, .line = line}; }

static Expr expression(Parser *p);

static Expr literal(Parser *p) {
  Token current = peek(p);
  switch (current.type) {
  case TOKEN_TRUE:
    return new_boolean(current.line, true);
  case TOKEN_FALSE:
    return new_boolean(current.line, false);
  default:
    return new_nil(current.line);
  }
}

static Expr group(Parser *p) {
  if (match(p, TOKEN_LEFT_PAREN)) {
    advance(p);
    int line = peek(p).line;
    Expr e = expression(p);
    advance(p);
    return new_group(line, &e);
  }
  return literal(p);
}

static Expr expression(Parser *p) {
  Expr expr = group(p);
  advance(p);
  return expr;
}

void parse(TokenArray *t, ExprArray *e) {
  Parser p;
  init_parser(&p, t, e);
  while (!is_end(&p)) {
    Expr expr = expression(&p);
#ifdef _LANG_DEBUG_PARSER
    print_expr(&expr);
#endif
    DYN_ARR_PUSH(Expr, e, expr);
  }
};
