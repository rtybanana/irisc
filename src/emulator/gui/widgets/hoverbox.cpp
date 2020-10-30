#include "hoverbox.h"
#include <FL/Fl.H>

using namespace widgets;

int HoverBox::handle(int event) {
  switch(event) {
    case FL_ENTER:
      _hovering = true;
      highlight(FL_MAGENTA);
      do_callback();
      return 1;
    case FL_LEAVE:
      _hovering = false;
      highlight();
      do_callback();
      return 1;
    default:
      return Fl_Widget::handle(event);
  }
}

void HoverBox::highlight(Fl_Color color) {
  Fl::lock();
    if (color != 0) {
      this->box(FL_BORDER_FRAME);
      this->color(color);
    }
    else this->box(FL_UP_FRAME);
    this->redraw();
  Fl::unlock();

  Fl::awake();
}