#include "debug.h"
#include <stdio.h>

void print_token(Token *token) {
  printf("Token = { ");
  const char *type;
  switch (token->type) {
  case TOKEN_LEFT_PAREN:
    type = "LEFT_PAREN";
    break;
  case TOKEN_RIGHT_PAREN:
    type = "RIGHT_PAREN";
    break;
  case TOKEN_LEFT_BRACE:
    type = "LEFT_BRACE";
    break;
  case TOKEN_RIGHT_BRACE:
    type = "RIGHT_BRACE";
    break;
  case TOKEN_LEFT_BRACKET:
    type = "LEFT_BRACKET";
    break;
  case TOKEN_RIGHT_BRACKET:
    type = "RIGHT_BRACKET";
    break;
  case TOKEN_PLUS:
    type = "PLUS";
    break;
  case TOKEN_MINUS:
    type = "MINUS";
    break;
  case TOKEN_STAR:
    type = "STAR";
    break;
  case TOKEN_SLASH:
    type = "SLASH";
    break;
  case TOKEN_LESS:
    type = "LESS";
    break;
  case TOKEN_GREATER:
    type = "GREATER";
    break;
  case TOKEN_BANG:
    type = "BANG";
    break;
  case TOKEN_EQUAL:
    type = "EQUAL";
    break;
  case TOKEN_COMMA:
    type = "COMMA";
    break;
  case TOKEN_DOT:
    type = "DOT";
    break;
  case TOKEN_COLON:
    type = "COLON";
    break;
  case TOKEN_NEWLINE:
    type = "NEWLINE";
    break;
  case TOKEN_BANG_EQUAL:
    type = "BANG_EQUAL";
    break;
  case TOKEN_EQUAL_EQUAL:
    type = "EQUAL_EQUAL";
    break;
  case TOKEN_ARROW:
    type = "ARROW";
    break;
  case TOKEN_LESS_EQUAL:
    type = "LESS_EQUAL";
    break;
  case TOKEN_GREATER_EQUAL:
    type = "GREATER_EQUAL";
    break;
  case TOKEN_PIPE:
    type = "PIPE";
    break;
  case TOKEN_IDENTIFIER:
    type = "IDENTIFIER";
    break;
  case TOKEN_STRING:
    type = "STRING";
    break;
  case TOKEN_NUMBER:
    type = "NUMBER";
    break;
  case TOKEN_NIL:
    type = "NIL";
    break;
  case TOKEN_FUNCTION:
    type = "FUNCTION";
    break;
  case TOKEN_IF:
    type = "IF";
    break;
  case TOKEN_ELSE:
    type = "ELSE";
    break;
  case TOKEN_TRUE:
    type = "TRUE";
    break;
  case TOKEN_FALSE:
    type = "FALSE";
    break;
  case TOKEN_EOF:
    type = "EOF";
    break;
  case TOKEN_ERROR:
    type = "ERROR";
    break;
  }
  printf("type = %12s, ", type);
  if (token->type == TOKEN_NEWLINE) {
    printf("lexeme = 'newline', ");
  } else {
    printf("lexeme = '%.*s', ", token->length, token->start);
  }
  printf("line = %04d }\n", token->line);
}

void print_expr(Expr *expr) {
  printf("Expr = { ");
  const char *type;
  switch (expr->type) {
  case EXPR_UNARY:
    type = "UNARY";
    break;
  case EXPR_BINARY:
    type = "BINARY";
    break;
  case EXPR_GROUP:
    type = "GROUP";
    break;
  case EXPR_BLOCK:
    type = "BLOCK";
    break;
  case EXPR_CALL:
    type = "CALL";
    break;
  case EXPR_VAR:
    type = "VAR";
    break;
  case EXPR_STRING:
    type = "STRING";
    break;
  case EXPR_NUMBER:
    type = "NUMBER";
    break;
  case EXPR_BOOLEAN:
    type = "BOOLEAN";
    break;
  case EXPR_NIL:
    type = "NIL";
    break;
  }
  printf("type = %s, ", type);
  printf("line = %04d }\n", expr->line);
}
