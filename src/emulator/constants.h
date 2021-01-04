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
  // MODES
  enum MODE {
    TEXT, 
    DATA
  };
}

#endif // IRISC_EMULATOR_CONSTANTS_H