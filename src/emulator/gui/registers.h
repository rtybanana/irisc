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

  //******************************************************************************************
  // CPSR FLAGS
  enum FLAG {
    N,  // negative
    Z,  // zero
    C,  // carry
    V   // overflow
  };

  static std::map<FLAG, std::string> flagShortName {
    {N, "N"}, {Z, "Z"}, {C, "C"}, {V, "V"}
  };

  static std::map<FLAG, std::string> flagTitle {
    {N, "Negative Flag (N)"}, {Z, "Zero Flag (Z)"}, {C, "Carry Flag (C)"}, {V, "Overflow Flag (V)"}
  };

  static std::map<FLAG, std::string> flagExplain {
    {N, "This bit is set when the signed result of the operation is negative."}, 
    {Z, "This bit is set when the result of the operation is equal to zero."}, 
    {C, "This bit is set when the operation results in an unsigned overflow."}, 
    {V, "This bit is set when the operation results in a signed overflow."}
  };

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

    public:
      Registers(int, int);
      // void linkEmulator(Emulator* emulator) { this->emulator = emulator; };
      void running(bool running) { _running = running; };
      void update();
      void updateReg(int, uint32_t);
      void prepare();
      void reset();
      void setFlags(uint32_t, uint32_t, uint64_t, char _operator = ' ');
      bool checkFlags(syntax::CONDITION);
      void describe(std::string, std::string);
      proxy& operator[] (int index) { return registers[index]; };
  };

}

#endif //IRISC_REGISTERS_H
