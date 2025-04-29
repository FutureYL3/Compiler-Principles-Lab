#include <grammar/lr0.h>
#include <queue>
#include <algorithm>

CCA CC;
std::map<std::pair<int, int>, int> GO;

static State closure(State I) {
  std::queue<Item> q;
  for (auto it : I) {
    q.push(it);
  }
  while (!q.empty()) {
    Item it = q.front();
    q.pop();
    const auto &pr = G[it.prod];
    if (it.dot < pr.rhs.size()) {
      int B = pr.rhs[it.dot];
      if (is_nonterm(B)) {
        for (size_t k = 0; k < G.size(); ++k) {
          if (G[k].lhs == B && I.insert({static_cast<int>(k), 0}).second) {
            q.push({static_cast<int>(k), 0});
          }
        }
      }
    }
  }
  return I;
}

static State GOTO(const State &I, int X) {
  State J;
  for (auto it : I) {
    const auto &pr = G[it.prod];
    if (it.dot < pr.rhs.size() && pr.rhs[it.dot] == X) {
      J.insert({it.prod, it.dot + 1});
    }
  }
  return closure(std::move(J));
}

void build_lr0() {
  CC.clear();
  GO.clear();
  CC.push_back(closure({Item{0, 0}}));
  std::queue<int> q;
  q.push(0);
  while (!q.empty()) {
    int I = q.front();
    q.pop();
    std::unordered_set<int> symbols;
    for (auto it : CC[I]) {
      const auto &pr = G[it.prod];
      if (it.dot < pr.rhs.size()) {
        symbols.insert(pr.rhs[it.dot]);
      }
    }
    for (int X : symbols) {
      State J = GOTO(CC[I], X);
      auto it = find(CC.begin(), CC.end(), J);
      int Jid;
      if (it == CC.end()) {
        CC.push_back(J);
        Jid = CC.size() - 1;
        q.push(Jid);
      }
      else {
        Jid = it - CC.begin();
      }
      GO[{I, X}] = Jid;
    }
  }
}
