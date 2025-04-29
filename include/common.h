#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <grammar/grammar.h>
#include <lexer/token.h>

template <class SET>
void dump_set(const SET &s, const char *name) {
  std::cout << name << " = { ";
  for (auto x : s) {
    std::cout << x << " ";
  }
  std::cout << "}\n";
}

inline const char* symbol_name(int x) {
  if (is_term(x)) {
    return TOKEN_NAMES[x];
  }
  return NONTERM_NAMES[x - NT_BASE];
}

#endif
