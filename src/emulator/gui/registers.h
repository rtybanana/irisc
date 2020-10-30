/**
 * @file registers.h
 * Emulates the registers in an ARMv7 processor. Responsible for managing the getting and setting of 
 * register values and displaying the related GUI window.
 * @author Rory Pinkney
 * @date 20/10/20
 */

#ifndef IRISC_REGISTERS_H
#define IRISC_REGISTERS_H

#include "../../parser/syntax.h"
#include "gui.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

namespace vm {
  class Registers : gui::Window {
    private:

      class proxy {
        public:
          Registers* container;
          int index;
          uint32_t value;

          proxy() : container(nullptr), value(0), index(0) {};
          proxy(Registers* container, int index) : container(container), value(0), index(index) {};
          proxy& operator=(const proxy* that) { 
            value = that->value; 
            container = that->container; 
            return *this; 
          };
          proxy& operator=(const uint32_t _value) { 
            this->value = _value; 
            container->updateReg(index, value);
            return *this; 
          };
          operator int() { return value; };
          friend std::ostream& operator<<(std::ostream& os, const proxy& p) { os << p.value; return os; };
      };

      std::array<proxy, 16> _registers;
      Fl_Window* window;
      std::array<Fl_Box*, 16> labels;
      std::string regstr(uint32_t);
      // void updateWindow();

    public:
      Registers();
      void update();
      void updateReg(int, uint32_t);
      void prepare();
      void clear();
      void printRegisters() const { for (auto x = std::begin(_registers); x != std::end(_registers);){std::cout <<*x++<< ' ';} std::cout << std::endl; };   // fishbone operator
      proxy& operator[] (int index) { return _registers[index]; };
  };
}

#endif //IRISC_REGISTERS_H
