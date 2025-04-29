#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <vector>
#include <unordered_set>
#include <lexer/token.h>      // 终结符枚举

/* ---------- 统一编号 ---------- */
inline constexpr int NT_BASE = TK_INVALID + 1;   // 30
enum {
  NT_Program = NT_BASE,
  NT_FunctionList,
  NT_Function,
  NT_StmtList,
  NT_Stmt,
  NT_Block,
  NT_DeclStmt,
  NT_IdList,
  NT_IdRest,
  NT_AssignStmt,
  NT_ReturnStmt,
  NT_WhileStmt,
  NT_IfStmt,
  NT_Expr,           
  NT_ExprRest,       
  NT_RelExpr,        // 新增：关系表达式
  NT_RelRest,        // 新增：关系表达式尾部
  NT_Term,
  NT_TermRest,
  NT_Factor,
  NT_OptSemiStmtList,
  NT_AUG_START,        // 增广符号
  NT_AFTER_LAST        // 枚举末端（= NT_BASE + 非终结符个数）
};

/* ---------- 常量 ---------- */
inline constexpr int NONTERM_CNT = NT_AFTER_LAST - NT_BASE;
inline constexpr int EPS    = -1;
inline constexpr int DOLLAR = TK_DOLLAR;

// 非终结符名称映射（对应 NT_Program ... NT_AUG_START）
static const char* NONTERM_NAMES[NONTERM_CNT] = {
  "Program",
  "FunctionList",
  "Function",
  "StmtList",
  "Stmt",
  "Block",
  "DeclStmt",
  "IdList",
  "IdRest",
  "AssignStmt",
  "ReturnStmt",
  "WhileStmt",
  "IfStmt",
  "Expr",
  "ExprRest",
  "RelExpr",
  "RelRest",
  "Term",
  "TermRest",
  "Factor",
  "OptSemiStmtList",
  "AUG_START"
};

/* ---------- 判定函数：区间互斥 ---------- */
inline bool is_term(int x)
{ return (x >= TK_FUNCTION && x <= TK_INVALID) || x == DOLLAR; }

inline bool is_nonterm(int x)
{ return x >= NT_BASE && x < NT_AFTER_LAST; }

/* ---------- 产生式 ---------- */
struct Production { int lhs; std::vector<int> rhs; };
extern std::vector<Production> G;
extern int START_SYM;
#endif
