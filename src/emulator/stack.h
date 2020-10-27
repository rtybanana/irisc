/**
 * @file stack.h
 * Helper class to emulate the stack in an ARMv7 computer.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_STACK_H
#define IRISC_STACK_H

#include <bitset>
#include <vector>
#include <FL/Fl_Window.H>

// THE STACK IS 8 BYTE ALIGNED - REMEMBER
namespace vm {
  class Stack {
    private:
      std::vector<uint32_t> _heap;
      Fl_Window* window;
      void createWindow();

    public:
      Stack();
      void push();
      void pop();
      void reset();
  };
}


#endif //IRISC_STACK_H