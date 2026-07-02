#include <string>

enum TokenType {
  // one char
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  PLUS,
  MINUS,
  STAR,
  SLASH,
  LESS,
  GREATER,
  BANG,
  EQUAL,
  COMMA,
  DOT,
  COLON,
  NEWLINE,

  // two char
  BANG_EQUAL,
  EQUAL_EQUAL,
  ARROW,
  LESS_EQUAL,
  GREATER_EQUAL,
  PIPE,

  // literals
  IDENTIFIER,
  STRING,
  NUMBER,

  // keywords
  AND,
  OR,
  NIL,
  NOT,
  FUNCTION,
  IF,
  ELSE,
  TRUE,
  FALSE,
};

class Token {
public:
  TokenType type;
  std::string lexeme;
  int line;
};
