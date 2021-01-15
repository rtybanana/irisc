/**
 * @file instruction.h
 * Handles the GUI instruction window which displays and explains the current instruction machine code.
 * @author Rory Pinkney
 * @date 28/10/20
 */

#ifndef IRISC_INSTRUCTION_H
#define IRISC_INSTRUCTION_H

#include "../../parser/syntax.h"
#include "../../widgets/hoverbox.h"
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>

namespace vm {

  class Instruction : public Fl_Group {
    private:
      Fl_Box* line;
      Fl_Box* status;
      std::array<Fl_Box*, 32> bits;
      std::vector<widgets::HoverBox*> regions;
      Fl_Box* _title;
      Fl_Box* _details;
      std::string cmd_string;
      bool headless;

    public:
      Instruction(int, int, bool = false);
      // Fl_Window* window;
      // void draw();
      void set(syntax::InstructionNode*, bool);
      void describe(std::string, std::string);
      void print();
  };

}

#endif //IRISC_INSTRUCTION_H
