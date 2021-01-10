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
#include "gui/memory.h"
#include "gui/registers.h"
#include "gui/instruction.h"
#include "gui/editor.h"
#include "../parser/syntax.h"
#include "constants.h"

namespace vm {

  class Editor;

  class Emulator {
    private:
      // Heap heap;
      Memory memory;
      Registers registers;
      Instruction instruction;
      Editor* editor;
      // std::vector<syntax::Node*> program;
      // Fl_Window* registers_w;
      // Fl_Window* instruction_w;
      // Fl_Window* editor_w;
      Fl_Window* window;
      // ui::Editor* editor;
      MODE _mode;
      bool _running;
      bool _paused;
      bool _step;
      double delay;

      bool executeBiOperand(syntax::BiOperandNode*);
      bool executeTriOperand(syntax::TriOperandNode*);
      bool executeShift(syntax::ShiftNode*);
      bool executeBranch(syntax::BranchNode*);
      uint32_t deflex(syntax::FlexOperand);
      uint32_t applyFlexShift(syntax::SHIFT, int, int);

    public:
      Emulator();
      bool running();
      bool paused() const { return _paused; };
      
      void reset();
      void toggleEditor();
      void calculateLabelOffsets();
      // void assemble()
      void execute(std::string);
      void execute(syntax::Node*);
      void compile(std::string);
      void running(bool);
      void run(std::string);
      void start(std::vector<syntax::Node*>, bool run = true);
      void run();
      void pause() { _paused = true; };
      void step();
      void resume() { _paused = false; };
      void stop();
      void speed(double);
      void mode(MODE);
  };
}

#endif //IRISC_EMULATOR_H