#ifndef COMMON_H
#define COMMON_H
#include <iostream>
template <class SET>
void dump_set(const SET &s, const char *name) {
  std::cout << name << " = { ";
  for (auto x : s) {
    std::cout << x << " ";
  }
  std::cout << "}\n";
}
#endif
