/**
 * @file emulator.h
 * Declares the main class of the ARMv7 virtual machine which contains a stack, heap and a set of registers.
 * Parsed instructions are executed here.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_EMULATOR_H
#define IRISC_EMULATOR_H

#include "heap.h"
#include "stack.h"
#include "../parser/syntax.h"
#include <iostream>
#include <bitset>

namespace vm {
  class Emulator {
    private:
      Heap heap;
      Stack stack;
      uint32_t registers[15];
      bool cpsr[4];
      std::vector<syntax::Node*> program;

      void executeBiOperand(syntax::BiOperandNode*);
      void executeTriOperand(syntax::TriOperandNode*);
      bool checkCondition(syntax::CONDITION);
      uint32_t deflex(syntax::FlexOperand);
      uint32_t applyShift(syntax::SHIFT, int, int);
      void setFlags(uint32_t, uint32_t, uint64_t, char);
      // std::pair<int, int> computeSigns(uint32_t, uint32_t);


    public:
      Emulator();
      void calculateLabelOffsets();
      void execute(syntax::InstructionNode*);
      void execute(std::vector<syntax::Node*>);
      void printRegisters() const { for (auto x = std::begin(registers); x != std::end(registers);){std::cout <<*x++<< ' ';} std::cout << std::endl; };   // fishbone operator
  };
}

#endif //IRISC_EMULATOR_H