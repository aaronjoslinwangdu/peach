#include "scanner.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void repl() {
  char input[1024];

  for (;;) {
    printf(">> ");

    if (!fgets(input, sizeof(input), stdin)) {
      printf("\n");
      break;
    }

    TokenArray tokens;
    init_token_array(&tokens);
    scan(input, &tokens);
  }
}

void run_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Couldn't open file '%s' \n", path);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char source[file_size];
  size_t bytes_read = fread(source, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
    fprintf(stderr, "Failed to read file '%s' \n", path);
    exit(EXIT_FAILURE);
  }
  fclose(file);
  source[bytes_read] = '\0';

  TokenArray tokens;
  init_token_array(&tokens);
  scan(source, &tokens);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: ./build/lang [file]");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
