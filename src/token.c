#include "token.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

void init_token_array(TokenArray *tokens) {
  tokens->count = 0;
  tokens->capacity = 0;
  tokens->entries = NULL;
}

void free_token_array(TokenArray *tokens) {
  DYN_ARR_FREE(tokens);
  init_token_array(tokens);
}

static char *token_type_string(TokenType type) {
  switch (type) {
  case TOKEN_LEFT_PAREN:
    return "LEFT_PAREN";
  case TOKEN_RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TOKEN_LEFT_BRACE:
    return "LEFT_BRACE";
  case TOKEN_RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TOKEN_LEFT_BRACKET:
    return "LEFT_BRACKET";
  case TOKEN_RIGHT_BRACKET:
    return "RIGHT_BRACKET";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_STAR:
    return "STAR";
  case TOKEN_SLASH:
    return "SLASH";
  case TOKEN_LESS:
    return "LESS";
  case TOKEN_GREATER:
    return "GREATER";
  case TOKEN_BANG:
    return "BANG";
  case TOKEN_EQUAL:
    return "EQUAL";
  case TOKEN_COMMA:
    return "COMMA";
  case TOKEN_DOT:
    return "DOT";
  case TOKEN_COLON:
    return "COLON";
  case TOKEN_NEWLINE:
    return "NEWLINE";
  case TOKEN_BANG_EQUAL:
    return "BANG_EQUAL";
  case TOKEN_EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TOKEN_ARROW:
    return "ARROW";
  case TOKEN_LESS_EQUAL:
    return "LESS_EQUAL";
  case TOKEN_GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TOKEN_PIPE:
    return "PIPE";
  case TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_STRING:
    return "STRING";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_AND:
    return "AND";
  case TOKEN_OR:
    return "OR";
  case TOKEN_NIL:
    return "NIL";
  case TOKEN_FUNCTION:
    return "FUNCTION";
  case TOKEN_IF:
    return "IF";
  case TOKEN_ELSE:
    return "ELSE";
  case TOKEN_TRUE:
    return "TRUE";
  case TOKEN_FALSE:
    return "FALSE";
  case TOKEN_EOF:
    return "EOF";
  case TOKEN_ERROR:
    return "ERROR";
  }
}

void print_token(Token token) {
  char lexeme[token.length + 1];
  strncpy(lexeme, token.start, token.length);
  lexeme[token.length] = '\0';
  if (token.type == TOKEN_NEWLINE) {
    lexeme[0] = '$';
  }
  printf("Token = { type = %12s, lexeme = '%s', line = %d }\n",
         token_type_string(token.type), lexeme, token.line);
}
