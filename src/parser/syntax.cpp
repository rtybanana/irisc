#include "syntax.h"
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

Node::Node(std::vector<lexer::Token> statement) : _statement(statement) {}

Node::Node(std::vector<lexer::Token> statement, unsigned int currentToken) : 
  _statement(statement), 
  currentToken(currentToken) 
{}

Node::~Node() {}

lexer::Token Node::nextToken() { 
  if (currentToken < _statement.size())
    return _statement[currentToken++];
  else throw SyntaxError("Unexpected instruction end '" + _statement.back().value() + "'.", _statement, lineNumber(), _statement.size() - 1);
}

lexer::Token Node::peekToken() { 
  if (currentToken < _statement.size())
    return _statement[currentToken];
  else throw SyntaxError("Unexpected instruction end '" + _statement.back().value() + "'.", _statement, lineNumber(), _statement.size() - 1);
}

bool Node::hasToken() {
  if (currentToken < _statement.size()) return true;
  return false;
}

std::string Node::toString() {
  std::string instruction = "";
  for (int i = 0; i < _statement.size(); i++) {
    std::stringstream oss;
    oss << (_statement[i].type() == lexer::COMMA || i == 0 ? "" : " ")
        << _statement[i].value();

    instruction += oss.str();  
  }

  return instruction;
}

/**
 * Functions for parsing various tokens
 */
