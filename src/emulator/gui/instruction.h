/**
 * @file instruction.h
 * Handles the GUI instruction window which displays and explains the current instruction machine code.
 * @author Rory Pinkney
 * @date 28/10/20
 */

#ifndef IRISC_INSTRUCTION_H
#define IRISC_INSTRUCTION_H

#include "../../parser/syntax.h"
#include "widgets/hoverbox.h"
#include "gui.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

namespace vm {
  class Instruction : gui::Window {
    private:
      Fl_Window* window;
      Fl_Box* line;
      Fl_Box* status;
      std::array<Fl_Box*, 32> bits;
      std::vector<widgets::HoverBox*> regions;
      Fl_Box* _title;
      Fl_Box* _details;

    public:
      Instruction();
      void set(syntax::InstructionNode*, bool);
      void describe(std::string, std::string);
  };
}

#endif //IRISC_INSTRUCTION_H
