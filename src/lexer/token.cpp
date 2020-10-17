/**
 * @file token.cpp
 * @author Rory Pinkney
 * @date 7/10/20
 */

#include <iostream>
#include <algorithm>
#include <cctype>
#include "token.h"

using namespace lexer;

Token::Token() = default;

Token::Token(int final_state, std::string value, unsigned int lineNumber, unsigned int tokenNumber) :
  m_type(tokenType(final_state, value)),
  m_value(value),
  m_lineNumber(lineNumber),
  m_tokenNumber(tokenNumber)
{}

Token::Token(TOKEN type, std::string value, unsigned int lineNumber, unsigned int tokenNumber) :
  m_type(type),
  m_value(value),
  m_lineNumber(lineNumber),
  m_tokenNumber(tokenNumber)
{}

TOKEN Token::tokenType(int final_state, std::string &value) {
    // to lower case
    std::transform(
      value.begin(), 
      value.end(), 
      value.begin(), 
      [](unsigned char c){ return std::tolower(c); }
    );

    switch(final_state) {
        case 1: {
          TOKEN token;
          std::vector<std::string>::iterator it = std::find_if(operations.begin(), operations.end(), [value](const std::string s){ return value.substr(0, s.size()) == s; });
          if (it != operations.end()) {
            std::string operation = *it;
            if (operation == "mov" || operation == "cmp")
              token = BI_OPERAND;
            else if (operation == "add" || operation == "sub")
              token = TRI_OPERAND;
            else if (operation == "ldr" || operation == "str")
              token = LOAD_STORE;
            else if (operation == "b")
              token = BRANCH;
            else 
              token = SHIFT;

            std::string suffix = value.substr(operation.size(), value.size());
            if (suffix.size() == 1 || suffix.size() == 3) {                                                       // valid operation suffixes are up to 3 letters long maximum
              char modifier = suffix[0];
              suffix = suffix.substr(1, suffix.size());
              
              if (suffix.size() == 0 || std::any_of(conditions.begin(), conditions.end(), [suffix](const std::string s){ return s == suffix; })) {
                if ( token == LOAD_STORE && std::any_of(sizes.begin(), sizes.end(), [modifier](const char c){ return c == modifier; }) ||
                     token == BRANCH && std::any_of(modifiers.begin(), modifiers.end(), [modifier](const char c){ return c == modifier; }) ||
                    (token == BI_OPERAND || token == TRI_OPERAND) && modifier == 's') {
                  return token;
                }
              }
            }                                                      
            else if (suffix.size() == 0 || suffix.size() == 2 && std::any_of(conditions.begin(), conditions.end(), [suffix](const std::string s){ return s == suffix; })) {
              return token;
            }      
          }
          if (value[0] == 'r' && value.size() <= 3) {
            if (std::all_of(value.begin() + 1, value.end(), ::isdigit)) {                   // all remaining characters are digits
              if (std::strtol(value.substr(1, value.size()).c_str(), nullptr, 10) < 13)     // digits value < 13 (valid register)
                return REGISTER;
            }
          }
          if (value == "sp" || value == "lr" || value == "pc") {
            return REGISTER;
          }
        }
        case 2:                       // fall through
          if (value[0] == '=')
            return VARIABLE;
          return OP_LABEL;
        case 3:
          if (value[0] == '=') 
            return ERROR;
          return LABEL;
        case 4:
          if (value[0] == '#')
            return IMM_DEC;
          if (value[0] == 'r' || value[0] == 'R')
            return REGISTER;
          return ERROR;
        case 6:
          return IMM_OCT;
        case 7:
          return IMM_HEX;
        case 8:
          return IMM_BIN;
        case 9:
          if (value == "[")
            return OPEN_SQR;
          if (value == "]")
            return CLOSE_SQR;
          if (value == ",")
            return COMMA;
          if (value == "!")
            return EXCLAMATION;
          return END;
        default:
          return ERROR;
    }
}