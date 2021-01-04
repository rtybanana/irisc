#include "instruction.h"
#include "constants.h"
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

Instruction::Instruction(int x, int y) : Fl_Group(x, y, 500, 160) {
  Fl::lock();
    // window = new Fl_Window(500, 160, "Machine Code");

    // Fl_Box* sharedStatus = new Fl_Box(10, 10, 480, 26, "Status: ");
    // sharedStatus->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    line = new Fl_Box(10, 10, 480, 26, "No Instruction");
    line->labelfont(FL_BOLD);
    line->labelsize(16);
    line->labelcolor(FL_WHITE);
    line->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    status = new Fl_Box(10, 10, 480, 26, "N/A");
    status->labelfont(FL_BOLD);
    status->labelsize(16);
    status->labelcolor(FL_WHITE);
    status->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

    Fl_Box* binary = new Fl_Box(10, 40, 480, 25);
    Fl_Box* boundary = new Fl_Box(10, 40, 480, 25);
    binary->box(FL_FLAT_BOX);
    binary->color(vm::darker);
    boundary->box(FL_BORDER_FRAME);
    boundary->color(vm::grey);

    for(int i = 0; i < 32; i++){
      Fl_Box* bit = new Fl_Box(10+(15*i), 40, 15, 25, "0");
      bit->labelfont(FL_COURIER);
      bit->labelcolor(FL_WHITE);
      bit->labelsize(16);

      bits[i] = bit;
    }

    Fl_Box* explanation = new Fl_Box(10, 75, 480, 100);
    explanation->box(FL_BORDER_FRAME);
    _title = new Fl_Box(10, 75, 480, 20);
    _title->labelfont(FL_BOLD);
    _title->labelcolor(FL_WHITE);
    _title->labelsize(15);
    _title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    _details = new Fl_Box(10, 100, 480, 75);
    _details->labelcolor(FL_WHITE);
    _details->labelsize(15);
    _details->align(FL_ALIGN_LEFT_BOTTOM | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

    describe("No Instruction", "Execute an instruction to examine its machine code.");

    end();
    // window->callback(gui::close_cb);
    // window->show();
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
      remove(region);
      delete region;
    }
    regions.clear();

    for (int i = 0; i < bits.size(); i++) bits[i]->copy_label(std::to_string(machinecode[31 - i]).c_str()); 

    begin();
    int offset = 0;
    for (auto [title, detail, range] : explanation) {
      widgets::HoverBox* region = new widgets::HoverBox(x() + 10+(15*offset), y() + 40, 15*range, 25, title, detail);
      region->callback(hover_cb, this);

      regions.push_back(region);
      offset += range;
    }
    end();

    // redraw();
    // draw_children();

    describe("Assembled Instruction", "This is the assembled machine code for last instruction, hover over the different sections to see what they mean.");
    // redraw();
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