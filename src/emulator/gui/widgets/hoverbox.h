/**
 * @file stack.h
 * Helper class to emulate the stack in an ARMv7 computer.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_HOVERBOX_H
#define IRISC_HOVERBOX_H

#include "../gui.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <string>

namespace widgets {
  class HoverBox : public Fl_Box {
    private:
      std::string _title;
      std::string _detail;
      Fl_Color _color;
      bool _hovering;

    public:
      HoverBox(int x, int y, int w, int h, std::string title, std::string detail, const char* l = (const char*)0);
      std::string title() const { return _title; };
      std::string detail() const { return _detail; };
      void highlightColor(Fl_Color color) { _color = color; };
      bool hovering() const { return _hovering; };
      int handle(int) override;
      void highlight(Fl_Color color = 0);
  };
}

#endif  // IRISC_HOVERBOX_H