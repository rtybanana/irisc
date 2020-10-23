#include "emulator.h"
#include <bit>

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
uint32_t Emulator::deflex(syntax::FlexOperand flex) {
  int deflex;

  auto [Rm, shift, Rs, immShift] = flex.unpack();
  if (flex.isImm()) return std::rotr((uint32_t)std::get<int>(Rm), immShift);      // return value of immediate (short circuit)
  else deflex = registers[std::get<syntax::REGISTER>(Rm)];                        // get value in register

  if (flex.shifted()) {
    int shiftBy;
    if (flex.shiftedByImm()) shiftBy = std::get<int>(flex.Rs());                  // get value of immediate shift
    else shiftBy = registers[std::get<syntax::REGISTER>(flex.Rs())];              // get value in shift register

    deflex = applyShift(flex.shift(), deflex, shiftBy);                           // apply the unpacked shift
  }
  
  return deflex;
}

/**
 * Applies a single shift operation.
 */
uint32_t Emulator::applyShift(syntax::SHIFT shift, int value, int amount) {
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

/** TODO: implement
 * Sets the CPSR flags based on the result of the executing instruction
 */
void Emulator::setFlags(uint32_t op1, uint32_t op2, uint64_t result, char _operator = ' ') {
  int sign1 = std::bitset<32>(op1)[31];               // sign of left hand operand
  int sign2 = std::bitset<32>(op2)[31];               // sign of right hand operand
  int signr = std::bitset<32>(result)[31];            // sign of result
  std::bitset<33> result_ext(result);

  std::cout << std::bitset<33>(result) << std::endl;

  std::cout << "\n" << "sign1: " << sign1 << ", sign2: " << sign2 << ", signr: " << signr << ", operator: " << _operator << std::endl;
  std::cout << result_ext[32] << " " << std::bitset<32>(result) << std::endl;

  cpsr[0] = result_ext[31] == 1;                      // msb = 1
  cpsr[1] = (uint32_t)result == 0;                    // all bits = 0
  cpsr[2] = result_ext[32];                           // unsigned overflow

  if (_operator == '+') cpsr[3] = sign1 == sign2 && sign1 != signr;         // two operands of the same sign result in changed sign
  else if (_operator == '-') cpsr[3] = sign1 != sign2 && sign2 == signr;    // signs different and result sign same as subtrahend
  
  std::cout << "  negative flag: " << cpsr[0] << "\n"
            << "  zero flag: " << cpsr[1] << "\n"
            << "  carry flag: " << cpsr[2] << "\n"
            << "  overflow flag: " << cpsr[3] << std::endl;

  return;                                        
}

void Emulator::executeBiOperand(syntax::BiOperandNode* instruction) {
  auto [op, cond, set, dest, flex] = instruction->unpack();           // unpack the instruction
  if (!checkCondition(cond)) return;                                  // returns early if condition code is not satisfied

  uint32_t src = deflex(flex);                                        // deflex the flex operand into a single value
  switch(op) {                                                        // check opcode and execute instruction
    case syntax::MOV:
      if (set) setFlags(registers[dest], src, src);
      registers[dest] = src;
      break;
    case syntax::MVN:
      if (set) setFlags(registers[dest], -src, -src);
      registers[dest] = -src;
      break;
    case syntax::CMP:
      setFlags(registers[dest], src, (uint64_t)registers[dest] - src, '-');
      break;
    case syntax::CMN:
      setFlags(registers[dest], src, (uint64_t)registers[dest] + src, '+');
      break;
    case syntax::TST:
      setFlags(registers[dest], src, (uint64_t)registers[dest] & src);
      break;
    case syntax::TEQ:
      setFlags(registers[dest], src, (uint64_t)registers[dest] ^ src);
      break;
  } 
}

void Emulator::executeTriOperand(syntax::TriOperandNode* instruction) {
  auto [op, cond, set, dest, src, flex] = instruction->unpack();    // unpack the instruction
  if (!checkCondition(cond)) return;                                // returns early if condition code is not satisfied

  uint32_t m = deflex(flex);                                             // deflex the flex operand into a value
  uint32_t n = registers[src];

  int result;
  switch(op) {                                                      // check opcode and execute instruction
    case syntax::ADD:
      if (set) setFlags(n, m, (uint64_t)n + m, '+');
      result = n + m;
      break;
    case syntax::SUB:
      if (set) setFlags(n, m, (uint64_t)n - m, '-');
      result = n - m;
      break;
    case syntax::RSB:
      if (set) setFlags(m, n, (uint64_t)m - n, '-');
      result = m - n;
  } 

  registers[dest] = result;
}