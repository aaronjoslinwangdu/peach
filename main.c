#include "scanner.h"

int main() {
  const char *source = "fn (a, b) -> print(a + b)\n"
                       "c = (x) -> { x * x }\n"
                       "d = 10\n"
                       "  |> c()\n"
                       "  |> fn (y) -> { y - 10 }()\n"
                       "// a comment\n"
                       "\0";
  init_scanner(source);
  scan();
  return 0;
}
