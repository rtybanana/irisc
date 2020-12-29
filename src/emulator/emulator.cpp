#include <iostream>
#include <bit>
#include <thread>
#include <chrono>
#include "emulator.h"
#include "../parser/parser.h"
#include "../error.h"
// #include "windows/gui.h"

using namespace vm;

Emulator::Emulator() : memory(), registers(), instruction(), _running(false) {};


void Emulator::reset() {
  registers.clear();
  // stack.reset();
}

void Emulator::execute(std::string statement) {
  lexer::Lexer lexer(statement);
  parser::Parser parser(lexer);
  syntax::Node* node = parser.parseSingle();


  // std::cout << "\n******* node *******\n" << node->toString() << "\n******* end  *******\n" << std::endl;

  if (node != nullptr) execute(node);
  free(node);
}

/**
 * Driver function to execute any instruction with a base class of InstructionNode
 */
void Emulator::execute(syntax::Node* node) {
  registers.prepare();

  if (dynamic_cast<syntax::InstructionNode*>(node)) {
    bool executed = false;
    if (dynamic_cast<syntax::BiOperandNode*>(node)) {
      executed = executeBiOperand(dynamic_cast<syntax::BiOperandNode*>(node));
    }
    else if (dynamic_cast<syntax::TriOperandNode*>(node)) {
      executed = executeTriOperand(dynamic_cast<syntax::TriOperandNode*>(node));
    }
    else if (dynamic_cast<syntax::ShiftNode*>(node)) {
      executed = executeShift(dynamic_cast<syntax::ShiftNode*>(node));
    }

    instruction.set(dynamic_cast<syntax::InstructionNode*>(node), executed);
  }
  
  else if (dynamic_cast<syntax::AllocationNode*>(node)) {

  }
  else if (dynamic_cast<syntax::LabelNode*>(node)) {
    throw InteractiveError("Label instructions are not executable on their own. Try using the editor (:editor) to execute multiple lines.", node->statement(), 0);
  }
}

/**
 * Parses and runs a string containing a series of statements
 */
void Emulator::run(std::string program) {
  lexer::Lexer lexer(program);
  // std::vector<lexer::Token> tokens = lexer.getTokens();
  // for (lexer::Token token : tokens) std::cout << lexer::tokenNames[token.type()] << ", ";
  // std::cout << std::endl;

  parser::Parser parser(lexer);
  std::vector<syntax::Node*> nodes = parser.parseMultiple();

  // std::cout << "\n******* nodes *******" << std::endl;
  // for (syntax::Node* node : nodes) std::cout << node->toString() << std::endl;
  // std::cout <<   "*******  end  *******\n" << std::endl;

  start(nodes);
  // for (auto node : nodes) free(node);
}

/**
 * Prepares the program nodes
 */
void Emulator::start(std::vector<syntax::Node*> nodes) {
  if (_running) return;

  memory.softReset();
  registers[syntax::PC] = memory.memstart();

  bool text = true;
  bool entry_point = false;
  std::vector<int> remove;
  for (int i = 0; i < nodes.size(); i++) {
    if (dynamic_cast<syntax::DirectiveNode*>(nodes[i])) {
      syntax::DirectiveNode* node = dynamic_cast<syntax::DirectiveNode*>(nodes[i]);
      if (node->isData()) text = false;
      else if (node->isText()) { text = true; remove.push_back(i); }
      else if (node->isGlobal()) entry_point = true;
    }
    else if (dynamic_cast<syntax::AllocationNode*>(nodes[i])) {
      if (text) throw AssemblyError("Cannot declare data outside of the data section.", nodes[i]->statement());
      else {
        memory.allocate(dynamic_cast<syntax::AllocationNode*>(nodes[i]));
      }
    }
    else if (dynamic_cast<syntax::LabelNode*>(nodes[i])) {
      if (!text) throw AssemblyError("Cannot declare branchable labels outside of the text section.", nodes[i]->statement());
      if (entry_point) {
        registers[syntax::PC] = memory.memstart() + ((i - remove.size()) * 32);
        entry_point = false;
      }

      syntax::LabelNode* node = dynamic_cast<syntax::LabelNode*>(nodes[i]);
      memory.addLabel(node->identifier(), i - remove.size());
      remove.push_back(i);
    }

    if (!text) remove.push_back(i);
  }

  std::reverse(remove.begin(), remove.end());
  for (int i : remove) {
    free(nodes[i]);
    nodes.erase(nodes.begin() + i);
  }

  std::vector<syntax::InstructionNode*> instructions;
  for (syntax::Node* node : nodes) instructions.push_back(dynamic_cast<syntax::InstructionNode*>(node));
  memory.setText(instructions);

  std::thread([this]{ this->run(); }).detach();
}

/**
 * 
 */ 
