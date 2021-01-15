#include "lexer.h"
#include "../error.h"
#include <stack>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace lexer;

Lexer::Lexer(std::string& program) {
  unsigned int current_index = 0;
  unsigned int tokenIndex = 0;
  unsigned int lineIndex = 1;

  // Tokenise the program, ignoring comments
  Token t;
  while(current_index <= program.length() && program.substr(current_index, program.size()).find_first_not_of('\0') != std::string::npos) {
    if (hasToken(program, current_index)) {
      t = nextToken(program, current_index, tokenIndex, lineIndex);

      if (t.type() == END) lineIndex++;
      tokens.push_back(t);
    }
  }
}

Lexer::~Lexer() = default;

Token Lexer::peekToken() {
  if(current_token < tokens.size())
    return tokens[current_token];
  else {
      std::string error = "Final token surpassed.";
      return Token(ERROR, error);
  }
}

Token Lexer::nextToken() {
  if(current_token < tokens.size())
      return tokens[current_token++];
  else {
      std::cout << "nextToken" << std::endl;
      std::string error = "Final token surpassed.";
      return Token(ERROR, error);
  }
}

std::vector<Token> Lexer::getTokens() {
  return tokens;
}

bool Lexer::hasToken(std::string &program, unsigned int &current_index) {
  while(current_index < program.length() && (program[current_index] == ' ' || program[current_index] == '\t'))
    current_index++;

  if (current_index == program.length()) return false;
  return true;
}

Token Lexer::nextToken(std::string &program, unsigned int &current_index, unsigned int &tokenIndex, unsigned int &lineIndex) {

    // Setup stack and lexeme
    int current_state = 0;
    std::stack<int> state_stack;
    char current_symbol;
    std::string lexeme;

    // Push 'BAD' state on the stack
    state_stack.push(-1);

    // // Ignore whitespaces or newlines in front of lexeme
    // while(current_index < program.length() &&
    //      (program[current_index] == ' ' || program[current_index] == '\n' ))
    //   current_index++;
    
    // While current state is not error state
    while(current_state != e){
      current_symbol = program[current_index];
      lexeme += current_symbol;

      // If current state is final, remove previously recorded final states
      if (f_states[current_state])
          while(!state_stack.empty())
              state_stack.pop();

      // and push current one on the stack
      state_stack.push(current_state);

      // Go to next state using delta function in DFA
      current_state = nextState(current_state, current_symbol);

      // std::cout << "[" << current_symbol << ", " << current_state << "]" << ", ";

      // Update current index for next iteration
      current_index++;
    }

    // std::cout << std::endl;

    // Rollback loop
    int errorIndex = current_index - 2;

    while(!f_states[current_state] && current_state != -1){
      current_state = state_stack.top();
      state_stack.pop();
      lexeme.pop_back();
      current_index--;
    }

    if(current_state == -1) {
      std::string::reverse_iterator rLineStart = std::find_if(program.rbegin() + program.size() - (errorIndex + 2), program.rend(), [](char c){ return c == '\n' || c == std::string::npos; });
      std::string::iterator lineStart = rLineStart.base();
      std::string statement(lineStart, std::find_if(lineStart, program.end(), [](char c){ return c == '\n' || c == std::string::npos; }));

      int startIndex = lineStart - program.begin();
      errorIndex = errorIndex - startIndex;

      throw LexicalError("Invalid token starting at position " + std::to_string(errorIndex + 2) + ".", statement, lineIndex, errorIndex + 1);
    }

    if(current_state >= 0 && f_states[current_state]) {
      return Token(current_state, std::move(lexeme), lineIndex, tokenIndex++);
    }
    else {
      throw LexicalError("Starting character is not recognised.", program, lineIndex, errorIndex);
    }
}

int Lexer::nextState(int s, char sigma) {

    /*
     * Check which transition type we have, and then refer to the
     * transition table.
     */
    switch(sigma){
      case '#':
        return t_table[HASHTAG][s];
      case '.':
        return t_table[DOT][s];
      case '"':
        return t_table[SPEECH][s];
      case '=':
        return t_table[EQUALS][s];
      case 'b':
        return t_table[ALPHA_B][s];
      case 'd':
        return t_table[ALPHA_D][s];
      case 'x':
        return t_table[ALPHA_X][s];
      case '[':
      case ']':
      case ',':
      case '!':
        return t_table[PUNCT][s];
      case '0':
        return t_table[ZERO][s];
      case '1':
        return t_table[ONE][s];
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        return t_table[TWO_SEVEN][s];
      case '8':
      case '9':
        return t_table[EIGHT_NINE][s];
      case '_':
        return t_table[UNDERSCORE][s];
      case ':':
        return t_table[COLON][s];
      case '\n':
        return t_table[ENDLINE][s];
      default:
        auto ascii = (int)sigma;

        // if alpha is in [A-F] or [a-f]
        if (((0x41 <= ascii) && (ascii <= 0x46)) ||
          ((0x61 <= ascii) && (ascii <= 0x66)))
          return t_table[A_TO_F][s];

        // if alpha is in [G-Z] or [g-z]
        if (((0x47 <= ascii) && (ascii <= 0x5A)) ||
          ((0x67 <= ascii) && (ascii <= 0x7A)))
          return t_table[G_TO_Z][s];

        // all else
        return t_table[OTHER][s];
    }
}