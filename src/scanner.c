#include "scanner.h"
#include "debug.h"
#include "memory.h"
#include "token.h"
#include <string.h>

typedef struct {
  const char *start;
  const char *current;
  int line;
  bool insert_delim;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
  scanner.insert_delim = false;
}

static Token make_token(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static bool is_end() { return *scanner.current == '\0'; }

static char advance() {
  scanner.current++;
  return scanner.current[-1];
}

static char peek() { return *scanner.current; }
static char peek_next() {
  if (is_end())
    return '\0';
  return scanner.current[1];
}

static bool match(char c) {
  if (is_end() || *scanner.current != c) {
    return false;
  }
  scanner.current++;
  return true;
}

static TokenType check_identifier(int start, int length, const char *pattern,
                                  TokenType type) {
  if (scanner.current - scanner.start == start + length &&
      memcmp(scanner.start + start, pattern, length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}

static bool is_alpha(char c) {
  return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static Token string() {
  for (;;) {
    if (is_end())
      return make_token(TOKEN_ERROR);
    if (advance() == '"') {
      scanner.insert_delim = true;
      return make_token(TOKEN_STRING);
    }
  }
}

static Token digit() {
  while (is_digit(peek())) {
    advance();
  }

  if (peek() == '.' && is_digit(peek_next())) {
    advance();
    while (is_digit(peek())) {
      advance();
    }
  }

  scanner.insert_delim = true;
  return make_token(TOKEN_NUMBER);
}

static TokenType identifier_type() {
  switch (scanner.start[0]) {
  case 'e':
    return check_identifier(1, 3, "lse", TOKEN_ELSE);
  case 'f':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'n':
        return check_identifier(2, 0, "", TOKEN_FUNCTION);
      case 'a':
        return check_identifier(2, 3, "lse", TOKEN_FALSE);
      default:
        return TOKEN_IDENTIFIER;
      }
    }
  case 'i':
    return check_identifier(1, 1, "f", TOKEN_IF);
  case 'n':
    return check_identifier(1, 2, "il", TOKEN_NIL);
  case 't':
    return check_identifier(1, 3, "rue", TOKEN_TRUE);
  }
  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while (is_alpha(peek()) || is_digit(peek())) {
    advance();
  }
  TokenType type = identifier_type();
  switch (type) {
  case TOKEN_IDENTIFIER:
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  case TOKEN_NIL:
    scanner.insert_delim = true;
    break;
  default:
    scanner.insert_delim = false;
    break;
  }
  return make_token(type);
}

static void skip_whitespace() {
  for (;;) {
    switch (peek()) {
    case ' ':
    case '\t':
    case '\r':
      advance();
      break;
    case '/':
      if (peek_next() == '/') {
        while (peek() != '\n' && !is_end()) {
          advance();
        }
      } else {
        return;
      }
      break;
    case '\n':
      if (scanner.insert_delim)
        return;
      scanner.line++;
      advance();
      break;
    default:
      return;
    }
  }
}

static Token scan_token() {
  skip_whitespace();
  scanner.start = scanner.current;

  if (is_end())
    return make_token(TOKEN_EOF);

  char c = advance();

  if (is_digit(c)) {
    return digit();
  } else if (is_alpha(c)) {
    return identifier();
  }

  scanner.insert_delim = false;
  switch (c) {
  case '(':
    return make_token(TOKEN_LEFT_PAREN);
  case ')':
    scanner.insert_delim = true;
    return make_token(TOKEN_RIGHT_PAREN);
  case '{':
    return make_token(TOKEN_LEFT_BRACE);
  case '}':
    scanner.insert_delim = true;
    return make_token(TOKEN_RIGHT_BRACE);
  case '[':
    return make_token(TOKEN_LEFT_BRACKET);
  case ']':
    scanner.insert_delim = true;
    return make_token(TOKEN_RIGHT_BRACKET);
  case '+':
    return make_token(TOKEN_PLUS);
  case '-':
    return make_token(match('>') ? TOKEN_ARROW : TOKEN_MINUS);
  case '*':
    return make_token(TOKEN_STAR);
  case '/':
    return make_token(TOKEN_SLASH);
  case '&':
    return make_token(match('&') ? TOKEN_AND : TOKEN_ERROR);
  case '|':
    if (match('>')) {
      return make_token(TOKEN_PIPE);
    }
    if (match('|')) {
      return make_token(TOKEN_OR);
    }
    return make_token(TOKEN_ERROR);
  case '<':
    return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '!':
    return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case ',':
    return make_token(TOKEN_COMMA);
  case '.':
    return make_token(TOKEN_DOT);
  case ':':
    return make_token(TOKEN_COLON);
  case '"':
    return string();
  case '\n': {
    scanner.insert_delim = false;
    Token token = make_token(TOKEN_DELIM);
    scanner.line++;
    return token;
  }
  }
  return make_token(TOKEN_ERROR);
}

void scan(const char *source, TokenArray *tokens) {
  init_scanner(source);
  for (;;) {
    Token token = scan_token();
    DYN_ARR_PUSH(Token, tokens, token);
#ifdef _PEACH_DEBUG_SCANNER
    print_token(&token);
#endif
    if (token.type == TOKEN_EOF)
      return;
  }
}
