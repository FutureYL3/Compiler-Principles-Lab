#include "lexer.h"
#include <cstdio>

int main(int argc, char *argv[]) {
  FILE *fp = fopen(argc > 1 ? argv[1] : "example_code", "r");
  if (!fp) { perror("fopen"); return 1; }

  /* ensure ch has an initial value */
  int ch = fgetc(fp);

  Token tk;
  do {
    tk = next_token(fp);
    printf("%3ld:%-2ld  %-10s %-10s\n", tk.line, tk.column, TOKEN_NAMES[tk.type], tk.lexeme.c_str());
  } while (tk.type != TK_EOF && tk.type != TK_INVALID);

  fclose(fp);
  return 0;
}