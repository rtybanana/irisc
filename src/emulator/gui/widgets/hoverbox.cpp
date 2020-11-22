#include "hoverbox.h"
#include <FL/Fl.H>

using namespace widgets;

HoverBox::HoverBox(int x, int y, int w, int h, std::string title, std::string detail, const char* l) : 
  Fl_Box(x, y, w, h, l),
  _title(title),
  _detail(detail),
  _color(FL_MAGENTA),
  _hovering(false) {
    this->box(FL_UP_FRAME);
  }

int HoverBox::handle(int event) {
  switch(event) {
    case FL_ENTER:
      _hovering = true;
      highlight(_color);
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
    this->redraw_label();
  Fl::unlock();

  Fl::awake();
}