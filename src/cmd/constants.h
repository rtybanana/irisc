/**
 * @file repl/constants.h
 * Holds maps which contain definitions for REPL syntax highlighting and code completion.
 * @author Rory Pinkney
 * @date 20/11/20
 */

#ifndef IRISC_CMD_CONSTANTS_H
#define IRISC_CMD_CONSTANTS_H

#include <map>
#include <vector>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl.H>
#include "replxx.hxx"

namespace ui {
  static std::vector<std::string> directives {
    ".text", ".data", ".global", ".asciz", ".word", ".skip"
  };

  using cl = replxx::Replxx::Color;
  static std::vector<std::pair<std::string, cl>> regex_color {
    {"#((0x([0-9]|[a-f])+)|(0b([0-1]+))|((0d)?[0-9]+))", cl::GRAY},
    {"\\.[A-Za-z]+", cl::BRIGHTMAGENTA}
  };

}


#endif //IRISC_CMD_CONSTANTS_H
