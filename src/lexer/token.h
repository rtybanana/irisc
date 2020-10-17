/**
 * @file token.h
 * Contains 
 * @author Rory Pinkney
 * @date 6/10/20
 */

#ifndef IRISC_TOKEN_H
#define IRISC_TOKEN_H

#include <string>
#include <vector>

namespace lexer {

  enum TOKEN {
    // general
    LABEL = 0,    

    // operations - reduced for now
    BRANCH,
    BI_OPERAND,
    TRI_OPERAND,
    LOAD_STORE,
    SHIFT,

    // operands
    REGISTER,
    IMM_BIN,
    IMM_OCT,
    IMM_DEC,
    IMM_HEX,
    VARIABLE,
    OP_LABEL,

    // punctuation
    COMMA,
    OPEN_SQR,
    CLOSE_SQR,
    EXCLAMATION,

    END,
    ERROR
  };

  static std::string tokenNames[] = {
    "LABEL",
    "BRANCH",
    "BI_OPERAND",
    "TRI_OPERAND",
    "LOAD_STORE",
    "SHIFT",

    "REGISTER",
    "IMM_BIN",
    "IMM_OCT",
    "IMM_DEC",
    "IMM_HEX",
    "VARIABLE",
    "OP_LABEL",

    "COMMA",
    "OPEN_SQR",
    "CLOSE_SQR",
    "EXCLAMATION",
    
    "END",
    "ERROR"
  };

  static std::vector<std::string> operations = {"mov", "cmp", "add", "sub", "ldr", "str", "lsl", "lsr", "b"};
  static std::vector<char> modifiers = {'l'};
  static std::vector<std::string> conditions = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "al"};
  static std::vector<char> sizes = {'b', 'h'};

  class Token {
    public:
      Token();
      Token(int, std::string, unsigned int lineNumber = 0, unsigned int tokenNumber = 0);
      Token(TOKEN, std::string, unsigned int lineNumber = 0, unsigned int tokenNumber = 0);
      TOKEN type() const { return m_type; };
      std::string value() const { return m_value; };
      unsigned int lineNumber() const { return m_lineNumber; };
      unsigned int tokenNumber() const { return m_tokenNumber; };

    private:
      TOKEN m_type;
      std::string m_value;
      unsigned int m_lineNumber;
      unsigned int m_tokenNumber;
      TOKEN tokenType(int, std::string&);
  };
}


#endif //IRISC_TOKEN_H