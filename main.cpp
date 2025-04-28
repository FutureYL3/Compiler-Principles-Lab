#include <lexer/lexer.h>
#include <cstdio>
#include <vector>

int main(int argc, char *argv[]) {
  FILE *fp = fopen(argv[1], "r");
  if (!fp) { perror("fopen"); return 1; }

  Token tk;
  std::vector<Token> tokens;
  do {
    tk = next_token(fp);
    tokens.push_back(tk);
  } while (tk.type != TK_EOF && tk.type != TK_INVALID);

  fclose(fp);
  return 0;
}