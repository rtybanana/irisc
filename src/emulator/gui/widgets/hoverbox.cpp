#include "hoverbox.h"
#include <FL/Fl.H>

using namespace widgets;

int HoverBox::handle(int event) {
  switch(event) {
    case FL_ENTER:
      _hovering = true;
      do_callback();
      return 1;
    case FL_LEAVE:
      _hovering = false;
      do_callback();
      return 1;
    default:
      return Fl_Widget::handle(event);
  }
}

// void HoverBox::highlight(Fl_Color color) {

// }