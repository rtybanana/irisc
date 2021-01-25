#include "parser.h"
#include "../error.h"
#include <vector>
#include <iostream>
#include <variant>
#include <typeinfo>

using namespace syntax;

Parser::Parser(lexer::Lexer& lexer) : lexer(lexer) {}

syntax::Node* Parser::parseSingle() {
  std::vector<lexer::Token> statement;
  while (lexer.peekToken().type() != lexer::END && lexer.peekToken().type() != lexer::ERROR) {
    statement.push_back(lexer.nextToken());
  }

  if (statement.empty()) 
    return nullptr;                                 // safely return nullptr which can be caught and dealt with

  if (statement[0].type() == lexer::BI_OPERAND) 
    return new syntax::BiOperandNode(statement);
  
  if (statement[0].type() == lexer::TRI_OPERAND) 
    return new syntax::TriOperandNode(statement);
  
  if (statement[0].type() == lexer::SHIFT)
    return new syntax::ShiftNode(statement);

  if (statement[0].type() == lexer::BRANCH)
    return new syntax::BranchNode(statement);

  if (statement[0].type() == lexer::LABEL) {
    if (statement.size() == 1) return new syntax::LabelNode(statement);
    else return new syntax::AllocationNode(statement);
  }

  if (statement[0].type() == lexer::DIRECTIVE) {
    return new syntax::DirectiveNode(statement);
  }
    // throw RuntimeError("Label statement is not executable", statement, 0);
  if (statement[0].type() == lexer::LOAD_STORE)
    return new syntax::LoadStoreNode(statement);

  if (statement[0].type() == lexer::OP_LABEL) {
    throw SyntaxError("Invalid label-like token detected, did you forget a colon?", statement, statement[0].lineNumber(), 0);
  }

  throw SyntaxError("Unrecognised instruction", statement, statement[0].lineNumber(), 0);
}

std::vector<syntax::Node*> Parser::parseMultiple() {
  std::vector<syntax::Node*> nodes;
  while (lexer.peekToken().type() != lexer::ERROR) {
    while (lexer.peekToken().type() == lexer::END) lexer.nextToken();   // skip all newlines

    syntax::Node* node;
    try { node = parseSingle(); }
    catch (Error& e) {                                                  // catch here and add to node list as error
      node = new syntax::ErrorNode(e);
    }

    if (node != nullptr) nodes.push_back(node);
  }

  return nodes;
}