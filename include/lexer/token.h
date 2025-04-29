#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TOKEN_TYPE {
  /* keyword */
  TK_FUNCTION, TK_BEGIN, TK_END, TK_VAR, TK_WHILE, TK_DO,
  TK_IF, TK_THEN, TK_ELSE, TK_RETURN,
  /* id & const */
  TK_ID, TK_INT_NUM,
  /* operator */
  TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE,
  TK_ASSIGN, TK_EQ, TK_NEQ, TK_LT, TK_GT, TK_LE, TK_GE,
  /* delimiter */
  TK_SEMICOLON, TK_COMMA, TK_LPAREN, TK_RPAREN,
  /* special */
  TK_DOLLAR, TK_EOF, TK_INVALID
};

inline constexpr const char* TOKEN_NAMES[] = {
 "FUNCTION","BEGIN","END","VAR","WHILE","DO","IF","THEN","ELSE","RETURN",
 "ID","INT","PLUS","MINUS","MUL","DIV","ASSIGN","EQ","NEQ","LT","GT","LE","GE",
 "SEMI","COMMA","LPAREN","RPAREN","$","EOF","INVALID"
};

struct Token {
  TOKEN_TYPE   type;
  std::string  lexeme;
  int          line;
  int          col;
  long long    int_val;
};

inline Token make_token(TOKEN_TYPE t,const std::string& lx,int ln,int cl,long long v=0){
  return Token{t,lx,ln,cl,v};
}

#endif


