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

namespace ui {
  static std::vector<std::string> directives {
    ".text", ".data", ".global", ".asciiz", ".word", ".skip"
  };

  using cl = replxx::Replxx::Color;
  static std::vector<std::pair<std::string, cl>> regex_color {
    {"#((0x([0-9]|[a-f])+)|(0b([0-1]+))|((0d)?[0-9]+))", cl::GRAY},
    {"\\.[A-Za-z]+", cl::BRIGHTMAGENTA}
  };

  // Style table
  static Fl_Text_Display::Style_Table_Entry styles[] = {
    // FONT COLOR      FONT FACE   FONT SIZE
    // --------------- ----------- --------------
    { FL_WHITE,         FL_COURIER, 15 }, // A - default
    { FL_RED,           FL_COURIER, 15 }, // B - Red
    { FL_BLUE,          FL_COURIER, 15 }, // C - Blue
    { FL_MAGENTA,       FL_COURIER, 15 }, // D - Magenta
    { FL_DARK_YELLOW,   FL_COURIER, 15 }, // E - Yellow
    { FL_DARK_GREEN,    FL_COURIER, 15 }, // F - Green
  };

  // static std::map<std::string, char> styleMap = {
  //   {"mov", 'B'}
  // };
}


#endif //IRISC_REPL_CONSTANTS_H
