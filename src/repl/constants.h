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
   // words to be completed
  static std::vector<std::string> examples {
    ".help", ".history", ".quit", ".exit", ".clear", ".prompt ",
    "hello", "world", "db", "data", "drive", "print", "put",
    "color_black", "color_red", "color_green", "color_brown", "color_blue",
    "color_magenta", "color_cyan", "color_lightgray", "color_gray",
    "color_brightred", "color_brightgreen", "color_yellow", "color_brightblue",
    "color_brightmagenta", "color_brightcyan", "color_white", "color_normal",
  };

  using cl = replxx::Replxx::Color;
  static std::vector<std::pair<std::string, cl>> regex_color {
    // // single chars
    // {"\\`", cl::BRIGHTCYAN},
    // {"\\'", cl::BRIGHTBLUE},
    // {"\\\"", cl::BRIGHTBLUE},
    // {"\\-", cl::BRIGHTBLUE},
    // {"\\+", cl::BRIGHTBLUE},
    // {"\\=", cl::BRIGHTBLUE},
    // {"\\/", cl::BRIGHTBLUE},
    // {"\\*", cl::BRIGHTBLUE},
    // {"\\^", cl::BRIGHTBLUE},
    // {"\\.", cl::BRIGHTMAGENTA},
    // {"\\(", cl::BRIGHTMAGENTA},
    // {"\\)", cl::BRIGHTMAGENTA},
    // {"\\[", cl::BRIGHTMAGENTA},
    // {"\\]", cl::BRIGHTMAGENTA},
    // {"\\{", cl::BRIGHTMAGENTA},
    // {"\\}", cl::BRIGHTMAGENTA},

    // // color keywords
    // {"color_black", cl::BLACK},
    // {"color_red", cl::RED},
    // {"color_green", cl::GREEN},
    // {"color_brown", cl::BROWN},
    // {"color_blue", cl::BLUE},
    // {"color_magenta", cl::MAGENTA},
    // {"color_cyan", cl::CYAN},
    // {"color_lightgray", cl::LIGHTGRAY},
    // {"color_gray", cl::GRAY},
    // {"color_brightred", cl::BRIGHTRED},
    // {"color_brightgreen", cl::BRIGHTGREEN},
    // {"color_yellow", cl::YELLOW},
    // {"color_brightblue", cl::BRIGHTBLUE},
    {"color_brightmagenta", cl::BRIGHTMAGENTA},
    // {"color_brightcyan", cl::BRIGHTCYAN},
    // {"color_white", cl::WHITE},
    {"color_normal", cl::NORMAL},

    // // commands
    // {"\\.help", cl::BRIGHTMAGENTA},
    // {"\\.history", cl::BRIGHTMAGENTA},
    // {"\\.quit", cl::BRIGHTMAGENTA},
    // {"\\.exit", cl::BRIGHTMAGENTA},
    // {"\\.clear", cl::BRIGHTMAGENTA},
    // {"\\.prompt", cl::BRIGHTMAGENTA},

    // numbers
    // {"[\\-|+]{0,1}[0-9]+", cl::YELLOW}, // integers
    // {"[\\-|+]{0,1}[0-9]*\\.[0-9]+", cl::YELLOW}, // decimals
    // {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+", cl::YELLOW}, // scientific notation

    {"#((0x([0-9]|[a-f])+)|(0b([0-1]+))|((0d)?[0-9]+))", cl::BRIGHTCYAN}

    // // strings
    // {"\".*?\"", cl::BRIGHTGREEN}, // double quotes
    // {"\'.*?\'", cl::BRIGHTGREEN}, // single quotes
  };
}


#endif //IRISC_REPL_CONSTANTS_H
