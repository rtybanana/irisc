#ifndef IRISC_ERROR_H
#define IRISC_ERROR_H

#include "lexer/token.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <exception>

/**
 * Error class - virtual base class which other error classes extend.
 */ 
class Error : public std::exception {
  protected:
    std::string msg;
    std::vector<lexer::Token> statement;
    int tokenIndex;

  public:
    Error(std::string msg, std::vector<lexer::Token> statement, int tokenIndex);
    virtual const char* what() const noexcept override = 0;
    std::string constructHelper() const;
};

inline Error::Error(std::string msg, std::vector<lexer::Token> statement, int tokenIndex) : 
  std::exception(),
  msg(msg),
  statement(statement),
  tokenIndex(tokenIndex)
{}

inline std::string Error::constructHelper() const {
  std::string helper = "";
  for (int i = 0; i < statement.size(); i++) {
    std::stringstream oss;
    bool problemToken = i == tokenIndex;
    oss << (statement[i].type() == lexer::COMMA ? "" : " ")
        << (problemToken ? "\e[1;3;4m" : "")
        << statement[i].value()
        << (problemToken ? "\e[0m" : "");

    helper += oss.str();  
  }

  return helper;
}


/**
 * LexicalError class - informs the user of invalid characters in their input.
 */ 
class LexicalError : public std::exception {
  protected:
    std::string msg;
    std::string statement;
    unsigned int symbolIndex;

  public:
    LexicalError(std::string msg, std::string statement, unsigned int symbolIndex);
    const char* what() const noexcept override;
    std::string constructHelper() const;
};

inline LexicalError::LexicalError(std::string msg, std::string statement, unsigned int symbolIndex) : 
  std::exception(),
  msg(msg),
  statement(statement),
  symbolIndex(symbolIndex)
{}

inline std::string LexicalError::constructHelper() const {
  std::string helper = "";
  for (int i = 0; i < statement.size(); i++) {
    std::stringstream oss;
    bool problemToken = i == symbolIndex;
    oss << (problemToken ? "\e[1;3;4m" : "")
        << statement[i]
        << (problemToken ? "\e[0m" : "");

    helper += oss.str();  
  }

  return helper;
}

inline const char* LexicalError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mLexical Error\033[0m: " << msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * SyntaxError class - informs the user of the first syntax error reached in their code.
 */ 
class SyntaxError : public Error {
  public:
    SyntaxError(std::string msg, std::vector<lexer::Token> statement, int tokenIndex)
      : Error(msg, statement, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* SyntaxError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mSyntax Error\033[0m: " << msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * NumericalError class - informs the user that an immediate value they have entered is invalid.
 */
class NumericalError : public Error {
  public:
    NumericalError(std::string msg, std::vector<lexer::Token> statement, int tokenIndex)
      : Error(msg, statement, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* NumericalError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mNumerical Error\033[0m: " << msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * AssemblyError class - informs the user that an immediate value they have entered is invalid.
 */
class AssemblyError : public Error {
  public:
    AssemblyError(std::string msg, std::vector<lexer::Token> statement, int tokenIndex = -1)
      : Error(msg, statement, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* AssemblyError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mAssembly Error\033[0m: " << msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


#endif //IRISC_ERROR_H