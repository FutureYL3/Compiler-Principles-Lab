#include <lexer/lexer.h>
#include <parser/parser.h>
#include <grammar/first_follow.h>
#include <grammar/lr0.h>
#include <grammar/grammar.h>
#include <parser/slr_table.h>
#include <common.h>
#include <vector>
#include <cstdio>
#include <iostream>
#include <iomanip>

/* 深度打印 AST */
static void print_ast(const AST *n, int indent = 0) {
  for (int i = 0; i < indent; ++i) {
    std::cout << "  ";
  }
  if (is_nonterm(n->symbol)) {
    int idx = n->symbol - NT_BASE;
    const char *name = (idx >= 0 && idx < NONTERM_CNT) ? NONTERM_NAMES[idx] : "<UnknownNonterm>";
    std::cout << name << "\n";
  } 
  else {
    std::cout << TOKEN_NAMES[n->symbol] << "\n";
  }
  for (const auto &c : n->child) {
    print_ast(c.get(), indent + 1);
  }
}

// Function to print the ACTION and GOTO tables
static void print_parsing_tables() {
  std::cout << "\n=== ACTION Table ===\n";
  for (size_t i = 0; i < ACTION.size(); ++i) {
    if (ACTION[i].empty())  continue; // Skip states with no ACTION entries if desired, or print "State i: (empty)"
    std::cout << "State " << i << ":\n";
    for (const auto& entry : ACTION[i]) {
      int terminal_symbol = entry.first;
      const Action& action_obj = entry.second;
      std::cout << "  ACTION[" << symbol_name(terminal_symbol) << "] = ";
      switch (action_obj.tag) {
        case SHIFT:
          std::cout << "SHIFT " << action_obj.param;
          break;
        case REDUCE:
          std::cout << "REDUCE " << action_obj.param << " (";
          if (action_obj.param >= 0 && static_cast<size_t>(action_obj.param) < G.size()) {
            const auto& prod = G[action_obj.param];
            std::cout << symbol_name(prod.lhs) << " ->";
            if (prod.rhs.empty()) {
              std::cout << " ε";
            } 
            else {
              for (int sym : prod.rhs) {
                std::cout << " " << symbol_name(sym);
              }
            }
          } 
          else {
            std::cout << "Invalid Production Index";
          }
          std::cout << ")";
          break;
        case ACCEPT:
          std::cout << "ACCEPT";
          break;
        case ERROR:
          std::cout << "ERROR";
          break;
        default:
          std::cout << "UNKNOWN_ACTION_TAG";
          break;
      }
      std::cout << "\n";
    }
  }

  std::cout << "\n=== GOTO Table ===\n";
  for (size_t i = 0; i < GOTO_TBL.size(); ++i) {
    if (GOTO_TBL[i].empty())  continue; // Skip states with no GOTO entries
    std::cout << "State " << i << ":\n";
    for (const auto& entry : GOTO_TBL[i]) {
      int nonterminal_symbol = entry.first;
      int target_state = entry.second;
      std::cout << "  GOTO[" << symbol_name(nonterminal_symbol) << "] = " << target_state << "\n";
    }
  }
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts("usage: ./compiler <source.txt>");
    return 0;
  }
  FILE *fp = fopen(argv[1], "r");
  if (!fp) {
    perror("fopen");
    return 1;
  }

  std::vector<Token> tokens;
  while (true) {
    Token tk = next_token(fp);
    if (tk.type == TK_EOF || tk.type == TK_INVALID) {
      break;
    }
    tokens.push_back(tk);
  }
  fclose(fp);
  tokens.push_back(make_token(TK_DOLLAR, "$", 0, 0));

  /* FIRST / FOLLOW */
  FSet FIRST, FOLLOW;
  build_first(FIRST);
  build_follow(FIRST, FOLLOW);

  /* 项集 + SLR 表 */
  build_lr0();
  if (!build_slr(FOLLOW)) {
    std::cerr << "Conflicts detected\n";
    return 1;
  }

  /* 解析 */
  ParseResult R = parse(tokens);
  if (!R.ok) {
    return 1;
  }

  /* 打印 AST & 产生式序列 */
  std::cout << "=== AST ===\n";
  print_ast(R.root.get());
  std::cout << "=== Production sequence (bottom-up) ===\n";
  for (int p : R.prod_seq) {
    // 检查 p 是否在 G 的有效范围内 (可选但推荐)
    if (p >= 0 && static_cast<size_t>(p) < G.size()) {
      const auto& production = G[p]; // 获取产生式结构

      // 打印左侧非终结符名称
      std::cout << symbol_name(production.lhs) << " ->";

      // 打印右侧符号名称
      if (production.rhs.empty()) {
        // 处理空产生式 (epsilon)
        std::cout << " ε"; // 或者 " epsilon"
      } 
      else {
        for (int symbol : production.rhs) {
          std::cout << " " << symbol_name(symbol);
        }
      }
      std::cout << std::endl; // 每个产生式换一行
    } 
    else {
      std::cerr << "Warning: Invalid production index " << p << " in sequence." << std::endl;
    }
  }
  std::cout << std::endl;

  // Print parsing tables before exiting
  print_parsing_tables();

  return 0;
}
