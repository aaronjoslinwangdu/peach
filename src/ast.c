#include "ast.h"

void init_expr_array(ExprArray *exprs) {
  exprs->count = 0;
  exprs->capacity = 0;
  exprs->entries = NULL;
}
