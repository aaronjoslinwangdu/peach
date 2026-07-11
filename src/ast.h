#ifndef _LANG_AST_H
#define _LANG_AST_H

#include "common.h"
#include "token.h"

typedef enum {
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_GROUP,
  EXPR_BLOCK,
  EXPR_CALL,
  EXPR_VAR,
  EXPR_STRING,
  EXPR_NUMBER,
  EXPR_BOOLEAN,
  EXPR_NIL,
} ExprType;

typedef struct Expr Expr;

typedef struct {
  Expr *expr;
} Group;

typedef struct {
  Expr **expr;
} Block;

typedef struct {
  const char *start;
  int length;
} Var;

typedef struct {
  Var *name;
  Expr **args;
} Call;

typedef struct {
  TokenType op;
  Expr *expr;
} Unary;

typedef struct {
  TokenType op;
  Expr *left, *right;
} Binary;

typedef struct Expr {
  ExprType type;
  int line;
  union {
    Group group;
    Block block;
    Call call;
    Unary unary;
    Binary binary;
    Var var;
    bool boolean;
    double number;
    char *string;
  } as;
} Expr;

typedef struct {
  int count;
  int capacity;
  Expr *entries;
} ExprArray;

void init_expr_array(ExprArray *exprs);

#endif
