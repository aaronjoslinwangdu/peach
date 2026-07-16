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

static void init_parser(Parser *parser, TokenArray *tokens, Arena *arena) {
  parser->current = 0;
  parser->tokens = tokens;
  parser->arena = arena;
  parser->has_error = false;
}

static Token previous(Parser *parser) {
  return parser->tokens->entries[parser->current - 1];
}

static Token peek(Parser *parser) {
  return parser->tokens->entries[parser->current];
}

static bool is_end(Parser *parser) { return peek(parser).type == TOKEN_EOF; }

static Token advance(Parser *parser) {
  if (!is_end(parser))
    parser->current++;
  return previous(parser);
}

static bool check(Parser *parser, TokenType type) {
  if (is_end(parser))
    return false;
  return peek(parser).type == type;
}

static bool match(Parser *parser, TokenType type) {
  if (check(parser, type)) {
    advance(parser);
    return true;
  }
  return false;
}

static void consume(Parser *parser, TokenType type, const char *error_message) {
  if (!match(parser, type)) {
    // TODO: unwind error here, don't exit
    fprintf(stderr, "%s\n", error_message);
    exit(EXIT_FAILURE);
  }
}

static Expr *expression(Parser *parser);

static Expr *literal(Parser *parser) {
  Token cur = peek(parser);

  if (match(parser, TOKEN_TRUE)) {
    return init_boolean(parser->arena, cur.line, true);
  }
  if (match(parser, TOKEN_FALSE)) {
    return init_boolean(parser->arena, cur.line, false);
  }
  if (match(parser, TOKEN_NUMBER)) {
    double num = strtod(cur.start, NULL);
    return init_number(parser->arena, cur.line, num);
  }
  if (match(parser, TOKEN_STRING)) {
    return init_string(parser->arena, cur.line, (char *)cur.start, cur.length);
  }
  if (match(parser, TOKEN_LEFT_PAREN)) {
    Expr *expr = expression(parser);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')'.");
    return init_grouping(parser->arena, cur.line, expr);
  }
  // TODO: list
  // TODO: tuple
  // TODO: map
  if (match(parser, TOKEN_IDENTIFIER)) {
    return init_var(parser->arena, cur.line, (char *)cur.start, cur.length);
  }
  if (match(parser, TOKEN_NIL)) {
    return init_nil(parser->arena, cur.line);
  }

  // TODO: unwind error here, don't exit
  fprintf(stderr, "%s\n", "Failed to match identifier.");
  exit(EXIT_FAILURE);
}

static Expr *unary(Parser *parser) {
  if (match(parser, TOKEN_MINUS) || match(parser, TOKEN_BANG)) {
    Token prev = previous(parser);
    Expr *expr = unary(parser);
    return init_unary(parser->arena, prev.line, prev.type, expr);
  }
  return literal(parser);
}

static Expr *pattern_match(Parser *parser) {
  Expr *expr = unary(parser);
  if (match(parser, TOKEN_EQUAL)) {
    int line = previous(parser).line;
    Expr *right = unary(parser);
    expr = init_binary(parser->arena, line, TOKEN_EQUAL, expr, right);
  }
  return expr;
}

static Expr *factor(Parser *parser) {
  Expr *expr = pattern_match(parser);
  while (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
    Token prev = previous(parser);
    Expr *right = pattern_match(parser);
    expr = init_binary(parser->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *term(Parser *parser) {
  Expr *expr = factor(parser);
  while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
    Token prev = previous(parser);
    Expr *right = factor(parser);
    expr = init_binary(parser->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *comparison(Parser *parser) {
  Expr *expr = term(parser);
  while (match(parser, TOKEN_LESS) || match(parser, TOKEN_LESS_EQUAL) ||
         match(parser, TOKEN_GREATER) || match(parser, TOKEN_GREATER_EQUAL)) {
    Token prev = previous(parser);
    Expr *right = term(parser);
    expr = init_binary(parser->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *equality(Parser *parser) {
  Expr *expr = comparison(parser);
  while (match(parser, TOKEN_EQUAL_EQUAL) || match(parser, TOKEN_BANG_EQUAL)) {
    Token prev = previous(parser);
    Expr *right = comparison(parser);
    expr = init_binary(parser->arena, prev.line, prev.type, expr, right);
  }
  return expr;
}

static Expr *_and(Parser *parser) {
  Expr *expr = equality(parser);
  while (match(parser, TOKEN_AND)) {
    Expr *right = equality(parser);
    expr = init_binary(parser->arena, previous(parser).line, TOKEN_AND, expr,
                       right);
  }
  return expr;
}

static Expr *_or(Parser *parser) {
  Expr *expr = _and(parser);
  while (match(parser, TOKEN_OR)) {
    Expr *right = _and(parser);
    expr = init_binary(parser->arena, previous(parser).line, TOKEN_OR, expr,
                       right);
  }
  return expr;
}

static Expr *block(Parser *parser) {
  if (match(parser, TOKEN_LEFT_BRACE)) {
    int line = previous(parser).line;
    ExprArray *exprs = arena_allocate(parser->arena, sizeof(*exprs));
    init_expr_array(exprs);
    while (!match(parser, TOKEN_RIGHT_BRACE)) {
      if (is_end(parser)) {
        // TODO: unwind error here, don't exit
        fprintf(stderr, "%s\n", "Expect '}'.");
        exit(EXIT_FAILURE);
      }
      Expr *expr = expression(parser);
      DYN_ARR_PUSH(Expr *, exprs, expr);
    }
    return init_block(parser->arena, line, exprs);
  }
  return _or(parser);
}

static Expr *function(Parser *parser) {
  if (match(parser, TOKEN_FUNCTION)) {
    int line = previous(parser).line;
    consume(parser, TOKEN_LEFT_PAREN, "Expect '('.");
    ExprArray *params = arena_allocate(parser->arena, sizeof(*params));
    init_expr_array(params);
    int arity = 0;
    while (!match(parser, TOKEN_RIGHT_PAREN)) {
      if (is_end(parser)) {
        // TODO: unwind error here, don't exit
        fprintf(stderr, "%s\n", "Expect ')'.");
        exit(EXIT_FAILURE);
      }
      arity += 1;
      Expr *param = unary(parser);
      DYN_ARR_PUSH(Expr *, params, param);
      if (!check(parser, TOKEN_RIGHT_PAREN))
        consume(parser, TOKEN_COMMA, "Expect ','.");
    }
    consume(parser, TOKEN_ARROW, "Expect '->'.");
    Expr *body = block(parser);
    return init_function(parser->arena, line, params, body, arity);
  }
  return block(parser);
}

static Expr *expression(Parser *parser) { return function(parser); }

void parse(TokenArray *tokens, Arena *arena) {
  Parser parser;
  init_parser(&parser, tokens, arena);
  while (!is_end(&parser)) {
    Expr *expr = expression(&parser);
#ifdef _PEACH_DEBUG_PARSER
    print_expr(expr, 0);
#endif
  }
};
