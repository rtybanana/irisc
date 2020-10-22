#include "syntax.h"
#include "../error.h"
#include <algorithm>
#include <iostream>
#include <bitset>
#include <bit>
#include <cmath>
#include <strings.h>

using namespace syntax;


/**
 * Node
 * Base class for all nodes in the syntax tree. Contains implementations for common methods e.g. 
 * error handling, token advancing and some common atomic parsing such as commas and immediates.
 */
Node::Node() = default;

Node::Node(std::vector<lexer::Token> statement) : statement(statement) {}

Node::Node(std::vector<lexer::Token> statement, unsigned int currentToken) : 
  statement(statement), 
  currentToken(currentToken) 
{}

Node::~Node() {}

lexer::Token Node::nextToken() { 
  if (currentToken < statement.size())
    return statement[currentToken++];
  else throw SyntaxError("Unexpected instruction end '" + statement.back().value() + "'.", statement, statement.size() - 1);
}

lexer::Token Node::peekToken() { 
  if (currentToken < statement.size())
    return statement[currentToken];
  else throw SyntaxError("Unexpected instruction end '" + statement.back().value() + "'.", statement, statement.size() - 1);
}

bool Node::hasToken() {
  if (currentToken < statement.size()) return true;
  return false;
}


/**
 * Functions for parsing various tokens
 */
bool Node::parseComma(lexer::Token token) {
  if (token.type() == lexer::COMMA) return true;
  else throw SyntaxError("COMMA expected between operands - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "', instead.", statement, currentToken - 1);
}

REGISTER Node::parseRegister(lexer::Token token) {
  if (token.type() == lexer::REGISTER) return regMap[token.value()];
  else throw SyntaxError("REGISTER expected - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "' instead.", statement, currentToken - 1);
}

uint64_t Node::parseImmediate(lexer::Token token) {
  int base = 0;
  int start;
  if (token.type() == lexer::IMM_BIN) {
    base = 2;
    start = token.value().find_last_not_of("01");
  }
  else if (token.type() == lexer::IMM_OCT) {
    base = 8;
    start = token.value().find_last_not_of("01234567");
  }
  else if (token.type() == lexer::IMM_DEC) {
    base = 10;
    start = token.value().find_last_not_of("0123456789");
  }
  else if (token.type() == lexer::IMM_HEX) {
    base = 16;
    start = token.value().find_last_not_of("0123456789abcdef");
  }
  else throw SyntaxError("IMMEDIATE value expected - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "' instead.", statement, token.tokenNumber());

  return std::strtoull(token.value().substr(start + 1, token.value().size()).c_str(), nullptr, base);
}

/**
 * Parse immediate with maximum bit-width
 */
uint32_t Node::parseImmediate(lexer::Token token, unsigned int bits) {
  uint64_t imm = parseImmediate(token);
  if (imm < pow(2, bits)) return imm;
  else throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") is greater than the " + std::to_string(bits) + "-bit maximum.", statement, token.tokenNumber());
}

/**
 * Parse immediate value with maximum bit-width and ARMv7 barrel shifter paradigm
 */
uint32_t Node::parseImmediate(lexer::Token token, unsigned int bits, unsigned int& immShift) {
  uint64_t imm = parseImmediate(token); 
  if (imm == 0) return imm;                                   // return 0 if imm == 0 (short circuit)

  // std::cout << std::bitset<64>(imm) << std::endl;

  int bottombit = ffs(imm) - 1;
  int topbit = (int)std::log2(imm);
  // std::cout << bottombit << ", " << topbit << std::endl;
  if (topbit > 31)
    throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") cannot be represented in 32 bits.", statement, token.tokenNumber());
  if ((topbit - bottombit) > --bits)
    throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") cannot be implicitly represented with a maximum set-bit width of 8.", statement, token.tokenNumber());

  if (topbit > bits) { 
    imm = std::rotr((uint32_t)imm, topbit - 7);
    immShift = 32 - (topbit - 7);
  }
    
  return imm;
}

/**
 * InstructionNode
 * Base class for all operation instructions. Contains implementations of common methods 
 * e.g. splitting the opcode into operation/modifier/condition.
 */
InstructionNode::InstructionNode(std::vector<lexer::Token> statement) : Node(statement) {}

std::tuple<std::string, std::string, std::string> InstructionNode::splitOpCode(lexer::Token token) {
  std::string operation;
  std::string modifier;
  std::string condition;

  std::string forceFlags[] = {"cmp", "cmn", "tst", "teq"};    // operations that always set flags regardless of modifier value

  using namespace lexer;
  std::map<std::string, TOKEN>::iterator it = std::find_if(operations.begin(), operations.end(), [token](const std::pair<std::string, TOKEN> op){ return token.value().substr(0, op.first.size()) == op.first; });
  operation = it->first;

  std::string suffix = token.value().substr(operation.size(), token.value().size());
  if (suffix.size() == 1 || suffix.size() == 3) {                                                       // valid operation suffixes are up to 3 letters long maximum
    modifier = suffix[0];
    suffix = suffix.substr(1, suffix.size());
  }                                                      
  condition = suffix;

  if (std::find(std::begin(forceFlags), std::end(forceFlags), operation) != std::end(forceFlags)) modifier = "s";

  return {operation, modifier, condition};
}


