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
    std::string _msg;
    std::vector<lexer::Token> _statement;
    int _tokenIndex;
    int _lineNumber;

  public:
    Error(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex) : 
      std::exception(),
      _msg(msg),
      _statement(statement),
      _lineNumber(lineNumber),
      _tokenIndex(tokenIndex) {};
    Error(std::string msg, int lineNumber) :
      std::exception(),
      _msg(msg),
      _lineNumber(lineNumber) {};
    
    virtual const char* what() const noexcept override = 0;
    std::string constructHelper() const;
    std::string msg() const { return _msg; };
    std::vector<lexer::Token> statement() const { return _statement; };
    int lineNumber() const { return _lineNumber; };
    int tokenIndex() const { return _tokenIndex; };
};

inline std::string Error::constructHelper() const {
  std::string helper = "";
  for (int i = 0; i < _statement.size(); i++) {
    std::stringstream oss;
    bool problemToken = i == _tokenIndex;
    oss << (_statement[i].type() == lexer::COMMA ? "" : " ")
        << (problemToken ? "\e[1;3;4m" : "")
        << _statement[i].value()
        << (problemToken ? "\e[0m" : "");

    helper += oss.str();  
  }

  return std::to_string(_lineNumber) + ": " + helper;
}


/**
 * LexicalError class - informs the user of invalid characters in their input.
 */ 
class LexicalError : public Error{
  protected:
    std::string _statementStr;
    int _symbolIndex;

  public:
    LexicalError(std::string msg, std::string statementStr, int lineNumber, int symbolIndex) : 
      Error(msg, lineNumber),
      _statementStr(statementStr),
      _symbolIndex(symbolIndex)
    {};
    const char* what() const noexcept override;
    std::string constructHelper() const;
    std::string statementStr() const { return _statementStr; };
    int symbolIndex() const { return _symbolIndex; };
};

inline std::string LexicalError::constructHelper() const {
  std::string helper = "";
  for (int i = 0; i < _statementStr.size(); i++) {
    std::stringstream oss;
    bool problemToken = i == _symbolIndex;
    oss << (problemToken ? "\e[1;3;4m" : "")
        << _statementStr[i]
        << (problemToken ? "\e[0m" : "");

    helper += oss.str();  
  }

  return std::to_string(_lineNumber) + ": " + helper;
}

inline const char* LexicalError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mLexical Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * SyntaxError class - informs the user of the first syntax error reached in their code.
 */ 
class SyntaxError : public Error {
  public:
    SyntaxError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* SyntaxError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mSyntax Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * NumericalError class - informs the user that an immediate value they have entered is invalid.
 */
class NumericalError : public Error {
  public:
    NumericalError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* NumericalError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mNumerical Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * AssemblyError class - informs the user that an error occured while assembling their code, usually an internal bug.
 */
class AssemblyError : public Error {
  public:
    AssemblyError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex = -1)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* AssemblyError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mAssembly Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * RuntimeError class - informs the user that an error occurred while executing their code, such as a segmentation error.
 */
class RuntimeError : public Error {
  public:
    RuntimeError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    RuntimeError(std::string msg)
      : Error(msg, std::vector<lexer::Token>(), -1, -1) {};
    const char* what() const noexcept override;
};

inline const char* RuntimeError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mRuntime Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * InteractiveError class - informs the user that an error occured while assembling their code, usually an internal bug.
 */
class InteractiveError : public Error {
  public:
    InteractiveError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* InteractiveError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mInteractive Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


/**
 * DeveloperError class - tongue in cheek class for when the user tries to access WIP functionality
 */
class DeveloperError : public Error {
  public:
    DeveloperError(std::string msg, std::vector<lexer::Token> statement, int lineNumber, int tokenIndex)
      : Error(msg, statement, lineNumber, tokenIndex) {};
    const char* what() const noexcept override;
};

inline const char* DeveloperError::what() const noexcept {
  std::stringstream stream;
  stream << "\033[91mDeveloper Error\033[0m: " << _msg << "\n\t" << constructHelper();

  std::string* out = new std::string(stream.str());
  return out->c_str();
}


#endif //IRISC_ERROR_H