/**
 * @file virtualmachine.h
 * Contains the main class of the ARMv7 virtual machine which contains a stack, heap and a set of registers.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_EMULATOR_H
#define IRISC_EMULATOR_H

#include "heap.h"
#include "stack.h"
#include "../parser/syntax.h"

class Emulator {
  private:
    Heap heap;
    Stack stack;
    int registers[15];
    std::vector<syntax::Node*> program;

  public:
    void calculateLabelOffsets();
};

#endif //IRISC_EMULATOR_H