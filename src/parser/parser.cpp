#include "parser.h"
#include <vector>
#include <iostream>

namespace parser {
  Parser::Parser(lexer::Lexer& lexer) : lexer(lexer) {
  }

  syntax::InstructionNode* Parser::parseSingle() {
    std::vector<lexer::Token> statement;
    while (lexer.peekToken().type() != lexer::END) {
      statement.push_back(lexer.nextToken());
    }

    if (statement[0].type() == lexer::BI_OPERAND) 
      return new syntax::BiOperandNode(statement);
    
    if (statement[0].type() == lexer::TRI_OPERAND) 
      return new syntax::TriOperandNode(statement);
    
    if (statement[0].type() == lexer::SHIFT)
      return new syntax::ShiftOperation(statement);

    // return new syntax::BiOperandNode();
  }
}