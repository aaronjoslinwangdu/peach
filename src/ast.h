#ifndef _PEACH_AST_H
#define _PEACH_AST_H

#include "common.h"
#include "memory.h"
#include "token.h"

typedef enum {
  EXPR_BLOCK,
  EXPR_GROUPING,
  EXPR_UNARY,
  EXPR_FUNCTION,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_VAR,
  EXPR_STRING,
  EXPR_NUMBER,
  EXPR_BOOLEAN,
  EXPR_NIL,
} ExprType;

typedef struct Expr Expr;

typedef struct {
  int count;
  int capacity;
  Expr **entries;
} ExprArray;

typedef struct {
  Expr *expr;
} Grouping;

typedef struct {
  ExprArray *exprs;
} Block;

typedef struct {
  const char *name;
} Var;

typedef struct {
  Var *name;
  Expr **args;
} Call;

typedef struct {
  TokenType op;
  Expr *left;
} Unary;

typedef struct {
  TokenType op;
  Expr *left, *right;
} Binary;

typedef struct {
  ExprArray *params;
  Expr *body;
  int arity;
} Function;

typedef struct Expr {
  ExprType type;
  int line;
  union {
    Grouping grouping;
    Block block;
    Function function;
    Call call;
    Unary unary;
    Binary binary;
    Var var;
    bool boolean;
    double number;
    char *string;
  } as;
} Expr;

Expr *init_nil(Arena *a, int line);
Expr *init_boolean(Arena *a, int line, bool value);
Expr *init_number(Arena *a, int line, double value);
Expr *init_var(Arena *a, int line, char *start, int length);
Expr *init_string(Arena *a, int line, char *start, int length);
Expr *init_unary(Arena *a, int line, TokenType op, Expr *left);
Expr *init_binary(Arena *a, int line, TokenType op, Expr *left, Expr *right);
Expr *init_grouping(Arena *a, int line, Expr *expr);
Expr *init_block(Arena *a, int line, ExprArray *exprs);
Expr *init_function(Arena *a, int line, ExprArray *params, Expr *body,
                    int arity);

void init_expr_array(ExprArray *exprs);

#endif
