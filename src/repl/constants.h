/**
 * @file repl/constants.h
 * Holds maps which contain definitions for REPL syntax highlighting and code completion.
 * @author Rory Pinkney
 * @date 20/11/20
 */

#ifndef IRISC_REPL_CONSTANTS_H
#define IRISC_REPL_CONSTANTS_H

#include <map>
#include <vector>
#include "replxx.hxx"

namespace irepl {
  static std::vector<std::string> directives {
    ".text", ".data", ".asciiz", ".word", ".skip"
  };

  using cl = replxx::Replxx::Color;
  static std::vector<std::pair<std::string, cl>> regex_color {
    {"#((0x([0-9]|[a-f])+)|(0b([0-1]+))|((0d)?[0-9]+))", cl::GRAY},
    {"\\.[A-Za-z]+", cl::BRIGHTMAGENTA}
  };
}


#endif //IRISC_REPL_CONSTANTS_H