bool Node::parseComma(lexer::Token token) {
  if (token.type() == lexer::COMMA) return true;
  else throw SyntaxError("COMMA expected between operands - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "', instead.", _statement, lineNumber(), currentToken - 1);
}

REGISTER Node::parseRegister(lexer::Token token) {
  if (token.type() == lexer::REGISTER) return regMap[token.value()];
  else throw SyntaxError("REGISTER expected - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "' instead.", _statement, lineNumber(), currentToken - 1);
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
  else throw SyntaxError("IMMEDIATE value expected - received " + lexer::tokenNames[token.type()] + " '" + token.value() + "' instead.", _statement, lineNumber(), token.tokenNumber());

  return std::strtoull(token.value().substr(start + 1, token.value().size()).c_str(), nullptr, base);
}

/**
 * Parse immediate with maximum bit-width
 */
uint32_t Node::parseImmediate(lexer::Token token, unsigned int bits) {
  uint64_t imm = parseImmediate(token);
  if (imm < pow(2, bits)) return imm;
  else throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") is greater than the " + std::to_string(bits) + "-bit maximum.", _statement, lineNumber(), token.tokenNumber());
}

/**
 * Parse immediate value with maximum bit-width and ARMv7 barrel shift applied if necessary
 */
uint32_t Node::parseImmediate(lexer::Token token, unsigned int bits, unsigned int& immShift) {
  uint64_t imm = parseImmediate(token); 
  if (imm == 0) return imm;                                   // return 0 if imm == 0 (short circuit)

  // std::cout << std::bitset<64>(imm) << std::endl;

  int bottombit = ffs(imm) - 1;
  int topbit = (int)std::log2(imm);
  // std::cout << bottombit << ", " << topbit << std::endl;
  if (topbit > 31)
    throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") cannot be represented in 32 bits.", _statement, lineNumber(), token.tokenNumber());
  if ((topbit - bottombit) > --bits)
    throw NumericalError("IMMEDIATE value '" + token.value() + "' (decimal " + std::to_string(imm) + ") cannot be implicitly represented with a maximum set-bit width of 8.", _statement, lineNumber(), token.tokenNumber());

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
  std::map<std::string, TOKEN>::reverse_iterator it = std::find_if(operations.rbegin(), operations.rend(), [token](const std::pair<std::string, TOKEN> op){ return token.value().substr(0, op.first.size()) == op.first; });
  TOKEN type = it->second;
  operation = it->first;

  if (type == BRANCH) {                                // branches are special cases because they are shorter instructions
    if (token.value().size() == 2 | token.value().size() == 4) {
      if (token.value()[1] == 'l') operation = "bl";
      else if (token.value()[1] == 'x') operation = "bx";
    }
  }
  
  std::string suffix = token.value().substr(operation.size(), token.value().size());
  if (suffix.size() == 1 || suffix.size() == 3) {  // valid operation suffixes are up to 3 letters long maximum
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


  if (peekToken().type() == lexer::REGISTER) 
    this->_Rd = parseRegister(peekToken());

  else if (peekToken().type() == lexer::OP_LABEL) 
    this->_Rd = peekToken().value().substr(0, peekToken().value().size());

  else throw SyntaxError("Expected either REGISTER or LABEL value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", statement, lineNumber(), currentToken);
  
  nextToken();
  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid instruction end.", statement, lineNumber(), peekToken().tokenNumber());
}

std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, std::string, int>>> BranchNode::assemble() {
  return {0, {{"", "", "", 0}}};
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

/**
 * Assembles this instruction into the proper machine code that would execute it on an ARM device.
 */
std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, std::string, int>>> BiOperandNode::assemble() {
  uint32_t instruction = 0;
  std::vector<std::tuple<std::string, std::string, std::string, int>> explanation;

  instruction = (instruction << 4) | _cond;
  explanation.push_back({"Condition Code", condTitle[_cond], condExplain[_cond], 4});

  instruction <<= 3;                // logical shift left 8 for data processing
  explanation.push_back({"Instruction Type", "Arithmetic Operation", "Indicates the organisation of bits to the processor so that the instruction can be decoded.", 3});

  instruction = (instruction << 4) | _op;
  explanation.push_back({"Operation Code", opTitle[_op], opExplain[_op], 4});

  instruction = (instruction << 1) | _setFlags;
  explanation.push_back({"CPSR Flags", (_setFlags ? "Set" : "Not Set"), flagExplain[_setFlags], 1});

  instruction <<= 4;
  explanation.push_back({"Second Operand", "Unused", "These bits are left unset because the instruction only has two operands.", 4});

  instruction = (instruction << 4) | _Rd;
  explanation.push_back({"First Operand", regTitle[_Rd], "The first operand is often referred to as the 'destination' register.", 4});

  if (_flex.isImm()) {                                                                          // operand is immediate
    int imm = std::get<int>(_flex.Rm());
    instruction = (instruction << 4) | _flex.immShift();
    explanation.push_back({"Barrel Shifter", (_flex.immShift() == 0 ? "Not Shifted" : "Rotated Right By " + std::to_string(_flex.immShift())), "The amount by which the eight bit immediate value is rotated right.", 4});
    instruction = (instruction << 8) | imm;
    explanation.push_back({"Immediate", "Value " + std::to_string(imm), "An eight bit immediate value. This value, along with the barrel shift, forms the second operand.", 8});
  }
  else if (_flex.isReg()) {                                                                     // operand is register
    if (_flex.shifted()) {                                                                      // operand is  optionally shifted
      SHIFT shiftOp = _flex.shift();
      if (_flex.shiftedByReg()) {                                                               // shifted by register
        REGISTER shift = std::get<REGISTER>(_flex.Rs());
        instruction = (instruction << 4) | shift;
        explanation.push_back({"Optional Shift Amount", regTitle[shift], "Shift the flexible operand by the value in " + regTitle[shift] + ".", 4});

        instruction = (instruction << 2) | shiftOp;
        explanation.push_back({"Optional Shift Operation", shiftTitle[shiftOp], "The type of shift to perform on the flexible operand.", 2});
        instruction = (instruction << 1) | 1;
        explanation.push_back({"Optional Shift Type", "Register", "The flexible operand is optionally shifted by a register value.", 1});
      }
      else if (_flex.shiftedByImm()) {                                                          // shifted by immediate
        int shift = std::get<int>(_flex.Rs());
        instruction = (instruction << 5) | shift;
        explanation.push_back({"Optional Shift Amount", "Immediate " + std::to_string(shift), "Shift by the provided five bit immediate value (" + std::to_string(shift) + ").", 5});

        instruction = ((instruction << 2) | shiftOp) << 1;
        explanation.push_back({"Optional Shift Operation", shiftTitle[shiftOp], "The type of shift to perform on the flexible operand.", 2});
        explanation.push_back({"Optional Shift Type", "Immediate", "The flexible operand is optionally shifted by an immediate value.", 1});
      }
      else throw AssemblyError("Optional shift operand Rs is neither a REGISTER nor IMMEDIATE value. Most likely a parser bug.", _statement, lineNumber());
    }
    else {                                                                                      // operand is not optionally shifted
      instruction <<= 8;
      explanation.push_back({"No Optional Shift", "N/A", "The flexible operand is not optionally shifted.", 8});
    }

    REGISTER reg = std::get<REGISTER>(_flex.Rm());
    instruction = (instruction << 4) | reg;
    explanation.push_back({"Flexible Operand", regTitle[reg], ". This operand has special properties in ARMv7. It can be either an immediate value or an optionally shifted register.", 4});
  }
  else throw AssemblyError("Source operand Rm is neither a REGISTER nor IMMEDIATE value. This is most likely a parser bug.", _statement, lineNumber());

  return {instruction, explanation};
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

  peekToken();                                                  // peek next token to see if it exists
  this->_flex = FlexOperand(statement, currentToken);           // parsing delegated to FlexOperand
}

/**
 * Assembles this instruction into the proper machine code that would execute it on an ARM device.
 */
std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, std::string, int>>> TriOperandNode::assemble() {
  uint32_t instruction = 0;
  std::vector<std::tuple<std::string, std::string, std::string, int>> explanation;

  instruction = (instruction << 4) | _cond;
  explanation.push_back({"Condition Code", condTitle[_cond], condExplain[_cond], 4});

  instruction <<= 3;                // logical shift left 8 for data processing
  explanation.push_back({"Instruction Type", "Arithmetic Operation", "Indicates the organisation of bits to the processor so that the instruction can be decoded.", 3});

  instruction = (instruction << 4) | _op;
  explanation.push_back({"Operation Code", opTitle[_op], opExplain[_op], 4});

  instruction = (instruction << 1) | _setFlags;
  explanation.push_back({"CPSR Flags", (_setFlags ? "Set" : "Not Set"), flagExplain[_setFlags], 1});

  instruction = (instruction << 4) | _Rn;
  explanation.push_back({"Second Operand", regTitle[_Rn], "The second operand is often referred to as a 'source' register.", 4});

  instruction = (instruction << 4) | _Rd;
  explanation.push_back({"First Operand", regTitle[_Rd], "The first operand is often referred to as the 'destination' register.", 4});

  if (_flex.isImm()) {                                                                          // operand is immediate
    int imm = std::get<int>(_flex.Rm());
    instruction = (instruction << 4) | _flex.immShift();
    explanation.push_back({"Barrel Shifter", (_flex.immShift() == 0 ? "Not Shifted" : "Rotated Right By " + std::to_string(_flex.immShift())), "The amount by which the eight bit immediate value is rotated right.", 4});
    instruction = (instruction << 8) | imm;
    explanation.push_back({"Immediate", "Value " + std::to_string(imm), "An eight bit immediate value. This value, along with the barrel shift, forms the second operand.", 8});
  }
  else if (_flex.isReg()) {                                                                     // operand is register
    if (_flex.shifted()) {                                                                      // operand is  optionally shifted
      SHIFT shiftOp = _flex.shift();
      if (_flex.shiftedByReg()) {                                                               // shifted by register
        REGISTER shift = std::get<REGISTER>(_flex.Rs());
        instruction = (instruction << 4) | shift;
        explanation.push_back({"Optional Shift Amount", regTitle[shift], "Shift by the value in " + regTitle[shift] + ".", 4});

        instruction = (instruction << 2) | shiftOp;
        explanation.push_back({"Optional Shift Operation", shiftTitle[shiftOp], "The type of shift to perform on the flexible operand.", 2});
        instruction = (instruction << 1) | 1;
        explanation.push_back({"Optional Shift Type", "Register", "The flexible operand is optionally shifted by a register value.", 1});
      }
      else if (_flex.shiftedByImm()) {                                                          // shifted by immediate
        int shift = std::get<int>(_flex.Rs());
        instruction = (instruction << 5) | shift;
        explanation.push_back({"Optional Shift Amount", "Immediate " + std::to_string(shift), "Shift by the provided five bit immediate value (" + std::to_string(shift) + ").", 5});

        instruction = ((instruction << 2) | shiftOp) << 1;
        explanation.push_back({"Optional Shift Operation", shiftTitle[shiftOp], "The type of shift to perform on the flexible operand.", 2});
        explanation.push_back({"Optional Shift Type", "Immediate", "The flexible operand is optionally shifted by an immediate value.", 1});
      }
      else throw AssemblyError("Optional shift operand Rs is neither a REGISTER nor IMMEDIATE value. Most likely a parser bug.", _statement, lineNumber());
    }
    else {                                                                                      // operand is not optionally shifted
      instruction <<= 8;
      explanation.push_back({"No Optional Shift", "N/A", "The flexible operand is not optionally shifted.", 8});
    }

    REGISTER reg = std::get<REGISTER>(_flex.Rm());
    instruction = (instruction << 4) | reg;
    explanation.push_back({"Flexible Operand", regTitle[reg], "This operand has special properties in ARMv7. It can be either an immediate value or an optionally shifted register.", 4});
  }
  else throw AssemblyError("Source operand Rm is neither a REGISTER nor IMMEDIATE value. This is most likely a parser bug.", _statement, lineNumber());

  return {instruction, explanation};
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
    try { flex = parseImmediate(peekToken(), 5); }          // attempt to parse as immediate by peeking at the next token
    catch(SyntaxError e) {  }                               // catch and carry on if syntax error (fail on numerical error)
  }

  if (flex.index() == 0)
    throw SyntaxError("Expected either REGISTER or IMMEDIATE value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", _statement, lineNumber(), currentToken); 
  
  nextToken();                                              // advance token because currently only peeked
  return flex;
}

std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, std::string, int>>> ShiftNode::assemble() {
  return {0, {{"", "", "", 0}}};
}


/**
 * LoadStoreNode
 * Responsible for parsing load/store operations the most complex instruction, syntactically
 */
LoadStoreNode::LoadStoreNode(std::vector<lexer::Token> statement) : InstructionNode(statement) {
  auto [operation, modifier, condition] = splitOpCode(nextToken());
  this->_op = opMap[operation];
  this->_setFlags = modifier.empty() ? false : true;
  this->_cond = condMap[condition];

  throw DeveloperError("Load and Store operations are not currently implemented. Sorry about that.", statement, lineNumber(), -1);
}

std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, std::string, int>>> LoadStoreNode::assemble() {
  return {0, {{"", "", "", 0}}};
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

  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid instruction end.", statement, lineNumber(), peekToken().tokenNumber());
};

void FlexOperand::parseShift() {
  parseComma(nextToken());
  if (peekToken().type()== lexer::SHIFT)
    this->_shift = shiftMap[nextToken().value()];
  else throw SyntaxError("The comma after the final operand indicates an optional shift, but no shift was found.", _statement, lineNumber(), currentToken);

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
    throw SyntaxError("Expected either REGISTER or IMMEDIATE value - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", _statement, lineNumber(), currentToken); 
  
  nextToken();                                                    // advance token because currently only peeked
  return flex;
}


/**
 * Node which holds a section change declaration
 */
DirectiveNode::DirectiveNode(std::vector<lexer::Token> statement) : Node(statement) {
  if (directiveMap.contains(peekToken().value())) {
    this->directive = directiveMap.at(nextToken().value());
  }
  else throw SyntaxError("Unrecognised directive '" + peekToken().value() + "'.", statement, lineNumber(), peekToken().tokenNumber());
  
  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid section declaration end.", statement, lineNumber(), peekToken().tokenNumber());
}


AllocationNode::AllocationNode(std::vector<lexer::Token> statement) : Node(statement) {
  this->_identifier = peekToken().value().substr(0, nextToken().value().size() - 1);

  lexer::Token typeDirective = nextToken();
  if (!syntax::typeMap.contains(typeDirective.value()))
    throw SyntaxError("Expected a DIRECTIVE to declare type for '" + this->_identifier + "' variable - received " + lexer::tokenNames[typeDirective.type()] + " '" + typeDirective.value() + "' instead.", statement, lineNumber(), typeDirective.tokenNumber()); 

  int type = syntax::typeMap.at(typeDirective.value()).index();
  if (type == 0) {         // .skip
    this->_value.emplace<size_t>(parseImmediate(makeImmediate(nextToken()), 32));
  }
  else if (type == 1) {    // .byte
    this->_value.emplace<uint8_t>(parseImmediate(makeImmediate(nextToken()), 8));
  }
  else if (type == 2) {    // .hword
    this->_value.emplace<uint16_t>(parseImmediate(makeImmediate(nextToken()), 16));
  }
  else if (type == 3) {    // .word
    this->_value.emplace<uint32_t>(parseImmediate(makeImmediate(nextToken()), 32));
  }
  else if (type == 4) {    // .asciz, .ascii, .string
    if (peekToken().type() != lexer::STRING) 
      throw SyntaxError("Expected STRING value for type directive '" + typeDirective.value() + "' - received " + lexer::tokenNames[peekToken().type()] + " '" + peekToken().value() + "' instead.", statement, lineNumber(), currentToken); 

    std::string str = nextToken().value();
    this->_value.emplace<std::string>(str.substr(str.find_first_of("\"") + 1, str.find_last_of("\"") - 1));
  }

  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid data declaration end.", statement, lineNumber(), peekToken().tokenNumber());

  throw DeveloperError("Variable declaration functionality is work in progress. Sorry about that.", statement, lineNumber(), -1);
}

lexer::Token AllocationNode::makeImmediate(lexer::Token token) {
  if (token.type() == lexer::BIN) return lexer::Token(lexer::IMM_BIN, token.value(), token.lineNumber(), token.tokenNumber());
  if (token.type() == lexer::OCT) return lexer::Token(lexer::IMM_OCT, token.value(), token.lineNumber(), token.tokenNumber());
  if (token.type() == lexer::DEC) return lexer::Token(lexer::IMM_DEC, token.value(), token.lineNumber(), token.tokenNumber());
  if (token.type() == lexer::HEX) return lexer::Token(lexer::IMM_HEX, token.value(), token.lineNumber(), token.tokenNumber());
  return token;
}

std::string AllocationNode::printValue() const {
  int type = _value.index();
  if (type == 0) {         // .skip
    size_t size = std::get<0>(_value);
    return std::to_string(size * 4) + " bytes of space";
  }
  else if (type == 1) {    // .byte
    uint8_t byte = std::get<1>(_value);
    return std::to_string(byte) + " as a byte";
  }
  else if (type == 2) {    // .hword
    uint16_t hword = std::get<2>(_value);
    return std::to_string(hword) + " as a halfword";
  }
  else if (type == 3) {    // .word
    uint32_t word = std::get<3>(_value);
    return std::to_string(word) + " as a word";
  }
  else return std::get<4>(_value);
}

/**
 * Node which holds a label indicating a named point in the program which can be branched to
 */
LabelNode::LabelNode(std::vector<lexer::Token> statement) : Node(statement) {
  this->_identifier = peekToken().value().substr(0, nextToken().value().size() - 1);

  if (hasToken()) throw SyntaxError("Unexpected token '" + peekToken().value() + "' after valid data declaration end.", statement, lineNumber(), peekToken().tokenNumber());
}


/**
 * Node which holds an error object so that all of the errors in a program can't be
 */ 
ErrorNode::ErrorNode(Error& error) : Node(std::vector<lexer::Token>{}), _error(error) {}