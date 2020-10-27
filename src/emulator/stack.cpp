#include "stack.h"
#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>

using namespace vm;

Stack::Stack() : _heap() {
  // createWindow();
};

void Stack::createWindow() {
  window = new Fl_Window(340,180,"Stack");
  Fl_Box *box = new Fl_Box(20,40,300,100,"Stack!");

  box->box(FL_UP_BOX);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);

  window->set_non_modal();
  window->end();
  window->show();

  std::cout << "stack wake main thread" << std::endl;

  Fl::awake();
}