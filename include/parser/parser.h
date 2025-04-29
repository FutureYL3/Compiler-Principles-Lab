#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include <lexer/token.h>
#include <grammar/grammar.h>

/* ---------- 抽象语法树 ---------- */
struct AST {
  int symbol; // 非终结符或终结符
  std::vector<std::unique_ptr<AST>> child;
  explicit AST(int sym) : symbol(sym) {}
};

struct ParseResult {
  bool ok;
  std::unique_ptr<AST> root;
  std::vector<int> prod_seq; // 规约产生式编号
};

ParseResult parse(const std::vector<Token> &tokens);

#endif
