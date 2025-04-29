#ifndef LR0_H
#define LR0_H
#include <grammar/grammar.h>
#include <vector>
#include <unordered_set>
#include <map>

struct Item {
  int prod, dot;
  bool operator==(const Item &o) const { return prod == o.prod && dot == o.dot; }
};
struct ItemHash {
  size_t operator()(const Item &i) const { return i.prod * 131 + i.dot; }
};

using State = std::unordered_set<Item, ItemHash>;
using CCA = std::vector<State>;

extern CCA CC;
extern std::map<std::pair<int, int>, int> GO;

void build_lr0();
#endif
