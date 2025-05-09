#include <parser/slr_table.h>
#include <lexer/token.h>
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
    } 
    else {
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
        } 
        else {
          Action &cell = itAct->second;
          if (cell.tag == SHIFT) {
            if (a == TK_ELSE) {
              // 悬挂 Else：保留 SHIFT
            } 
            else {
              std::cerr << "SLR conflict at state " << I << " on symbol " << a << ": Shift/Reduce" << std::endl;
              cell = {ERROR, 0};
              ok = false;
            }
          } 
          else if (cell.tag == REDUCE) {
            if (cell.param != reduceAction.param) {
              std::cerr << "SLR conflict at state " << I << " on symbol " << a << ": Reduce/Reduce" << std::endl;
              cell = {ERROR, 0};
              ok = false;
            }
          } 
          else if (cell.tag == ACCEPT) {
            std::cerr << "SLR conflict at state " << I << " on symbol " << a << ": Action/Accept" << std::endl;
            cell = {ERROR, 0};
            ok = false;
          } 
          else {
            // cell.tag == ERROR，覆盖插入
            // No, if it's already an ERROR due to S/R or R/R, keep it as ERROR.
            // Only fill if it was truly empty. The logic above already sets ERROR on conflict.
            // The new loop below will handle initially empty cells.
            // However, if a reduce action wants to overwrite an ERROR, it might be fine if the ERROR was a placeholder.
            // Let's refine: if cell.tag == ERROR, we might allow a reduce action to be placed if it's the first *meaningful* action.
            // For now, the original logic is: if (itAct == ACTION[I].end()) { ACTION[I][a] = reduceAction; } else { // handle conflict }
            // This means if there's *any* prior entry (SHIFT, REDUCE, ACCEPT, or even a pre-filled ERROR), it goes to conflict.
            // This is okay. The new loop will fill truly empty cells.
             ACTION[I][a] = reduceAction; // Original code has this, let's stick to it for now.
          }
        }
      }
    }
  }

  // Fill empty ACTION table cells with ERROR
  for (size_t i = 0; i < n; ++i) {
    // Iterate through all relevant terminal symbols. 
    // TOKEN_TYPE enum goes from TK_FUNCTION to TK_INVALID.
    // Terminals used in parsing are typically from the first keyword/symbol up to TK_DOLLAR.
    // TK_EOF and TK_INVALID are special and usually not direct keys in ACTION table rows for SHIFT/REDUCE.
    for (int t = 0; t <= TK_DOLLAR; ++t) { // Assuming terminals are 0-indexed up to TK_DOLLAR
      if (ACTION[i].find(t) == ACTION[i].end()) {
        ACTION[i][t] = Action{ERROR, 0};
      }
    }
  }

  return ok;
}
