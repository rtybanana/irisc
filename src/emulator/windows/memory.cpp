#include "memory.h"
#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>

using namespace vm;

Memory::Memory() : stack(), data(), _text(), _memstart(0) {
  window = new Fl_Window(340,180,"Memory");
  Fl_Box *box = new Fl_Box(20,40,300,100,"Memory!");

  box->box(FL_UP_BOX);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);

  window->set_non_modal();
  window->end();
  window->show();

  std::cout << "stack wake main thread" << std::endl;

  Fl::awake();
};

/**
 * Safely disposes of any current instructions and replaces with a new program.
 */ 
void Memory::setText(std::vector<syntax::InstructionNode*> text) {
  for (syntax::InstructionNode* instruction : this->_text) free(instruction);
  this->_text.clear();
  this->_text = text;
}

void Memory::addLabel(std::string label, unsigned int index) {
  labels.insert({label, index});
}

unsigned int Memory::label(std::string label) const {
  // for (auto [key, value] : labels) std::cout << "[" << key << ", " << value << "]";
  int index = _memstart + (labels.at(label) * 32); 
  // std::cout << index << std::endl; 
  return index;
}

/**
 * Allocates data defined in the .data section(s) of the program
 * TODO: implement
 */
void Memory::allocate(syntax::AllocationNode* node) {
  return;
}

/**
 * Resets the the labels and text sections of the memory in preparation for a different program
 * to be assembled.
 */
void Memory::softReset() {
  labels.clear();
  _text.clear();
  // stack.clear();
}