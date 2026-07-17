#include "ast.h"
#include "debug.h"
#include "memory.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int current;
  TokenArray *tokens;
  Arena *arena;
  bool has_error;
} Parser;

static void init_parser(Parser *p, TokenArray *tokens, Arena *arena) {
  p->current = 0;
  p->tokens = tokens;
  p->arena = arena;
  p->has_error = false;
}

static Token previous(Parser *p) { return p->tokens->entries[p->current - 1]; }

static Token peek(Parser *p) { return p->tokens->entries[p->current]; }

static bool is_end(Parser *p) { return peek(p).type == TOKEN_EOF; }

static Token advance(Parser *p) {
  if (!is_end(p))
    p->current++;
  return previous(p);
}

static bool check(Parser *p, TokenType type) {
  if (is_end(p))
    return false;
  return peek(p).type == type;
}

static bool match(Parser *p, TokenType type) {
  if (check(p, type)) {
    advance(p);
    return true;
  }
  return false;
}

static void consume_delim(Parser *p) {
  switch (peek(p).type) {
  case TOKEN_RIGHT_BRACE:
  case TOKEN_EOF:
    return;
  case TOKEN_DELIM:
    advance(p);
    return;
  default: {
    Token t = peek(p);
    // TODO: unwind error here, don't exit
    fprintf(stderr, "%s\n", "Expected delimiter.");
    print_token(&t);
    exit(EXIT_FAILURE);
  }
  }
}

static void consume(Parser *p, TokenType type, const char *error_message) {
  if (!match(p, type)) {
    Token t = peek(p);
    // TODO: unwind error here, don't exit
    fprintf(stderr, "%s\n", error_message);
    print_token(&t);
    exit(EXIT_FAILURE);
  }
}

static Expr *expression(Parser *p);

static Expr *literal(Parser *p) {
  Token cur = peek(p);

  if (match(p, TOKEN_TRUE)) {
    return init_boolean(p->arena, cur.line, true);
  }
  if (match(p, TOKEN_FALSE)) {
    return init_boolean(p->arena, cur.line, false);
  }
  if (match(p, TOKEN_NUMBER)) {
    double num = strtod(cur.start, NULL);
    return init_number(p->arena, cur.line, num);
  }
  if (match(p, TOKEN_STRING)) {
    return init_string(p->arena, cur.line, (char *)cur.start, cur.length);
  }
  if (match(p, TOKEN_LEFT_PAREN)) {
    Expr *expr = expression(p);
    consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
    return init_grouping(p->arena, cur.line, expr);
  }
  // TODO: list
  // TODO: tuple
  // TODO: map
  if (match(p, TOKEN_IDENTIFIER)) {
    return init_var(p->arena, cur.line, (char *)cur.start, cur.length);
  }
  if (match(p, TOKEN_NIL)) {
    return init_nil(p->arena, cur.line);
  }

  // TODO: unwind error here, don't exit
  fprintf(stderr, "%s\n", "Failed to match identifier.");
  print_token(&cur);
  exit(EXIT_FAILURE);
}

static Expr *unary(Parser *p) {
  if (match(p, TOKEN_MINUS) || match(p, TOKEN_BANG)) {
    Token prev = previous(p);
    Expr *expr = unary(p);
    return init_unary(p->arena, prev.line, prev.type, expr);
  }
  return literal(p);
}

static Expr *pattern_match(Parser *p) {
  Expr *expr = unary(p);
  if (match(p, TOKEN_EQUAL)) {
    int line = previous(p).line;
    Expr *right = unary(p);
    expr = init_binary(p->arena, line, TOKEN_EQUAL, expr, right);
  }
  return expr;
}

static Expr *factor(Parser *p) {
  Expr *expr = pattern_match(p);
  while (match(p, TOKEN_STAR) || match(p, TOKEN_SLASH)) {
    Token prev = previous(p);
    Expr *right = pattern_match(p);
    expr = init_binary(p->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *term(Parser *p) {
  Expr *expr = factor(p);
  while (match(p, TOKEN_PLUS) || match(p, TOKEN_MINUS)) {
    Token prev = previous(p);
    Expr *right = factor(p);
    expr = init_binary(p->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *comparison(Parser *p) {
  Expr *expr = term(p);
  while (match(p, TOKEN_LESS) || match(p, TOKEN_LESS_EQUAL) ||
         match(p, TOKEN_GREATER) || match(p, TOKEN_GREATER_EQUAL)) {
    Token prev = previous(p);
    Expr *right = term(p);
    expr = init_binary(p->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *equality(Parser *p) {
  Expr *expr = comparison(p);
  while (match(p, TOKEN_EQUAL_EQUAL) || match(p, TOKEN_BANG_EQUAL)) {
    Token prev = previous(p);
    Expr *right = comparison(p);
    expr = init_binary(p->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *_and(Parser *p) {
  Expr *expr = equality(p);
  while (match(p, TOKEN_AND)) {
    Expr *right = equality(p);
    expr = init_binary(p->arena, previous(p).line, TOKEN_AND, expr, right);
  }
  return expr;
}

static Expr *_or(Parser *p) {
  Expr *expr = _and(p);
  while (match(p, TOKEN_OR)) {
    Expr *right = _and(p);
    expr = init_binary(p->arena, previous(p).line, TOKEN_OR, expr, right);
  }
  return expr;
}

static Expr *block(Parser *p) {
  if (match(p, TOKEN_LEFT_BRACE)) {
    int line = previous(p).line;
    consume_delim(p);
    ExprArray *exprs = arena_allocate(p->arena, sizeof(*exprs));
    init_expr_array(exprs);
    while (!match(p, TOKEN_RIGHT_BRACE)) {
      if (is_end(p)) {
        // TODO: unwind error here, don't exit
        fprintf(stderr, "%s\n", "Expect '}'.");
        exit(EXIT_FAILURE);
      }
      Expr *expr = expression(p);
      DYN_ARR_PUSH(Expr *, exprs, expr);
      consume_delim(p);
    }
    return init_block(p->arena, line, exprs);
  }
  return _or(p);
}

static Expr *function(Parser *p) {
  if (match(p, TOKEN_FUNCTION)) {
    int line = previous(p).line;
    consume(p, TOKEN_LEFT_PAREN, "Expect '('.");
    ExprArray *params = arena_allocate(p->arena, sizeof(*params));
    init_expr_array(params);
    int arity = 0;
    while (!match(p, TOKEN_RIGHT_PAREN)) {
      if (is_end(p)) {
        // TODO: unwind error here, don't exit
        fprintf(stderr, "%s\n", "Expect ')'.");
        exit(EXIT_FAILURE);
      }
      arity += 1;
      Expr *param = unary(p);
      DYN_ARR_PUSH(Expr *, params, param);
      if (!check(p, TOKEN_RIGHT_PAREN))
        consume(p, TOKEN_COMMA, "Expect ','.");
    }
    consume(p, TOKEN_ARROW, "Expect '->'.");
    Expr *body = block(p);
    return init_function(p->arena, line, params, body, arity);
  }
  return block(p);
}

static Expr *expression(Parser *p) { return function(p); }

void parse(TokenArray *tokens, Arena *arena) {
  Parser parser;
  init_parser(&parser, tokens, arena);
  while (!is_end(&parser)) {
    Expr *expr = expression(&parser);
    consume_delim(&parser);
#ifdef _PEACH_DEBUG_PARSER
    print_expr(expr, 0);
#endif
  }
};
