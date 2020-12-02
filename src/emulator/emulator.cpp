#include <bit>
#include "emulator.h"
// #include "windows/gui.h"

using namespace vm;

Emulator::Emulator() : heap(), stack(), registers(), instruction() {};


void Emulator::reset() {
  registers.clear();
  // stack.reset();
  // heap.reset();
}

/**
 * Driver function to execute any instruction with a base class of InstructionNode
 */
void Emulator::execute(syntax::InstructionNode* base) {
  registers.prepare();

  bool executed = false;
  if (dynamic_cast<syntax::BiOperandNode*>(base)) {
    executed = executeBiOperand(dynamic_cast<syntax::BiOperandNode*>(base));
  }
  else if (dynamic_cast<syntax::TriOperandNode*>(base)) {
    executed = executeTriOperand(dynamic_cast<syntax::TriOperandNode*>(base));
  }
  else if (dynamic_cast<syntax::ShiftNode*>(base)) {
    executed = executeShift(dynamic_cast<syntax::ShiftNode*>(base));
  }

  instruction.set(base, executed);
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
  switch(op) {                                                      // check opcode and execute instruction
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
  if (src2.index() == 1) m = registers[std::get<int>(src2)];
  else m = std::get<int>(src2);

  int result;
  switch(op) {                                                        // check opcode and execute instruction
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
 * Switches the emulator mode so that it knows to parse data or text.
 */
void Emulator::mode(MODE mode) {
  _mode = mode;
}