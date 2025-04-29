#include <parser/parser.h>
#include <parser/slr_table.h>
#include <stack>
#include <iostream>

ParseResult parse(const std::vector<Token> &tk) {
  ParseResult R;
  R.ok = false;

  std::vector<int> stateSt{0};
  std::vector<std::unique_ptr<AST>> symSt; // AST* 与状态同步
  size_t ip = 0;

  while (true) {
    int s = stateSt.back();
    int a = tk[ip].type;
    auto it = ACTION[s].find(a);
    if (it == ACTION[s].end()) {
      // error
      std::cerr << "Syntax error at " << tk[ip].line << ":" << tk[ip].col << "\n";
      return R;
    }
    Action act = it->second;
    if (act.tag == SHIFT) {
      stateSt.push_back(act.param);
      /* 终结符节点 */
      symSt.push_back(std::make_unique<AST>(a));
      ++ip;
    }
    else if (act.tag == REDUCE) {
      const auto &pr = G[act.param];
      std::unique_ptr<AST> node = std::make_unique<AST>(pr.lhs);
      for (size_t k = 0; k < pr.rhs.size(); ++k) {
        stateSt.pop_back();
      }
      /* 弹 rhs 个数的 AST 作为孩子（逆序） */
      std::vector<std::unique_ptr<AST>> children;
      for (size_t k = 0; k < pr.rhs.size(); ++k) {
        children.push_back(std::move(symSt.back()));
        symSt.pop_back();
      }
      std::reverse(children.begin(), children.end());
      node->child = std::move(children);

      int t = stateSt.back();
      stateSt.push_back(GOTO_TBL[t][pr.lhs]);
      symSt.push_back(std::move(node));
      R.prod_seq.push_back(act.param); // 记下规约产生式号
    }
    else if (act.tag == ACCEPT) {
      R.ok = true;
      R.root = std::move(symSt.back());
      return R;
    }
  }
}
