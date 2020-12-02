/**
 * @file emulator.h
 * Declares the main class of the ARMv7 virtual machine which contains a stack, heap and a set of registers.
 * Parsed instructions are executed here.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_EMULATOR_H
#define IRISC_EMULATOR_H

#include <iostream>
#include <bitset>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include "windows/heap.h"
#include "windows/stack.h"
#include "windows/registers.h"
#include "windows/instruction.h"
#include "../parser/syntax.h"
#include "constants.h"

namespace vm {

  class Emulator {
    private:
      Heap heap;
      Stack stack;
      Registers registers;
      Instruction instruction;
      std::vector<syntax::Node*> program;
      Fl_Window* window;
      MODE _mode;

      bool executeBiOperand(syntax::BiOperandNode*);
      bool executeTriOperand(syntax::TriOperandNode*);
      bool executeShift(syntax::ShiftNode*);
      uint32_t deflex(syntax::FlexOperand);
      uint32_t applyFlexShift(syntax::SHIFT, int, int);

    public:
      Emulator();
      void reset();
      void calculateLabelOffsets();
      void execute(syntax::InstructionNode*);
      void execute(std::vector<syntax::Node*>);
      void mode(MODE);
  };
}

#endif //IRISC_EMULATOR_H