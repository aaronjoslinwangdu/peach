#include "scanner.h"
#include "debug.h"
#include "memory.h"
#include "token.h"
#include <string.h>

typedef struct {
  const char *start;
  const char *current;
  int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
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
    if (advance() == '"')
      return make_token(TOKEN_STRING);
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
  return make_token(identifier_type());
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
      return;
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

  switch (c) {
  case '(':
    return make_token(TOKEN_LEFT_PAREN);
  case ')':
    return make_token(TOKEN_RIGHT_PAREN);
  case '{':
    return make_token(TOKEN_LEFT_BRACE);
  case '}':
    return make_token(TOKEN_RIGHT_BRACE);
  case '[':
    return make_token(TOKEN_LEFT_BRACKET);
  case ']':
    return make_token(TOKEN_RIGHT_BRACKET);
  case '+':
    return make_token(TOKEN_PLUS);
  case '-':
    return match('>') ? make_token(TOKEN_ARROW) : make_token(TOKEN_MINUS);
  case '*':
    return make_token(TOKEN_STAR);
  case '/':
    return make_token(TOKEN_SLASH);
  case '|':
    return match('>') ? make_token(TOKEN_PIPE) : make_token(TOKEN_ERROR);
  case '<':
    return match('=') ? make_token(TOKEN_LESS_EQUAL) : make_token(TOKEN_LESS);
  case '>':
    return match('=') ? make_token(TOKEN_GREATER_EQUAL)
                      : make_token(TOKEN_GREATER);
  case '!':
    return match('=') ? make_token(TOKEN_BANG_EQUAL) : make_token(TOKEN_BANG);
  case '=':
    return match('=') ? make_token(TOKEN_EQUAL_EQUAL) : make_token(TOKEN_EQUAL);
  case ',':
    return make_token(TOKEN_COMMA);
  case '.':
    return make_token(TOKEN_DOT);
  case ':':
    return make_token(TOKEN_COLON);
  case '\n':
    return make_token(TOKEN_NEWLINE);
  case '"':
    return string();
  }
  return make_token(TOKEN_ERROR);
}

void scan(const char *source, TokenArray *tokens) {
  init_scanner(source);
  for (;;) {
    Token token = scan_token();
    DYN_ARR_PUSH(Token, tokens, token);
    if (token.type == TOKEN_NEWLINE)
      scanner.line++;
#ifdef _LANG_DEBUG_SCANNER
    print_token(&token);
#endif
    if (token.type == TOKEN_EOF)
      return;
  }
}