/**
 * BranchNode
 * Responsible for parsing and delegating parsing of branch instructions B, BL and BX
 */
BranchNode::BranchNode(std::vector<lexer::Token> statement) : InstructionNode(statement) {
  auto [operation, modifier, condition] = splitOpCode(nextToken());
  this->_op = opMap[operation];
  this->_setFlags = false;
  this->_cond = condMap[condition];


  try { this->_Rd = parseRegister(peekToken()); }               // attempt to parse as register by peeking at the next token
  catch(SyntaxError e) {  }                                     // catch and carry on if syntax error
  
  if (this->_Rd.index() == 0) {
    if (peekToken().type() == lexer::LABEL) this->_Rd = nextToken().value();
    else throw SyntaxError("Expected either REGISTER or LABEL value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", statement, currentToken);
  }
}

unsigned int BranchNode::assemble() {
  return 0;
}


/**
 * BiOperandNode
 * Responsible for parsing and delegating parsing of a binary operand instruction in ARMv7 assembly.
 */
BiOperandNode::BiOperandNode(std::vector<lexer::Token> statement) : InstructionNode(statement) {
  auto [operation, modifier, condition] = splitOpCode(nextToken());
  this->_op = opMap[operation];
  this->_setFlags = modifier.empty() ? false : true;
  this->_cond = condMap[condition];

  this->_Rd = parseRegister(nextToken());

  peekToken();                                              // peek next token to see if it exists
  this->_flex = FlexOperand(statement, currentToken);          // parsing delegated to FlexOperand
}

unsigned int BiOperandNode::assemble() {
  unsigned int instruction = 0;
  std::cout << "assembling..." << std::endl;

  std::cout << "condition: " << _cond << ", (" << std::bitset<4>(_cond) << ")" << std::endl;
  instruction |= _cond;

  std::cout << "next three clear bits (000) indicate arithmetic operation" << std::endl;
  instruction <<= 3;                // logical shift left 8 for data processing

  std::cout << "op code: " << _op << ", (" << std::bitset<4>(_op) << ")" << std::endl;
  instruction = (instruction << 4) | _op;

  std::cout << "set flags: " << (_setFlags ? "true" : "false") << ", (" << std::bitset<1>(_setFlags) << ")" << std::endl;
  instruction = (instruction << 1) | _setFlags;

  std::cout << "next four clear bits (0000) indicate that the operation has two operands" << std::endl;
  instruction <<= 4;

  std::cout << "dest register: " << _Rd << ", (" << std::bitset<4>(_Rd) << ")" << std::endl;
  instruction = (instruction << 4) | _Rd;

  if (_flex.isImm()) {                                                                          // operand is immediate
    int imm = std::get<int>(_flex.Rm());
    std::cout << "src value: " << std::rotr((uint32_t)imm, _flex.immShift()) << ", (" << std::bitset<8>(imm) << (_flex.immShift() > 0 ? " - barrel shifted right by " + std::to_string(_flex.immShift()) : "") << ")" << std::endl;
    instruction = (instruction << 4) | _flex.immShift();
    instruction = (instruction << 8) | imm;
  }
  else if (_flex.isReg()) {                                                                     // operand is register
    if (_flex.shifted()) {                                                                      // operand is  optionally shifted
      SHIFT shiftOp = _flex.shift();
      if (_flex.shiftedByReg()) {                                                               // shifted by register
        REGISTER shift = std::get<REGISTER>(_flex.Rs());
        std::cout << "shift register: " << shift << ", (" << std::bitset<4>(shift) << ")" << std::endl;
        instruction = (instruction << 4) | shift;

        std::cout << "next three bits (xx1) indicate the shift operation (xx) and that the operand is a register (1)" << std::endl;
        instruction = (instruction << 2) | shiftOp;
        instruction = (instruction << 1) | 1;
      }
      else if (_flex.shiftedByImm()) {                                                          // shifted by immediate
        int shift = std::get<int>(_flex.Rs());
        std::cout << "shift by immediate: " << shift << ", (" << std::bitset<5>(shift) << ")" << std::endl;
        instruction = (instruction << 5) | shift;

        std::cout << "next three bits (xx0) indicate the shift operation (xx) and that the operand is immediate (0)" << std::endl;
        instruction = ((instruction << 2) | shiftOp) << 1;
      }
      else throw AssemblyError("Optional shift operand Rs is neither a REGISTER nor IMMEDIATE value. Most likely a parser bug.", statement);
    }
    else {                                                                                      // operand is not optionally shifted
      std::cout << "next eight clear bits (00000000) indicate that the operation is not optionally shifted" << std::endl;
      instruction <<= 8;
    }

    REGISTER reg = std::get<REGISTER>(_flex.Rm());
    std::cout << "src register: " << reg << ", (" << std::bitset<4>(reg) << ")" << std::endl;
    instruction = (instruction << 4) | reg;
  }
  else throw AssemblyError("Source operand Rm is neither a REGISTER nor IMMEDIATE value. This is most likely a parser bug.", statement);

  std::cout << std::bitset<32>(instruction) << std::endl;
  return 0;
}