void Emulator::run() {
  _running = true;

  while (running()) {
    syntax::InstructionNode* node = memory.instruction(registers[syntax::PC]);
    std::cout << "PC: " << registers[syntax::PC] << ": " << node->toString() << std::endl;
    editor->highlightLine(node->statement()[0].lineNumber());

    if (dynamic_cast<syntax::BranchNode*>(node)) {
      registers.prepare();
      bool executed = executeBranch(dynamic_cast<syntax::BranchNode*>(node));
      if (!executed) registers[syntax::PC] += 32;                     // increment to the next instruction
    }
    else {
      execute(node);
      registers[syntax::PC] += 32;                                    // increment to the next instruction
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  _running = false;
  editor->highlightLine(-1);          // unhighlight all lines
  registers.prepare();
}

bool Emulator::running() {
  return _running && registers[syntax::PC] < (memory.memstart() + (memory.text().size() * 32));
}

void Emulator::stop() {
  _running = false;
}

/**
 * Simplifies the flex operand down to a single source value, fetching register values and applying shifts.
 */
uint32_t Emulator::deflex(syntax::FlexOperand flex) {
  int deflex;

  auto [Rm, shift, Rs, immShift] = flex.unpack();
  if (flex.isImm()) return std::rotr((uint32_t)std::get<int>(Rm), immShift);      // return value of immediate (short circuit)
  else deflex = registers[std::get<syntax::REGISTER>(Rm)];                        // get value in register

  if (flex.shifted()) {
    int shiftBy;
    if (flex.shiftedByImm()) shiftBy = std::get<int>(flex.Rs());                  // get value of immediate shift
    else shiftBy = registers[std::get<syntax::REGISTER>(flex.Rs())];              // get value in shift register

    deflex = applyFlexShift(flex.shift(), deflex, shiftBy);                           // apply the unpacked shift
  }
  
  return deflex;
}

/**
 * Applies a single shift operation.
 */
uint32_t Emulator::applyFlexShift(syntax::SHIFT shift, int value, int amount) {
  switch(shift) {
    case syntax::LSL:
      return value <<= amount;
    case syntax::LSR:
      if (amount == 0) amount = 32;       // special case for right shifts
      return value >>= amount;
    default:
      throw std::runtime_error("");
  }
}

/**
 * Executes any bi-operand instruction such as MOV, MVN and comparisions CMP, TST etc
 */
bool Emulator::executeBiOperand(syntax::BiOperandNode* instruction) {
  auto [op, cond, set, dest, flex] = instruction->unpack();           // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                            // returns early if condition code is not satisfied

  uint32_t src = deflex(flex);                                        // deflex the flex operand into a single value
  switch(op) {                                                        // check opcode and execute instruction
    case syntax::MOV:
      if (set) registers.setFlags(registers[dest], src, src);
      registers[dest] = src;
      break;
    case syntax::MVN:
      if (set) registers.setFlags(registers[dest], -src, -src);
      registers[dest] = -src;
      break;
    case syntax::CMP:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] - src, '-');
      break;
    case syntax::CMN:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] + src, '+');
      break;
    case syntax::TST:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] & src);
      break;
    case syntax::TEQ:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] ^ src);
      break;
  } 

  return true;
}

/**
 * TODO: implement arithmetic with carry (ADC SBC and RSC)
 * Executes any tri-operand arithmetic opereration (besides shifts)
 */
bool Emulator::executeTriOperand(syntax::TriOperandNode* instruction) {
  auto [op, cond, set, dest, src, flex] = instruction->unpack();    // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                          // returns early if condition code is not satisfied

  uint32_t n = registers[src];
  uint32_t m = deflex(flex);                                        // deflex the flex operand into a value
  int result;
  switch (op) {                                                      // check opcode and execute instruction
    case syntax::AND:
      if (set) registers.setFlags(n, m, (uint64_t)n & m);
      result = n & m;
      break;
    case syntax::EOR:
      if (set) registers.setFlags(n, m, (uint64_t)n ^ m);
      result = n ^ m;
      break;
    case syntax::ORR:
      if (set) registers.setFlags(n, m, (uint64_t)n | m);
      result = n | m;
      break;
    case syntax::ADD:
      if (set) registers.setFlags(n, m, (uint64_t)n + m, '+');
      result = n + m;
      break;
    case syntax::SUB:
      if (set) registers.setFlags(n, m, (uint64_t)n - m, '-');
      result = n - m;
      break;
    case syntax::RSB:
      if (set) registers.setFlags(m, n, (uint64_t)m - n, '-');
      result = m - n;
  } 

  registers[dest] = result;
  return true;
}

/**
 * TODO: implement ASR and ROR
 * Executes a shift operation
 */ 
bool Emulator::executeShift(syntax::ShiftNode* instruction) {
  auto [op, cond, set, dest, src1, src2] = instruction->unpack();     // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                            // returns early if condition code is not satisfied

  uint32_t n = registers[src1];
  uint32_t m;
  if (src2.index() == 1) m = registers[std::get<syntax::REGISTER>(src2)];
  else m = std::get<int>(src2);

  int result;
  switch (op) {                                                        // check opcode and execute instruction
    case syntax::LSL:
      if (set) registers.setFlags(n, m, n << m);
      result = n << m;
      break;
    case syntax::LSR:
      if (set) registers.setFlags(n, m, n >> m);
      result = n >> m;
      break;
    case syntax::ASR:
    case syntax::ROR:
    default:
      break;
  }

  registers[dest] = result;
  return true;
}

/**
 * TODO: implement
 * Executes a branch operation
 */ 
bool Emulator::executeBranch(syntax::BranchNode* instruction) {
  auto [op, cond, to] = instruction->unpack();
  if (!registers.checkFlags(cond)) return false;                          // returns early if condition code is not satisfied

  int address;
  if (to.index() == 0) address = registers[std::get<syntax::REGISTER>(to)];
  else address = memory.label(std::get<std::string>(to));
  
  switch (op) {
    case syntax::B:
      // std::cout << "B" << std::endl;
      registers[syntax::PC] = address;
      break;
    case syntax::BL:
      // std::cout << "BL" << std::endl;
      registers[syntax::LR] = registers[syntax::PC] + 32;
      registers[syntax::PC] = address;
      break;
    case syntax::BX:
      // std::cout << "BX" << std::endl;
      registers[syntax::PC] = address;
      break;
  }

  return true;
}

/**
 * Switches the emulator mode so that it knows to parse data or text.
 */
void Emulator::mode(MODE mode) {
  _mode = mode;
}