#include <grammar/first_follow.h>

void build_first(FSet &FIRST) {
  FIRST.assign(NT_AFTER_LAST, {});
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &p : G) {
      bool nullable = true;
      for (int X : p.rhs) {
        if (X == EPS) {
          changed |= FIRST[p.lhs].insert(EPS).second;
          nullable = false;
          break;
        }
        if (is_term(X)) {
          changed |= FIRST[p.lhs].insert(X).second;
          nullable = false;
          break;
        }
        for (int a : FIRST[X]) {
          if (a != EPS) {
            changed |= FIRST[p.lhs].insert(a).second;
          }
        }
        if (!FIRST[X].count(EPS)) {
          nullable = false;
          break;
        }
      }
      if (nullable) {
        changed |= FIRST[p.lhs].insert(EPS).second;
      }
    }
  }
}

void build_follow(const FSet &FIRST, FSet &FOLLOW) {
  FOLLOW.assign(NT_AFTER_LAST, {});
  FOLLOW[START_SYM].insert(DOLLAR);
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &p : G) {
      for (size_t i = 0; i < p.rhs.size(); ++i) {
        int B = p.rhs[i];
        if (!is_nonterm(B))  continue;

        bool nullable = true;
        for (size_t k = i + 1; k < p.rhs.size() && nullable; ++k) {
          int X = p.rhs[k];
          if (is_nonterm(X)) {
            for (int a : FIRST[X]) {
              if (a != EPS) changed |= FOLLOW[B].insert(a).second;
            }
            nullable = FIRST[X].count(EPS);
          } 
          else {                          // X 是终结符
            changed |= FOLLOW[B].insert(X).second;
            nullable = false;
          }
        }
        if (nullable) {
          for (int a : FOLLOW[p.lhs]) {
            changed |= FOLLOW[B].insert(a).second;
          }
        }
      }
    }
  }
}
