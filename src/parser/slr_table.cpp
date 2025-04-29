#include <parser/slr_table.h>
#include <iostream>

std::vector<std::unordered_map<int,Action>> ACTION;
std::vector<std::unordered_map<int,int>>    GOTO_TBL;

bool build_slr(const FSet &FOLLOW) {
  size_t n = CC.size();
  ACTION.assign(n, {});
  GOTO_TBL.assign(n, {});
  bool ok = true;

  /* shift 行 */
  for (const auto &e : GO) {
    int I = e.first.first;
    int X = e.first.second;
    int J = e.second;
    if (is_term(X)) {
      ACTION[I][X] = {SHIFT, J};
    } else {
      GOTO_TBL[I][X] = J;
    }
  }

  /* reduce / accept */
  for (size_t I = 0; I < n; ++I) {
    for (const auto &item : CC[I]) {
      const auto &pr = G[item.prod];
      if (item.dot != pr.rhs.size()) continue;

      if (item.prod == 0) {
        // S' → Program·
        ACTION[I][DOLLAR] = {ACCEPT, 0};
        continue;
      }

      Action reduceAction{REDUCE, static_cast<int>(item.prod)};
      for (int a : FOLLOW[pr.lhs]) {
        auto itAct = ACTION[I].find(a);
        if (itAct == ACTION[I].end()) {
          // 没有已有动作，安全插入 Reduce
          ACTION[I][a] = reduceAction;
        } else {
          Action &cell = itAct->second;
          if (cell.tag == SHIFT) {
            if (a == TK_ELSE) {
              // 悬挂 Else：保留 SHIFT
            } else {
              std::cerr << "SLR conflict at state " << I
                        << " on symbol " << a << ": Shift/Reduce\n";
              cell = {ERROR, 0};
              ok = false;
            }
          } else if (cell.tag == REDUCE) {
            if (cell.param != reduceAction.param) {
              std::cerr << "SLR conflict at state " << I
                        << " on symbol " << a << ": Reduce/Reduce\n";
              cell = {ERROR, 0};
              ok = false;
            }
          } else if (cell.tag == ACCEPT) {
            std::cerr << "SLR conflict at state " << I
                      << " on symbol " << a << ": Action/Accept\n";
            cell = {ERROR, 0};
            ok = false;
          } else {
            // cell.tag == ERROR，覆盖插入
            cell = reduceAction;
          }
        }
      }
    }
  }

  return ok;
}
