#include <lexer/lexer.h>
#include <cctype>       // isalpha isdigit isspace
#include <unordered_map>
#include <string>

/* ---------- global status variable ---------- */
static int  ch      = ' ';   // current character
static long line_no = 1;     // line number：start from 1
static long col_no  = 0;     // column number：start from 0

/* ---------- basic operation ---------- */
static inline int  peek(FILE *fp)          { int c = fgetc(fp); ungetc(c, fp); return c; }
static inline void advance(FILE *fp)       { ch = fgetc(fp); ++col_no; }
static inline void newline()               { ++line_no; col_no = 0; }

/* ---------- keyword table ---------- */
static const std::unordered_map<std::string, TOKEN_TYPE> KW = {
  {"function", TK_FUNCTION}, {"begin",   TK_BEGIN }, {"end",    TK_END},
  {"var",      TK_VAR     }, {"while",   TK_WHILE }, {"do",     TK_DO},
  {"if",       TK_IF      }, {"then",    TK_THEN  }, {"else",   TK_ELSE},
  {"return",   TK_RETURN  }
};

/* ---------- skip space and comments ---------- */
static void skip_space_and_comments(FILE *fp) {
  bool again = true;
  while (again) {
    again = false;

    /* 1. space */
    while (isspace(ch)) {
      if (ch == '\n') newline();
      advance(fp);
    }

    /* 2. // comment */
    if (ch == '/' && peek(fp) == '/') {
      while (ch != '\n' && ch != EOF) advance(fp);
      again = true;
    }
    /* 3. /* comment *\/ */
    else if (ch == '/' && peek(fp) == '*') {
      advance(fp);           // read '*'
      advance(fp);           // enter comment body
      while (true) {
        if (ch == EOF) break;                   /* unclosed comment */
        if (ch == '\n') newline();
        if (ch == '*' && peek(fp) == '/') { advance(fp); advance(fp); break; }  /* finished '*\/' */
        else advance(fp);
      }
      again = true;
    }
  }
}

/* ---------- get next token ---------- */
Token next_token(FILE *fp) {
  skip_space_and_comments(fp);

  long tok_line = line_no;
  long tok_col  = col_no;
  std::string lexeme;
  long long    int_val = 0;

  switch (ch) {
  /* ---------- EOF ---------- */
  case EOF:
    return make_token(TK_EOF, "", tok_line, tok_col);

  /* ---------- single character delim & operator ---------- */
  case '+': advance(fp); return make_token(TK_PLUS,      "+", tok_line, tok_col);
  case '-': advance(fp); return make_token(TK_MINUS,     "-", tok_line, tok_col);
  case '*': advance(fp); return make_token(TK_MULTIPLY,  "*", tok_line, tok_col);
  case '/': advance(fp); return make_token(TK_DIVIDE,    "/", tok_line, tok_col);
  case ';': advance(fp); return make_token(TK_SEMICOLON, ";", tok_line, tok_col);
  case ',': advance(fp); return make_token(TK_COMMA,     ",", tok_line, tok_col);
  case '(': advance(fp); return make_token(TK_LPAREN,    "(", tok_line, tok_col);
  case ')': advance(fp); return make_token(TK_RPAREN,    ")", tok_line, tok_col);

  /* ---------- double/single character relation & assignment operator ---------- */
  case ':':
    advance(fp);
    if (ch == '=') { advance(fp); return make_token(TK_ASSIGN, ":=", tok_line, tok_col); }
    else           { return make_token(TK_INVALID, ":", tok_line, tok_col); }

  case '=':
    advance(fp);
    if (ch == '=') { advance(fp); return make_token(TK_EQ,  "==", tok_line, tok_col); }
    else           { return make_token(TK_INVALID, "=", tok_line, tok_col); }

  case '!':
    if (peek(fp) == '=') { advance(fp); advance(fp); return make_token(TK_NEQ, "!=", tok_line, tok_col); }
    else                 { advance(fp); return make_token(TK_INVALID, "!", tok_line, tok_col); }

  case '<':
    advance(fp);
    if (ch == '=') { advance(fp); return make_token(TK_LE, "<=", tok_line, tok_col); }
    else           { return make_token(TK_LT, "<", tok_line, tok_col); }

  case '>':
    advance(fp);
    if (ch == '=') { advance(fp); return make_token(TK_GE, ">=", tok_line, tok_col); }
    else           { return make_token(TK_GT, ">", tok_line, tok_col); }

  /* ---------- identifier / keyword ---------- */
  default:
    /* --- Identifier or keyword --- */
    if (isalpha(ch) || ch == '_') {       // identifier can start with letter and _
      while (isalnum(ch) || ch == '_') {  // identifier can only contain letter, digit and _
        lexeme.push_back(ch);
        advance(fp);
      }
      auto it = KW.find(lexeme);
      if (it != KW.end())
        return make_token(it->second, lexeme, tok_line, tok_col);
      else
        return make_token(TK_ID, lexeme, tok_line, tok_col);  // if not found in keyword table, it's an identifier
    }
    /* --- integer constant --- */
    else if (isdigit(ch)) {
      while (isdigit(ch)) {
        int_val = int_val * 10 + (ch - '0');
        lexeme.push_back(ch);
        advance(fp);
      }
      return make_token(TK_INT_NUM, lexeme, tok_line, tok_col, int_val);
    }
    /* other invalid characters */
    else {
      char bad = ch;
      advance(fp);
      return make_token(TK_INVALID, std::string(1, bad), tok_line, tok_col);
    }
  }
}
