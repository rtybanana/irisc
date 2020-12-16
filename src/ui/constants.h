/**
 * @file repl/constants.h
 * Holds maps which contain definitions for REPL syntax highlighting and code completion.
 * @author Rory Pinkney
 * @date 20/11/20
 */

#ifndef IRISC_UI_CONSTANTS_H
#define IRISC_UI_CONSTANTS_H

#include <map>
#include <vector>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl.H>
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
  
  static Fl_Color dark = fl_rgb_color(uchar(35));
  static Fl_Color grey = fl_rgb_color(uchar(175));
  static Fl_Color light = fl_rgb_color(uchar(225));
  static Fl_Color red = fl_rgb_color(uchar(255), uchar(85), uchar(85));
  static Fl_Color blue = fl_rgb_color(uchar(85), uchar(85), uchar(255));

  // Style table
  static Fl_Text_Display::Style_Table_Entry styles[] = {
    // FONT COLOR      FONT FACE   FONT SIZE
    // --------------- ----------- --------------
    { FL_WHITE,       FL_COURIER, 15 }, // A - default
    { red,            FL_COURIER, 15 }, // B - Red
    { blue,           FL_COURIER, 15 }, // C - Blue
    { FL_MAGENTA,     FL_COURIER, 15 }, // D - Magenta
    { FL_DARK_YELLOW, FL_COURIER, 15 }, // E - Yellow
    { FL_DARK_GREEN,  FL_COURIER, 15 }, // F - Green
  };

  // static std::map<std::string, char> styleMap = {
  //   {"mov", 'B'}
  // };

}


#endif //IRISC_UI_CONSTANTS_H
