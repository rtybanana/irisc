/**
 * @file registers.h
 * Emulates the registers in an ARMv7 processor. Responsible for managing the getting and setting of 
 * register values and displaying the related GUI window.
 * @author Rory Pinkney
 * @date 20/10/20
 */

#ifndef IRISC_REGISTERS_H
#define IRISC_REGISTERS_H

#include <sstream>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
// #include "../emulator.h"
#include "../../parser/syntax.h"
#include "../../widgets/hoverbox.h"

namespace vm {

  class Registers : public Fl_Group {
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
          proxy& operator+=(const uint32_t _value) {
            this->value += _value;
            container->updateReg(index, value);
            return *this;
          }
          bool operator==(const uint32_t _value) const {
            return this->value == _value;
          }
          operator int() { return value; };
          friend std::ostream& operator<<(std::ostream& os, const proxy& p) { os << p.value; return os; };
      };

      // Emulator* emulator;
      std::array<proxy, 16> registers;
      bool cpsr[4];
      std::array<Fl_Box*, 16> labels;
      std::array<Fl_Box*, 16> label_borders;
      std::array<Fl_Box*, 4> flags;
      std::string regstr(uint32_t);
      Fl_Box* _title;
      Fl_Box* _details;
      bool _running;
      bool headless;

    public:
      Registers(int, int, bool = false);
      // void linkEmulator(Emulator* emulator) { this->emulator = emulator; };
      void running(bool running) { _running = running; };
      void update();
      void updateReg(int, uint32_t);
      void prepare();
      void reset();
      void setFlags(uint32_t, uint32_t, uint64_t, char _operator = ' ');
      bool checkFlags(syntax::CONDITION);
      void describe(std::string, std::string);
      void print();
      void printIndex(int);
      void printCPSR();
      proxy& operator[] (int index) { return registers[index]; };
      bool operator[] (syntax::FLAG flag) { return cpsr[flag]; };
  };

}

#endif //IRISC_REGISTERS_H
