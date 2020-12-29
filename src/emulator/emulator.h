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
#include "windows/memory.h"
#include "windows/registers.h"
#include "windows/instruction.h"
#include "../parser/syntax.h"
#include "../ui/editor.h"
#include "constants.h"

namespace ui {
  class Editor;
}

namespace vm {

  class Emulator {
    private:
      // Heap heap;
      Memory memory;
      Registers registers;
      Instruction instruction;
      std::vector<syntax::Node*> program;
      Fl_Window* window;
      ui::Editor* editor;
      MODE _mode;
      bool _running;

      bool executeBiOperand(syntax::BiOperandNode*);
      bool executeTriOperand(syntax::TriOperandNode*);
      bool executeShift(syntax::ShiftNode*);
      bool executeBranch(syntax::BranchNode*);
      uint32_t deflex(syntax::FlexOperand);
      uint32_t applyFlexShift(syntax::SHIFT, int, int);
      bool running();

    public:
      Emulator();
      void reset();
      void setEditor(ui::Editor* editor) { this->editor = editor; };
      void calculateLabelOffsets();
      // void assemble()
      void execute(std::string);
      void execute(syntax::Node*);
      void run(std::string);
      void start(std::vector<syntax::Node*>);
      void run();
      void stop();
      void mode(MODE);
  };
}

#endif //IRISC_EMULATOR_H