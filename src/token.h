#ifndef _PEACH_TOKEN_H
#define _PEACH_TOKEN_H

typedef enum {
  // one char
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_BANG,
  TOKEN_EQUAL,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_COLON,

  // two char
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_ARROW,
  TOKEN_LESS_EQUAL,
  TOKEN_GREATER_EQUAL,
  TOKEN_OR,
  TOKEN_AND,
  TOKEN_PIPE,

  // literals
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  // keywords
  TOKEN_NIL,
  TOKEN_FUNCTION,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_TRUE,
  TOKEN_FALSE,

  // other
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_DELIM,
} TokenType;

typedef struct {
  TokenType type;
  const char *start;
  int length;
  int line;
} Token;

typedef struct {
  int count;
  int capacity;
  Token *entries;
} TokenArray;

void init_token_array(TokenArray *tokens);
void free_token_array(TokenArray *tokens);

#endif
