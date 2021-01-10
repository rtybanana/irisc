#include "registers.h"
// #include "gui.h"
#include "constants.h"
#include "../constants.h"
#include "../../parser/constants.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

using namespace vm;

static void hover_cb(Fl_Widget* widget, void* group) {
  widgets::HoverBox* hover = (widgets::HoverBox*)widget;
  Registers& registers = *(Registers*)group;                  // casting and dereferencing void pointer
  std::stringstream ss;
  ss << "0x" << std::hex << registers[hover->id()];

  if (hover->hovering()) {
    if (hover->id() == -1) registers.describe(hover->title(), hover->detail());
    else registers.describe(hover->title(), hover->detail() + "\nDecimal value: " + std::to_string(registers[hover->id()]) + "\nHex value: " + ss.str());
  }
  else registers.describe("Registers", "A simplified view of the data currently stored in the CPU. Hover over the different sections to learn what they are.");
}

static void reset_cb(Fl_Widget* widget, void* group) {
  Registers* registers = (Registers*)group;
  registers->reset();
}


Registers::Registers(int x, int y) : Fl_Group(x, y, 240, 540), registers {}, labels {}, label_borders {}, cpsr {}, flags {}, _running(false) {
  for (int i = 0; i < registers.size(); i++) registers[i] = proxy(this, i);

  Fl::lock();
    // window = new Fl_Window(240,540,"Registers");
    Fl_Box* reg_title = new Fl_Box(10, 15, 220, 15);
    reg_title->label("Registers");
    reg_title->labelsize(16);
    reg_title->labelfont(FL_BOLD);
    reg_title->labelcolor(FL_WHITE);
    reg_title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    Fl_Button* ref = new Fl_Button(205, 10, 25, 25, "@+1refresh");
    ref->box(FL_NO_BOX);
    ref->labelcolor(FL_WHITE);
    ref->tooltip("Reset registers");
    ref->callback(reset_cb, this);

    for(auto const& [name, index] : syntax::regMap){
      Fl_Box* reg = new Fl_Box(10, 39+(25*index), 30, 25, name.c_str());
      reg->box(FL_FLAT_BOX);
      reg->color(vm::darker);
      reg->labelcolor(FL_WHITE);
      reg->labelfont(FL_BOLD);
      reg->labelsize(13);

      Fl_Box* val = new Fl_Box(40, 39+(25*index), 190, 25);
      Fl_Box* val_border = new Fl_Box(40, 39+(25*index), 190, 25);
      val->copy_label(regstr(registers[index]).c_str());
      val->box(FL_FLAT_BOX);
      val->color(vm::darker);
      val->labelcolor(FL_WHITE);
      val->labelfont(FL_COURIER);
      val->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
      val->labelsize(13);
      val_border->box(FL_BORDER_FRAME);
      val_border->color(vm::grey);

      labels[index] = val;
      label_borders[index] = val_border;

      // explain on hover
      syntax::REGISTER r = static_cast<syntax::REGISTER>(index);
      widgets::HoverBox* hover = new widgets::HoverBox(10, 39+(25*index), 220, 25, syntax::regTitle[r], syntax::regExplain[r]);
      hover->id(index);
      hover->callback(hover_cb, this);
    }

    Fl_Box* flagbg = new Fl_Box(10, 449, 220, 25);
    flagbg->box(FL_FLAT_BOX);
    flagbg->color(vm::darker);
    
    Fl_Box* flagstitle = new Fl_Box(10, 449, 100, 25, "CPSR");
    flagstitle->box(FL_BORDER_FRAME);
    flagstitle->labelfont(FL_BOLD);
    flagstitle->labelcolor(FL_WHITE);
    flagstitle->labelsize(13);

    widgets::HoverBox* flagshover = new widgets::HoverBox(10, 449, 100, 25, "CPSR Flags", "Four bits in the Current Program Status Register which are used to decide whether an instruction should execute.");
    flagshover->callback(hover_cb, this);

    for (int i = 0; i < 4; i++) {
      FLAG f = static_cast<FLAG>(i);
      Fl_Box* flagname = new Fl_Box(110 + (i * 30), 449, 30, 25, flagShortName[f].c_str());
      flagname->align(FL_ALIGN_LEFT_BOTTOM | FL_ALIGN_INSIDE);
      flagname->labelfont(FL_BOLD);
      flagname->labelcolor(FL_WHITE);
      flagname->labelsize(9);

      Fl_Box* flag = new Fl_Box(110 + (i * 30), 449, 30, 25, "0");
      flag->labelfont(FL_COURIER);
      flag->labelcolor(FL_WHITE);
      flag->labelsize(14);
      flags[i] = flag;

      // explain on hover
      widgets::HoverBox* hover = new widgets::HoverBox(110 + (i * 30), 449, 30, 25, flagTitle[f], flagExplain[f]);
      hover->callback(hover_cb, this);
    }

    Fl_Box* explanation = new Fl_Box(10, 484, 220, 100);
    explanation->box(FL_BORDER_FRAME);
    _title = new Fl_Box(10, 484, 220, 20);
    _title->labelfont(FL_BOLD);
    _title->labelcolor(FL_WHITE);
    _title->labelsize(14);
    _title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    _details = new Fl_Box(10, 509, 220, 75);
    _details->labelcolor(FL_WHITE);
    _details->labelsize(14);
    _details->align(FL_ALIGN_LEFT_BOTTOM | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

    describe("Registers", "A simplified view of the data currently stored in the CPU. Hover over the different sections to learn what they are.");

    end();
    // window->callback(gui::close_cb);
    // window->show();
  Fl::unlock();

  Fl::awake();
};

void Registers::updateReg(int index, uint32_t value) {
  Fl::lock();
    labels[index]->copy_label(regstr(value).c_str());
    // labels[index]->color(FL_YELLOW);
    labels[index]->labelcolor(FL_YELLOW);
    // labels[index]->redraw();
  Fl::unlock();

  Fl::awake();
}

void Registers::prepare() {
  Fl::lock();
    for (int i = 0; i < registers.size(); i++) {
      // labels[i]->color(vm::darker);
      labels[i]->labelcolor(FL_WHITE);
      labels[i]->redraw();
      label_borders[i]->redraw();
    }
  Fl::unlock();

  Fl::awake();
}

void Registers::reset() {
  if (_running) return;

  Fl::lock();
    for (int i = 0; i < registers.size(); i++) {
      registers[i] = proxy(this, i);
      // labels[i]->color(vm::darker);
      labels[i]->labelcolor(FL_WHITE);
      labels[i]->copy_label(regstr(0).c_str());
      // labels[i]->redraw();
      // label_borders[i]->redraw();
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

/** TODO: check the logic for setting the carry bit
 * Sets the CPSR flags based on the result of the executing instruction
 */
void Registers::setFlags(uint32_t op1, uint32_t op2, uint64_t result, char _operator) {
  int sign1 = std::bitset<32>(op1)[31];               // sign of left hand operand
  int sign2 = std::bitset<32>(op2)[31];               // sign of right hand operand
  int signr = std::bitset<32>(result)[31];            // sign of result
  std::bitset<33> result_ext(result);                 // msb = carry bit

  // std::cout << std::bitset<33>(result) << std::endl;

  // std::cout << "\n" << "sign1: " << sign1 << ", sign2: " << sign2 << ", signr: " << signr << ", operator: " << _operator << std::endl;
  // std::cout << result_ext[32] << " " << std::bitset<32>(result) << std::endl;

  cpsr[N] = result_ext[31] == 1;                      // msb = 1
  cpsr[Z] = (uint32_t)result == 0;                    // all bits = 0
  cpsr[C] = result_ext[32];                           // unsigned overflow

  if (_operator == '+') cpsr[V] = sign1 == sign2 && sign1 != signr;         // two operands of the same sign result in changed sign
  else if (_operator == '-') cpsr[V] = sign1 != sign2 && sign2 == signr;    // signs different and result sign same as subtrahend
  
  // std::cout << "  negative flag: " << cpsr[N] << "\n"
  //           << "  zero flag: " << cpsr[Z] << "\n"
  //           << "  carry flag: " << cpsr[C] << "\n"
  //           << "  overflow flag: " << cpsr[V] << std::endl;

  Fl::lock();
    for (int flag : {N, Z, C, V}) {
      // std::cout << std::to_string(cpsr[flag]).c_str() << std::endl;
      flags[flag]->copy_label(std::to_string(cpsr[flag]).c_str());
      // flags[flag]->redraw();
    }
  Fl::unlock();

  Fl::awake();                                     
}

/** TODO: check that each of these works as expected
 * Checks the CPSR flags against the current condition code to determine if the instruction should be executed
 */
bool Registers::checkFlags(syntax::CONDITION cond) {
  std::bitset<4> bits(cond);

  bool result;
  switch(cond) {
    case syntax::EQ: case syntax::NE:                       // equality
      result = cpsr[Z]; break;
    case syntax::CS: case syntax::CC:
      result = cpsr[C]; break;
    case syntax::MI: case syntax::PL:
      result = cpsr[N]; break;
    case syntax::VS: case syntax::VC:
      result = cpsr[V]; break;
    case syntax::HI: case syntax::LS:
      result = cpsr[C] && !cpsr[Z]; break;
    case syntax::GE: case syntax::LT:
      result = cpsr[N] == cpsr[V]; break;
    case syntax::GT: case syntax::LE:
      result = (cpsr[N] == cpsr[V]) && !cpsr[Z]; break;
    default:
      return true;                                          // AL flag returns true regardless
  }

  if (bits[0] == 1) result = !result;
  return result;                                        
}

void Registers::describe(std::string title, std::string details) {
  Fl::lock();
    _title->copy_label(title.c_str());
    // _title->redraw_label();
    _details->copy_label(details.c_str());
    // _details->redraw_label();
  Fl::unlock();

  Fl::awake();
}

