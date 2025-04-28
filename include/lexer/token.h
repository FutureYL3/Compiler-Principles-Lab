#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TOKEN_TYPE {
  /* keyword */
  TK_FUNCTION, TK_BEGIN, TK_END, TK_VAR, TK_WHILE, TK_DO, TK_IF, TK_THEN, TK_ELSE, TK_RETURN,
  /* identifier and constant */
  TK_ID, TK_INT_NUM,
  /* operator */
  TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_ASSIGN, TK_EQ, TK_NEQ, TK_LT, TK_GT, TK_LE, TK_GE,
  /* delimiter */
  TK_SEMICOLON, TK_COMMA, TK_LPAREN, TK_RPAREN,
  /* end of file and invalid token */
  TK_EOF, TK_INVALID
};

constexpr const char* TOKEN_NAMES[] = {
  "FUNCTION","BEGIN","END","VAR","WHILE","DO","IF","THEN","ELSE","RETURN",
  "ID","INT_NUM",
  "PLUS","MINUS","MULTIPLY","DIVIDE","ASSIGN","EQ","NEQ","LT","GT","LE","GE",
  "SEMICOLON","COMMA","LPAREN","RPAREN",
  "EOF","INVALID"
};

typedef struct Token {
  TOKEN_TYPE    type;
  std::string   lexeme;    // only used for TK_ID and TK_INT_NUM
  int           line;
  int           column;
  long long     int_val;  // only used for TK_INT_NUM
} Token;

/* factory function, convenient to create and initialize */
inline Token make_token(TOKEN_TYPE t, const std::string& lex, int ln, int col, long long v = 0) {
  return Token{t, lex, ln, col, v};
}

#endif


