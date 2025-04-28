#include <lexer/lexer.h>
#include <cstdio>

int main(int argc, char *argv[]) {
  FILE *fp = fopen(argv[1], "r");
  if (!fp) { perror("fopen"); return 1; }

  Token tk;
  do {
    tk = next_token(fp);
    printf("%3ld:%-2ld  %-10s %-10s\n", tk.line, tk.column, TOKEN_NAMES[tk.type], tk.lexeme.c_str());
  } while (tk.type != TK_EOF && tk.type != TK_INVALID);

  fclose(fp);
  return 0;
}