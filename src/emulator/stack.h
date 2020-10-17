/**
 * @file stack.h
 * Helper class to emulate the stack in an ARMv7 computer.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_STACK_H
#define IRISC_STACK_H

#include <vector>

// THE STACK IS 8 BYTE ALIGNED - REMEMBER
class Stack {
  private:
    std::vector<int> heap;

  public:
    void push();
    void pop();
};

#endif //IRISC_STACK_H