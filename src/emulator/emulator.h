/**
 * @file emulator.h
 * Declares the main class of the ARMv7 virtual machine which contains a stack, heap and a set of registers.
 * Parsed instructions are executed here.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_EMULATOR_H
#define IRISC_EMULATOR_H

#include "gui/heap.h"
#include "gui/stack.h"
#include "gui/registers.h"
#include "gui/instruction.h"
#include "../parser/syntax.h"
#include <iostream>
#include <bitset>

namespace vm {
  class Emulator {
    private:
      Heap heap;
      Stack stack;
      Registers registers;
      Instruction instruction;
      bool cpsr[4];
      std::vector<syntax::Node*> program;

      bool executeBiOperand(syntax::BiOperandNode*);
      bool executeTriOperand(syntax::TriOperandNode*);
      bool executeShift(syntax::ShiftNode*);
      bool checkCondition(syntax::CONDITION);
      uint32_t deflex(syntax::FlexOperand);
      uint32_t applyFlexShift(syntax::SHIFT, int, int);
      void setFlags(uint32_t, uint32_t, uint64_t, char);

    public:
      Emulator();
      void reset();
      void calculateLabelOffsets();
      void execute(syntax::InstructionNode*);
      void execute(std::vector<syntax::Node*>);
      void printRegisters() const { registers.printRegisters(); };   // fishbone operator
  };
}

#endif //IRISC_EMULATOR_H