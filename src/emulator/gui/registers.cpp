#include "registers.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>

using namespace vm;

Registers::Registers() : _registers {} {
  for (int i = 0; i < _registers.size(); i++) _registers[i] = proxy(this, i);

  Fl::lock();
    window = new Fl_Window(220,500,"Registers");
    for(auto const& [name, index] : syntax::regMap){
      Fl_Box* reg = new Fl_Box(10, 10+(30*index), 30, 30, name.c_str());
      reg->box(FL_UP_BOX);
      reg->labelfont(FL_BOLD);
      reg->labelsize(12);

      Fl_Box* val = new Fl_Box(40, 10+(30*index), 170, 30);
      val->copy_label(regstr(_registers[index]).c_str());
      val->box(FL_UP_BOX);
      val->labelfont(FL_COURIER);
      val->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
      val->labelsize(12);

      labels[index] = val;
    }

    window->end();
    window->callback(gui::close_cb);
    window->show();
  Fl::unlock();

  Fl::awake();
};

void Registers::updateReg(int index, uint32_t value) {
  Fl::lock();
    for (int i = 0; i < labels.size(); i++) {
      labels[i]->color(FL_BACKGROUND_COLOR);
      labels[i]->redraw();
    }
    
    labels[index]->copy_label(regstr(value).c_str());
    labels[index]->color(FL_YELLOW);
  Fl::unlock();

  Fl::awake();
}

void Registers::reset() {
  Fl::lock();
    for (int i = 0; i < _registers.size(); i++) {
      _registers[i] = proxy(this, i);
      labels[i]->color(FL_BACKGROUND_COLOR);
      labels[i]->copy_label(regstr(0).c_str());
      labels[i]->redraw();
    }
  Fl::unlock();

  Fl::awake();
}

std::string Registers::regstr(u_int32_t value) {
  std::stringstream ss;
  ss << "0x" 
      << std::setfill ('0') << std::setw(sizeof(uint32_t)*2) 
      << std::hex << value << " (" << std::dec << value << ")";

  return ss.str();
}