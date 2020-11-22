#include "parser.h"
#include "../error.h"
#include <vector>
#include <iostream>

namespace parser {
  Parser::Parser(lexer::Lexer& lexer) : lexer(lexer) {}

  syntax::InstructionNode* Parser::parseSingle() {

    std::vector<lexer::Token> statement;
    while (lexer.peekToken().type() != lexer::END && lexer.peekToken().type() != lexer::ERROR) {
      statement.push_back(lexer.nextToken());
    }

    if (statement[0].type() == lexer::BI_OPERAND) 
      return new syntax::BiOperandNode(statement);
    
    if (statement[0].type() == lexer::TRI_OPERAND) 
      return new syntax::TriOperandNode(statement);
    
    if (statement[0].type() == lexer::SHIFT)
      return new syntax::ShiftNode(statement);

    if (statement[0].type() == lexer::BRANCH)
      return new syntax::BranchNode(statement);

    // if (statement[0].type() == lexer::LOAD_STORE)
    //   return new syntax::LoadStoreNode(statement);

    throw SyntaxError("Unrecognised instruction", statement, 0);
    // return new syntax::BiOperandNode();
  }
}