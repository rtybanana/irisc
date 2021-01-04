/**
 * @file repl/constants.h
 * Holds maps which contain definitions for REPL syntax highlighting and code completion.
 * @author Rory Pinkney
 * @date 20/11/20
 */

#ifndef IRISC_GUI_CONSTANTS_H
#define IRISC_GUI_CONSTANTS_H

#include <map>
#include <vector>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include "replxx.hxx"

namespace vm {

  static void close_cb(Fl_Widget *widget, void *) {
    int result = fl_choice("Are you sure you want to exit?", "Yes", "No", 0);
    if (result == 0) Fl::awake(new int(0));
  }

  static std::vector<std::string> directives {
    ".text", ".data", ".global", ".asciz", ".word", ".skip"
  };

  
  static Fl_Color darker = fl_rgb_color(uchar(20));
  static Fl_Color dark = fl_rgb_color(uchar(35));
  static Fl_Color grey = fl_rgb_color(uchar(175));
  static Fl_Color light = fl_rgb_color(uchar(225));
  static Fl_Color red = fl_rgb_color(uchar(255), uchar(85), uchar(85));
  static Fl_Color blue = fl_rgb_color(uchar(85), uchar(85), uchar(255));

  // Style table
  static Fl_Text_Display::Style_Table_Entry styles[] = {
  //  FONT COLOR        FONT FACE     FONT SIZE
  //  ----------------- ------------- --------------
    { FL_WHITE,         FL_COURIER,   20 },   // A - default
    { red,              FL_COURIER,   20 },   // B - Red
    { blue,             FL_COURIER,   20 },   // C - Blue
    { FL_MAGENTA,       FL_COURIER,   20 },   // D - Magenta
    { FL_DARK_YELLOW,   FL_COURIER,   20 },   // E - Yellow
    { FL_DARK_GREEN,    FL_COURIER,   20 },   // F - Green
  };

  // static std::map<std::string, char> styleMap = {
  //   {"mov", 'B'}
  // };

}


#endif //IRISC_GUI_CONSTANTS_H
