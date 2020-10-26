/**
 * @file emulator.h
 * Declares the main class of the ARMv7 virtual machine which contains a stack, heap and a set of registers.
 * Parsed instructions are executed here.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_REGISTERS_H
#define IRISC_REGISTERS_H

#include "../parser/syntax.h"
#include <iostream>
#include <bitset>

namespace vm {
  class Registers {
    private:
      uint32_t _registers[15];

    public:
      uint32_t get(syntax::REGISTER index) const { return _registers[index]; };
      void set(syntax::REGISTER index, uint32_t value) { _registers[index] = value; };
      void printRegisters() const { for (auto x = std::begin(_registers); x != std::end(_registers);){std::cout <<*x++<< ' ';} std::cout << std::endl; };   // fishbone operator

  }
}

#endif //IRISC_REGISTERS_H
