#include <grammar/grammar.h>
#include <vector>

std::vector<Production> G;
int START_SYM = NT_Program;

/* helper to add productions */
static void add(int lhs, std::vector<int> rhs) {
  G.push_back({lhs, std::move(rhs)});
}

struct InitGrammar {
  InitGrammar() {
    // Augmented grammar start
    add(NT_AUG_START,     {NT_Program});

    // Program structure
    add(NT_Program,       {NT_FunctionList});
    add(NT_FunctionList,  {NT_Function, NT_FunctionList});
    add(NT_FunctionList,  {});
    add(NT_Function,      {TK_FUNCTION, TK_ID, TK_LPAREN, TK_RPAREN,
                           TK_BEGIN, NT_StmtList, TK_END});

    // Statements
    add(NT_StmtList,       {NT_Stmt, NT_OptSemiStmtList});
    add(NT_OptSemiStmtList,{TK_SEMICOLON, NT_Stmt, NT_OptSemiStmtList});
    add(NT_OptSemiStmtList,{TK_SEMICOLON});       // 允许尾分号
    add(NT_OptSemiStmtList,{});                   // 或 ε


    add(NT_Stmt,          {NT_DeclStmt});
    add(NT_Stmt,          {NT_AssignStmt});
    add(NT_Stmt,          {NT_WhileStmt});
    add(NT_Stmt,          {NT_IfStmt});
    add(NT_Stmt,          {NT_ReturnStmt});
    add(NT_Stmt,          {NT_Block});
    add(NT_Block,         {TK_BEGIN, NT_StmtList, TK_END});

    // Declarations
    add(NT_DeclStmt,      {TK_VAR, NT_IdList});
    add(NT_IdList,        {TK_ID, NT_IdRest});
    add(NT_IdRest,        {TK_COMMA, TK_ID, NT_IdRest});
    add(NT_IdRest,        {});

    // Assign, return, while, if use relational expressions
    add(NT_AssignStmt,    {TK_ID, TK_ASSIGN, NT_RelExpr});
    add(NT_ReturnStmt,    {TK_RETURN, NT_RelExpr});
    add(NT_WhileStmt,     {TK_WHILE, NT_RelExpr, TK_DO,
                           NT_StmtList, TK_END, TK_WHILE});
    add(NT_IfStmt,        {TK_IF, NT_RelExpr, TK_THEN,
                           NT_StmtList, TK_END, TK_IF});
    add(NT_IfStmt,        {TK_IF, NT_RelExpr, TK_THEN,
                           NT_StmtList, TK_ELSE, NT_StmtList,
                           TK_END, TK_IF});

    // Expression: term +-, then relational
    add(NT_Expr,          {NT_Term, NT_ExprRest});
    add(NT_ExprRest,      {TK_PLUS, NT_Term, NT_ExprRest});
    add(NT_ExprRest,      {TK_MINUS,NT_Term, NT_ExprRest});
    add(NT_ExprRest,      {});

    add(NT_Term,          {NT_Factor, NT_TermRest});
    add(NT_TermRest,      {TK_MULTIPLY, NT_Factor, NT_TermRest});
    add(NT_TermRest,      {TK_DIVIDE,   NT_Factor, NT_TermRest});
    add(NT_TermRest,      {});

    // Factor: identifier, number, or parenthesized relational expression
    add(NT_Factor,        {TK_ID});
    add(NT_Factor,        {TK_INT_NUM});
    add(NT_Factor,        {TK_LPAREN, NT_RelExpr, TK_RPAREN});

    // Relational expressions: Expr then optional relational operator
    add(NT_RelExpr,       {NT_Expr, NT_RelRest});
    add(NT_RelRest,       {TK_LT,  NT_Expr});
    add(NT_RelRest,       {TK_GT,  NT_Expr});
    add(NT_RelRest,       {TK_LE,  NT_Expr});
    add(NT_RelRest,       {TK_GE,  NT_Expr});
    add(NT_RelRest,       {TK_EQ,  NT_Expr});
    add(NT_RelRest,       {TK_NEQ, NT_Expr});
    add(NT_RelRest,       {});
  }
} _init;
