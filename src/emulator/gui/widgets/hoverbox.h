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
      bool _hovering;

    public:
      HoverBox(int x, int y, int w, int h, std::string title, std::string detail, const char* l = (const char*)0) : 
        Fl_Box(x, y, w, h, l),
        _title(title),
        _detail(detail),
        _hovering(false) {}
      std::string title() const { return _title; };
      std::string detail() const { return _detail; };
      bool hovering() const { return _hovering; };
      int handle(int) override;
      void highlight(Fl_Color color = 0);
  };
}

#endif  // IRISC_HOVERBOX_H