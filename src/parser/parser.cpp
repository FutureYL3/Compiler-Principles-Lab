#include <parser/parser.h>
#include <parser/slr_table.h>
#include <lexer/token.h> // Added for TOKEN_NAMES and token enums
#include <stack>
#include <iostream>
#include <string> // Required for std::string if accumulating errors

// Forward declaration or include for symbol_name if it's not in a common header
// const char* symbol_name(int symbol); // Example: if you have this function

ParseResult parse(const std::vector<Token> &tk) {
  ParseResult R;
  R.ok = true; // Assume OK until an error occurs and isn't recovered, or for multiple errors.

  std::vector<int> stateSt{0};
  std::vector<std::unique_ptr<AST>> symSt; // AST* 与状态同步
  size_t ip = 0;

  while (true) {
    if (ip >= tk.size()) { // Should not happen if TK_DOLLAR is always present and handled
      R.ok = false;
      std::string errMsg = "Parser reached end of tokens unexpectedly.";
      std::cerr << errMsg << std::endl;
      // R.errors.push_back(errMsg); // If you add R.errors
      return R;
    }

    int s = stateSt.back();
    int a = tk[ip].type;
    auto it = ACTION[s].find(a);

    if (it == ACTION[s].end() || it->second.tag == ERROR) { // Explicitly check for ERROR tag if you add it
      if (R.ok) R.ok = false; // First error occurred
      std::string errMsg = "Syntax error at line " + std::to_string(tk[ip].line) +
                           ", col " + std::to_string(tk[ip].col) +
                           ". Unexpected token: " + tk[ip].lexeme + " (type: " + TOKEN_NAMES[a] + ")"; // Using TOKEN_NAMES
      std::cerr << errMsg << std::endl;
      // R.errors.push_back(errMsg);


      // --- Panic Mode Error Recovery ---
      bool recovered = false;
      while(ip < tk.size() - 1) { // -1 to keep TK_DOLLAR for final check (tk.size()-1 is the $ token)
        // Current token type 'a' is already tk[ip].type
        
        bool is_sync_token = false;
        switch (a) {
          case TK_DOLLAR:
          case TK_SEMICOLON:
          case TK_END:
          // Keywords that start statements/blocks could also be sync tokens
          case TK_VAR:
          case TK_WHILE:
          case TK_DO: // TK_DO might be tricky if it's not always a strong sync point
          case TK_IF:
          case TK_RETURN:
          case TK_FUNCTION: // Usually a very strong synchronization point
          case TK_BEGIN:    // Also a strong synchronization point
            is_sync_token = true;
            break;
          default:
            is_sync_token = false;
            break;
        }

        if (is_sync_token) {
          std::cerr << "  Found synchronization token " << tk[ip].lexeme << " (" << TOKEN_NAMES[a] << "). Attempting to resume." << std::endl;
          // Now, try to find a state on stack that can handle this token 'a'
          while (!stateSt.empty()) {
            int top_state = stateSt.back();
            // Check if current state can shift sync token OR if there's a GOTO for a non-terminal that 'a' can start
            // For simplicity, we primarily check ACTION table for SHIFT.
            // A more complex check would involve seeing if 'a' can start any non-terminal X,
            // and if GOTO[top_state][X] exists.
            auto& action_map_for_top_state = ACTION[top_state];
            if (action_map_for_top_state.count(a)) {
              Action& potential_action = action_map_for_top_state.at(a);
              // We want to shift a sync token, or accept if it's EOF ($)
              if (potential_action.tag == SHIFT || potential_action.tag == ACCEPT) {
                recovered = true;
                break; // Found a state that can process the sync token
              }
            }
            
            // If current state cannot handle 'a', pop the state.
            // std::cerr << "  Popping state " << top_state << " from stack (sync check)." << std::endl;
            stateSt.pop_back();
            // Handle symSt: If symSt elements correspond 1:1 to stateSt elements after a shift,
            // and new AST nodes are pushed on symSt BEFORE new states on stateSt during SHIFT,
            // then popping stateSt should often be accompanied by popping symSt.
            // The original code pushes to symSt on SHIFT, and stateSt.
            // On REDUCE, it pops stateSt pr.rhs.size() times, and symSt pr.rhs.size() times.
            // This suggests symSt might track symbols that led to states.
            // It's safer to be conservative or ensure logic is perfectly mirrored.
            // If a state is popped, the corresponding symbol that was shifted to reach that state
            // (or the LHS of a reduction that led to that GOTO state) is effectively being discarded
            // from the current parse path attempt.
            if (!symSt.empty() && symSt.size() >= stateSt.size() ) { // A cautious check
              // This part is tricky and depends on exact symSt <-> stateSt synchronization.
              // If a state represents "having seen a sequence leading to this state",
              // and symSt holds AST nodes for symbols in that sequence, popping state
              // means that path is invalid, so corresponding symSt node might be too.
              // However, for panic mode, we are trying to RESUME, not perfectly fix the AST.
              // Often, it's okay if symSt gets a bit desynchronized if it means we can parse further.
              // The AST node created from a faulty reduction might be wrong anyway.
              // A simple strategy: if stateSt is popped, also pop symSt.
              // symSt.pop_back(); // This is a common strategy but needs verification for your specific AST construction.
            }
          }
          if (recovered) break; // Break from token discarding (while ip < tk.size()-1) loop
          else {
            // If stack became empty and we couldn't use the sync token, we can't recover with this token.
            // Continue discarding to find next sync token.
            // std::cerr << "  Stack empty or sync token " << TOKEN_NAMES[a] << " not usable. Discarding it." << std::endl;
          }
        }
        
        // If not a usable sync token or recovery with it failed, discard current token and advance.
        if (!recovered || !is_sync_token) { // if it wasn't a sync token, or it was but we couldn't use it.
          // Only print discard message if we are not about to exit due to ip reaching end
          if (ip + 1 < tk.size() -1) {
            std::cerr << "  Discarding token: " << tk[ip].lexeme << " (" << TOKEN_NAMES[a] << ")" << std::endl;
          }
        }
        ip++;
        if (ip < tk.size()) {
          a = tk[ip].type; // Update lookahead token for next iteration of discard loop or for parsing
        } else {
          // Reached end of tokens while discarding
          break; // Exit discard loop
        }
      } // End of token discarding loop

      if (!recovered) {
        std::cerr << "  Error recovery failed. Stopping parse." << std::endl;
        return R; // R.ok is already false
      }
      // If recovered, the outer loop will continue.
      // 's' needs to be updated from stateSt.back() as stack might have been popped.
      // 'a' is already tk[ip].type (the sync token).
      if (stateSt.empty()) {
        std::cerr << "  Recovery led to empty state stack. Stopping parse." << std::endl;
        // R.ok is already false.
        return R;
      }
      s = stateSt.back(); // Update current state, 'a' is the current tk[ip].type
      std::cerr << "  Resuming parse from state " << s << " with token " << tk[ip].lexeme << " (" << TOKEN_NAMES[a] << ")" << std::endl;
      continue; // Retry parsing with the current state 's' and sync token 'a'
    }

    Action act = it->second;
    if (act.tag == SHIFT) {
      stateSt.push_back(act.param);
      /* 终结符节点 */
      symSt.push_back(std::make_unique<AST>(a));
      ++ip;
    }
    else if (act.tag == REDUCE) {
      const auto &pr = G[act.param];
      std::unique_ptr<AST> node = std::make_unique<AST>(pr.lhs);
      for (size_t k = 0; k < pr.rhs.size(); ++k) {
        stateSt.pop_back();
      }
      /* 弹 rhs 个数的 AST 作为孩子（逆序） */
      std::vector<std::unique_ptr<AST>> children;
      for (size_t k = 0; k < pr.rhs.size(); ++k) {
        children.push_back(std::move(symSt.back()));
        symSt.pop_back();
      }
      std::reverse(children.begin(), children.end());
      node->child = std::move(children);

      int t = stateSt.back();
      stateSt.push_back(GOTO_TBL[t][pr.lhs]);
      symSt.push_back(std::move(node));
      R.prod_seq.push_back(act.param); // 记下规约产生式号
    }
    else if (act.tag == ACCEPT) {
      R.ok = true;
      R.root = std::move(symSt.back());
      return R;
    }
  }
}