/**
 * TriOperandNode
 * Responsible for parsing and delegating parsing of a binary operand instruction in ARMv7 assembly.
 */
TriOperandNode::TriOperandNode(std::vector<lexer::Token> statement) : InstructionNode(statement) {
  auto [operation, modifier, condition] = splitOpCode(nextToken());
  this->_op = opMap[operation];
  this->_setFlags = modifier.empty() ? false : true;
  this->_cond = condMap[condition];

  this->_Rd = parseRegister(nextToken());
  parseComma(nextToken());
  this->_Rn = parseRegister(nextToken());

  peekToken();                                              // peek next token to see if it exists
  this->_flex = FlexOperand(statement, currentToken);          // parsing delegated to FlexOperand
}

unsigned int TriOperandNode::assemble() {
  return 0;
}

/**
 * ShiftNode
 * Responsible for parsing shift operations, a special form of TriOperandNode.
 */
ShiftNode::ShiftNode(std::vector<lexer::Token> statement) : InstructionNode(statement) {
  auto [operation, modifier, condition] = splitOpCode(nextToken());
  this->_op = opMap[operation];
  this->_setFlags = modifier.empty() ? false : true;
  this->_cond = condMap[condition];

  this->_Rd = parseRegister(nextToken());
  parseComma(nextToken());
  this->_Rn = parseRegister(nextToken());
  parseComma(nextToken());

  this->_Rs = parseRegOrImm();
}

std::variant<std::monostate, REGISTER, int> ShiftNode::parseRegOrImm() {
  std::variant<std::monostate, REGISTER, int> flex;
  try { flex = parseRegister(peekToken()); }                // attempt to parse as register by peeking at the next token
  catch(SyntaxError e) {  }                                 // catch and carry on if syntax error
  
  if (flex.index() == 0) {
    try { flex = parseImmediate(peekToken(), 5); }             // attempt to parse as immediate by peeking at the next token
    catch(SyntaxError e) {  }                               // catch and carry on if syntax error (fail on numerical error)
  }

  if (flex.index() == 0)
    throw SyntaxError("Expected either REGISTER or IMMEDIATE value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", statement, currentToken); 
  
  nextToken();                                              // advance token because currently only peeked
  return flex;
}

unsigned int ShiftNode::assemble() {
  return 0;
}


/**
 * FlexOperand 
 * Responsible for parsing the flexible operand 2 in ARMv7 assembly.
 */
FlexOperand::FlexOperand() = default;

FlexOperand::FlexOperand(std::vector<lexer::Token> statement, unsigned int currentToken) : Node(statement, currentToken) {
  parseComma(nextToken());
  this->_Rm = parseRegOrImm();      // parse immediate with default 8 bits (with extended 4 bit shift)
  if (_Rm.index() == 1 && hasToken()) parseShift();

  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid instruction end.", statement, peekToken().tokenNumber());
};

void FlexOperand::parseShift() {
  parseComma(nextToken());
  if (peekToken().type()== lexer::SHIFT)
    this->_shift = shiftMap[nextToken().value()];
  else throw SyntaxError("The comma after the final operand indicates an optional shift, but no shift was found.", statement, currentToken);

  this->_Rs = parseRegOrImm(5);     // parse immediate with a max length of 5 bits
}

std::variant<std::monostate, REGISTER, int> FlexOperand::parseRegOrImm(unsigned int immBits) {
  std::variant<std::monostate, REGISTER, int> flex;
  try { flex = parseRegister(peekToken()); }                      // attempt to parse as register by peeking at the next token
  catch(SyntaxError e) {  }                                       // catch and carry on if syntax error
  
  if (flex.index() == 0) {
    try { 
      unsigned int immShift = 0;
      if (immBits == 8) {
        flex = parseImmediate(peekToken(), immBits, immShift);    // attempt to parse as immediate by peeking at the next token
        this->_immShift = immShift;
      }
      else flex = parseImmediate(peekToken(), immBits);
    }                                                           
    catch(SyntaxError e) {  }                                     // catch and carry on if syntax error (fail on numerical error)
  }

  if (flex.index() == 0)
    throw SyntaxError("Expected either REGISTER or IMMEDIATE value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", statement, currentToken); 
  
  nextToken();                                                    // advance token because currently only peeked
  return flex;
}