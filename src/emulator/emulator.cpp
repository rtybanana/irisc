#include "emulator.h"

using namespace vm;

Emulator::Emulator() : heap(), stack(), registers {0} {};

void Emulator::execute(syntax::InstructionNode* base) {
  if (dynamic_cast<syntax::BiOperandNode*>(base)) {
    return executeBiOperand(dynamic_cast<syntax::BiOperandNode*>(base));
  }
  if (dynamic_cast<syntax::TriOperandNode*>(base)) {
    return executeTriOperand(dynamic_cast<syntax::TriOperandNode*>(base));
  }
}

/** TODO: implement
 * Checks the CPSR flags against the current condition code to determine if the instruction should be executed
 */
bool Emulator::checkCondition(syntax::CONDITION) {
  return true;                                        
}

/**
 * Simplifies the flex operand down to a single source value, fetching register values and applying shifts.
 */
int Emulator::deflex(syntax::FlexOperand flex) {
  int deflex;

  auto [Rm, shift, Rs] = flex.unpack();
  if (flex.isImm()) return std::get<int>(Rm);                               // return value of immediate (short circuit)
  else deflex = registers[std::get<syntax::REGISTER>(Rm)];                  // get value in register

  if (flex.shifted()) {
    int shiftBy;
    if (flex.shiftedByImm()) shiftBy = std::get<int>(flex.Rs());            // get value of immediate shift
    else shiftBy = registers[std::get<syntax::REGISTER>(flex.Rs())];        // get value in shift register

    deflex = applyShift(flex.shift(), deflex, shiftBy);
  }
  
  return deflex;
}

/**
 * Applies a single shift operation.
 */
int Emulator::applyShift(syntax::SHIFT shift, int value, int amount) {
  switch(shift) {
    case syntax::LSL:
      return value <<= amount;
    case syntax::LSR:
      return value >>= amount;
    default:
      throw std::runtime_error("");
  }
}

/** TODO: implement
 * Checks the CPSR flags against the current condition code to determine if the instruction should be executed
 */
void Emulator::setFlags(int value) {
  return;                                        
}

void Emulator::executeBiOperand(syntax::BiOperandNode* instruction) {
  auto [op, cond, set, dest, flex] = instruction->unpack();         // unpack the instruction
  if (!checkCondition(cond)) return;                                // returns early if condition code is not satisfied

  int src = deflex(flex);                                           // deflex the flex operand into a value
  switch(op) {                                                      // check opcode and execute instruction
    case syntax::MOV:
      registers[dest] = src;
      if (set) setFlags(registers[dest]);
      break;
    case syntax::CMP:
      setFlags(registers[dest] - src);
      break;
    case syntax::CMN:
      setFlags(registers[dest] - (- src));
      break;
    case syntax::TST:
      setFlags(registers[dest] & src);
      break;
    case syntax::TEQ:
      setFlags(registers[dest] ^ src);
      break;
  } 
}

void Emulator::executeTriOperand(syntax::TriOperandNode* instruction) {
  auto [op, cond, set, dest, src, flex] = instruction->unpack();    // unpack the instruction
  if (!checkCondition(cond)) return;                                // returns early if condition code is not satisfied

  int m = deflex(flex);                                             // deflex the flex operand into a value
  int n = registers[src];
  int result;
  switch(op) {                                                      // check opcode and execute instruction
    case syntax::ADD:
      result = n + m;
      break;
    case syntax::SUB:
      result = n - m;
      break;
    case syntax::RSB:
      result = m - n;
  } 

  registers[dest] = result;
  if (set) setFlags(registers[dest]);
}