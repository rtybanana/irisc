/**
 * @file memory.h
 * Helper class to emulate the RAM in an ARMv7 computer. Responsible for handling the text and data sections; pushing and popping
 * to/from the stack; and managing the related GUI window.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_STACK_H
#define IRISC_STACK_H

#include <iostream>
#include <bitset>
#include <vector>
#include <map>
#include <FL/Fl_Window.H>
#include "../../parser/syntax.h"

// THE STACK IS 8 BYTE ALIGNED - REMEMBER
namespace vm {
  class Memory {
    private:
      std::vector<uint32_t> stack;
      std::vector<syntax::InstructionNode*> _text;
      std::vector<uint32_t> data;
      std::map<std::string, unsigned int> symbols;
      std::map<std::string, unsigned int> labels;
      size_t maximum_size;
      size_t current_size;
      size_t _memstart;

      Fl_Window* window;

    public:
      Memory();
      std::vector<syntax::InstructionNode*> text() const { return _text; };
      size_t memstart() const { return _memstart; };
      syntax::InstructionNode* instruction(uint32_t offset) { return _text[(offset - _memstart) / 32]; };
      void allocate(syntax::AllocationNode*);
      bool hasLabel(std::string label) const { return labels.contains(label); };
      void addLabel(std::string, unsigned int);
      unsigned int label(std::string label)const;
      void setText(std::vector<syntax::InstructionNode*>);
      void push();
      void pop();
      void softReset();
      void reset();
  };
}


#endif //IRISC_STACK_H