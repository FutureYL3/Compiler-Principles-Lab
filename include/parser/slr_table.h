#ifndef SLR_TABLE_H
#define SLR_TABLE_H
#include <grammar/lr0.h>
#include <grammar/first_follow.h>
#include <unordered_map>

enum ActTag {
  SHIFT,
  REDUCE,
  ACCEPT,
  ERROR
};
struct Action {
  ActTag tag;
  int param;
};

extern std::vector<std::unordered_map<int, Action>> ACTION;
extern std::vector<std::unordered_map<int, int>>    GOTO_TBL;

bool build_slr(const FSet &FOLLOW); // 返回 false 表示存在冲突
#endif
