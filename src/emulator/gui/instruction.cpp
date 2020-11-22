#include "instruction.h"
#include "gui.h"
#include <FL/Fl.H>
#include <sstream>
#include <iostream>
#include <bitset>

using namespace vm;

static void hover_cb(Fl_Widget* widget, void* window) {
  widgets::HoverBox* hover = (widgets::HoverBox*)widget;
  Instruction* instruction = (Instruction*)window;
  if (hover->hovering())
    instruction->describe(hover->title(), hover->detail());
  else
    instruction->describe("Assembled Instruction", "This is the assembled machine code for last instruction, hover over the different sections to see what they mean.");
}

Instruction::Instruction() {
  Fl::lock();
    window = new Fl_Window(500, 160, "Machine Code");

    // Fl_Box* sharedStatus = new Fl_Box(10, 10, 480, 26, "Status: ");
    // sharedStatus->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    line = new Fl_Box(10, 10, 480, 26, "No Instruction");
    line->labelfont(FL_BOLD);
    line->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    status = new Fl_Box(10, 10, 480, 26, "N/A");
    status->labelfont(FL_BOLD);
    status->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

    Fl_Box* boundary = new Fl_Box(10, 41, 480, 26);
    boundary->box(FL_UP_BOX);

    for(int i = 0; i < 32; i++){
      Fl_Box* bit = new Fl_Box(10+(15*i), 41, 15, 26, "0");
      bit->labelfont(FL_COURIER);
      bit->labelsize(15);

      bits[i] = bit;
    }

    Fl_Box* explanation = new Fl_Box(10, 72, 480, 78);
    explanation->box(FL_UP_BOX);
    _title = new Fl_Box(10, 72, 480, 26);
    _title->labelfont(FL_BOLD);
    _title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    _details = new Fl_Box(10, 98, 480, 52);
    _details->align(FL_ALIGN_LEFT_BOTTOM | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

    describe("No Instruction", "Execute an instruction to examine its machine code.");

    window->end();
    window->callback(gui::close_cb);
    window->show();
  Fl::unlock();

  Fl::awake();
}

void Instruction::set(syntax::InstructionNode* instruction, bool executed) {
  auto [assembled, explanation] = instruction->assemble();
  std::bitset<32> machinecode(assembled);

  Fl::lock();
    line->copy_label(instruction->toString().c_str());
    if (executed) { 
      status->label("Executed"); 
      status->labelcolor(FL_DARK_GREEN); }
    else { 
      status->label("Not Executed"); 
      status->labelcolor(FL_DARK_RED); 
    }

    for (widgets::HoverBox* region : regions) {
      window->remove(region);
      delete region;
    }
    regions.clear();

    for (int i = 0; i < bits.size(); i++) bits[i]->copy_label(std::to_string(machinecode[31 - i]).c_str()); 

    window->begin();
    int offset = 0;
    for (auto [title, detail, range] : explanation) {
      widgets::HoverBox* region = new widgets::HoverBox(10+(15*offset), 41, 15*range, 26, title, detail);
      // region->box(FL_UP_FRAME);
      // region->color(FL_BLACK);
      region->callback(hover_cb, this);
      regions.push_back(region);
      offset += range;
    }

    describe("Assembled Instruction", "This is the assembled machine code for last instruction, hover over the different sections to see what they mean.");
    window->end();
  Fl::unlock();

  Fl::awake();
}

void Instruction::describe(std::string title, std::string details) {
  Fl::lock();
    _title->copy_label(title.c_str());
    _title->redraw_label();
    _details->copy_label(details.c_str());
    _details->redraw_label();
  Fl::unlock();

  Fl::awake();
}