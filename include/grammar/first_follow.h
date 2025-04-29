#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H
#include <grammar/grammar.h>
#include <vector>
#include <unordered_set>

using Set  = std::unordered_set<int>;
using FSet = std::vector<Set>;

void build_first(FSet& FIRST);
void build_follow(const FSet& FIRST, FSet& FOLLOW);
#endif
