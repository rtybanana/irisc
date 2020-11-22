/**
 * @file emulator/constants.h
 * Holds maps which are useful as lookup tables to convert variables into human readable explanations.
 * @author Rory Pinkney
 * @date 20/11/20
 */

#ifndef IRISC_EMULATOR_CONSTANTS_H
#define IRISC_EMULATOR_CONSTANTS_H

#include <map>

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
    {N, "Negative Flag"}, {Z, "Zero Flag"}, {C, "Carry Flag"}, {V, "Overflow Flag"}
  };

  static std::map<FLAG, std::string> flagExplain {
    {N, "This bit is set when the signed result of the operation is negative."}, 
    {Z, "This bit is set when the result of the operation is equal to zero."}, 
    {C, "This bit is set when the operation results in an unsigned overflow."}, 
    {V, "This bit is set when the operation results in a signed overflow."}
  };
}

#endif // IRISC_EMULATOR_CONSTANTS_H