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
  using cl = replxx::Replxx::Color;
  static std::vector<std::pair<std::string, cl>> regex_color {
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
    // {"color_brightmagenta", cl::BRIGHTMAGENTA},
    // {"color_brightcyan", cl::BRIGHTCYAN},
    // {"color_white", cl::WHITE},
    // {"color_normal", cl::NORMAL},

    {"#((0x([0-9]|[a-f])+)|(0b([0-1]+))|((0d)?[0-9]+))", cl::GRAY}
  };
}


#endif //IRISC_REPL_CONSTANTS_H
