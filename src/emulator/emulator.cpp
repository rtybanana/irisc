#include <iostream>
#include <bit>
#include <thread>
#include <chrono>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include "emulator.h"
#include "gui/constants.h"
#include "../parser/parser.h"
#include "../error.h"

using namespace vm;

Emulator::Emulator() : memory(), registers(0, 0), instruction(0, 0), _running(false), delay(1) {
  editor = new Editor(0, 0, this);

  Fl::lock();
    window = new Fl_Window(1140, 600, "iRISC");
    window->color(vm::dark);

    Fl_Box* reg_title = new Fl_Box(15, 15, 220, 15);
    reg_title->label("Registers");
    reg_title->labelsize(16);
    reg_title->labelfont(FL_BOLD);
    reg_title->labelcolor(FL_WHITE);
    reg_title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    window->add(&registers);
    registers.position(5, 29);

    window->add(&instruction);
    instruction.position(245, 409);

    Fl_Box* edt_title = new Fl_Box(255, 15, 220, 15);
    edt_title->label("Editor");
    edt_title->labelsize(16);
    edt_title->labelfont(FL_BOLD);
    edt_title->labelcolor(FL_WHITE);
    edt_title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    window->add(editor);
    editor->position(245, 0);
    
    window->end();
    window->callback(vm::close_cb);
    window->show();

    // registers_w = new Fl_Window(240,540,"Registers");
    // registers_w->add(&registers);
    // registers_w->end();
    // registers_w->show();

    // instruction_w = new Fl_Window(500,160,"Machine Code");
    // instruction_w->add(&instruction);
    // instruction_w->end();
    // instruction_w->show();

    // editor_w = new Fl_Window(280, 350, "Editor");
    // editor_w->color(fl_rgb_color(uchar(35)));
    // editor_w->add(editor);
    // editor_w->end();
    // editor_w->show();
  Fl::unlock();

  Fl::awake();
};


void Emulator::reset() {
  registers.clear();
  // stack.reset();
}

void Emulator::execute(std::string statement) {
  try {
    lexer::Lexer lexer(statement);
    parser::Parser parser(lexer);
    syntax::Node* node = parser.parseSingle();

    if (node != nullptr) execute(node);
    free(node);
  }
  // Catch exception and print error
  catch(const Error &e) {
    std::cerr << e.what() << std::endl;
  }
}

/**
 * Driver function to execute any instruction with a base class of InstructionNode
 */
void Emulator::execute(syntax::Node* node) {
  registers.prepare();

  if (dynamic_cast<syntax::InstructionNode*>(node)) {
    bool executed = false;
    if (dynamic_cast<syntax::BiOperandNode*>(node)) {
      executed = executeBiOperand(dynamic_cast<syntax::BiOperandNode*>(node));
    }
    else if (dynamic_cast<syntax::TriOperandNode*>(node)) {
      executed = executeTriOperand(dynamic_cast<syntax::TriOperandNode*>(node));
    }
    else if (dynamic_cast<syntax::ShiftNode*>(node)) {
      executed = executeShift(dynamic_cast<syntax::ShiftNode*>(node));
    }

    instruction.set(dynamic_cast<syntax::InstructionNode*>(node), executed);
  }
  
  else if (dynamic_cast<syntax::AllocationNode*>(node)) {

  }
  else if (dynamic_cast<syntax::LabelNode*>(node)) {
    throw InteractiveError("Label instructions are not executable on their own. Try using the editor (:editor) to execute multiple lines.", node->statement(), 0, node->lineNumber());
  }
}

/**
 * Parses but does not run a string containing a series of statements
 */
void Emulator::compile(std::string program) {
  try {
    lexer::Lexer lexer(program);
    parser::Parser parser(lexer);
    std::vector<syntax::Node*> nodes = parser.parseMultiple();

    start(nodes, false);
  }
  // Catch exception and print error
  catch(const Error &e) {
    std::cerr << e.what() << std::endl;
  }
}

/**
 * Parses and runs a string containing a series of statements
 */
void Emulator::run(std::string program) {
  try {
    lexer::Lexer lexer(program);
    parser::Parser parser(lexer);
    std::vector<syntax::Node*> nodes = parser.parseMultiple();

    start(nodes);
  }
  // Catch exception and print error
  catch(const Error &e) {
    std::cerr << e.what() << std::endl;
  }
}

/**
 * Prepares the program nodes
 */
void Emulator::start(std::vector<syntax::Node*> nodes, bool run) {
  if (_running) return;

  memory.softReset();
  registers[syntax::PC] = memory.memstart();

  bool text = true;
  // bool entry_point = false;
  std::vector<int> remove;
  std::vector<syntax::ErrorNode> errors; 
  for (int i = 0; i < nodes.size(); i++) {
    if (dynamic_cast<syntax::ErrorNode*>(nodes[i])) {
      errors.push_back(*dynamic_cast<syntax::ErrorNode*>(nodes[i]));
      remove.push_back(i);
      continue;
    }

    if (dynamic_cast<syntax::DirectiveNode*>(nodes[i])) {
      syntax::DirectiveNode* node = dynamic_cast<syntax::DirectiveNode*>(nodes[i]);
      if (node->isData()) text = false;
      else if (node->isText()) { text = true; remove.push_back(i); }
      // else if (node->isGlobal()) { entry_point = true; remove.push_back(i); }
    }
    else if (dynamic_cast<syntax::AllocationNode*>(nodes[i])) {
      if (text) throw AssemblyError("Cannot declare data outside of the data section.", nodes[i]->statement(), nodes[i]->lineNumber(), -1);
      else {
        memory.allocate(dynamic_cast<syntax::AllocationNode*>(nodes[i]));
      }
    }
    else if (dynamic_cast<syntax::LabelNode*>(nodes[i])) {
      if (!text) throw AssemblyError("Cannot declare branchable labels outside of the text section.", nodes[i]->statement(), nodes[i]->lineNumber(), -1);
      syntax::LabelNode* node = dynamic_cast<syntax::LabelNode*>(nodes[i]);
      
      if (memory.hasLabel(node->identifier())) {
        throw AssemblyError("Cannot declare multiple labels with the same name: '" + node->identifier() + "'.", node->statement(), node->lineNumber(), 0);
      }
      else {
        if (node->identifier() == "main") registers[syntax::PC] = memory.memstart() + ((i - remove.size()) * 32);
        memory.addLabel(node->identifier(), i - remove.size());
        remove.push_back(i);
      }
    }

    if (!text) remove.push_back(i);
  }

  if (!errors.empty()) {
    editor->wavyLine(errors);
    for (syntax::Node* node : nodes) free(node);
    return;
  }

  std::reverse(remove.begin(), remove.end());
  for (int i : remove) {
    free(nodes[i]);
    nodes.erase(nodes.begin() + i);
  }

  std::vector<syntax::InstructionNode*> instructions;
  for (syntax::Node* node : nodes) instructions.push_back(dynamic_cast<syntax::InstructionNode*>(node));
  memory.setText(instructions);

  if (run) std::thread([this]{ this->run(); }).detach();
}

/**
 * 
 */ 
void Emulator::run() {
  _running = true;

  while (running()) {
    syntax::InstructionNode* node = memory.instruction(registers[syntax::PC]);
    std::cout << "PC: " << registers[syntax::PC] << ": " << node->toString() << std::endl;
    editor->highlightLine(node->statement()[0].lineNumber());

    
    if (dynamic_cast<syntax::BranchNode*>(node)) {
      registers.prepare();
      bool executed = executeBranch(dynamic_cast<syntax::BranchNode*>(node));
      if (!executed) registers[syntax::PC] += 32;                                    // increment to the next instruction
    }
    else {
      registers[syntax::PC] += 32;                                    // increment to the next instruction
      execute(node);
    }
      
    int sleepfor = 50;
    int sleptfor = 0;
    while (sleptfor < delay*1000 && _running) {      // check every 50ms to see if speed value has changed
      // sleep in 50ms chunks (smallest speed denomination)   
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepfor));
      sleptfor += sleepfor;
    }
  }

  _running = false;
  editor->highlightLine(-1);          // unhighlight all lines
  registers.prepare();
}

bool Emulator::running() {
  return _running && registers[syntax::PC] < (memory.memstart() + (memory.text().size() * 32));
}

void Emulator::stop() {
  _running = false;
}

void Emulator::speed(double delay) {
  this->delay = delay;
}

/**
 * Simplifies the flex operand down to a single source value, fetching register values and applying shifts.
 */
uint32_t Emulator::deflex(syntax::FlexOperand flex) {
  int deflex;

  auto [Rm, shift, Rs, immShift] = flex.unpack();
  if (flex.isImm()) return std::rotr((uint32_t)std::get<int>(Rm), immShift);      // return value of immediate (short circuit)
  else deflex = registers[std::get<syntax::REGISTER>(Rm)];                        // get value in register

  if (flex.shifted()) {
    int shiftBy;
    if (flex.shiftedByImm()) shiftBy = std::get<int>(flex.Rs());                  // get value of immediate shift
    else shiftBy = registers[std::get<syntax::REGISTER>(flex.Rs())];              // get value in shift register

    deflex = applyFlexShift(flex.shift(), deflex, shiftBy);                           // apply the unpacked shift
  }
  
  return deflex;
}

/**
 * Applies a single shift operation.
 */
uint32_t Emulator::applyFlexShift(syntax::SHIFT shift, int value, int amount) {
  switch(shift) {
    case syntax::LSL:
      return value <<= amount;
    case syntax::LSR:
      if (amount == 0) amount = 32;       // special case for right shifts
      return value >>= amount;
    default:
      throw std::runtime_error("");
  }
}

/**
 * Executes any bi-operand instruction such as MOV, MVN and comparisions CMP, TST etc
 */
bool Emulator::executeBiOperand(syntax::BiOperandNode* instruction) {
  auto [op, cond, set, dest, flex] = instruction->unpack();           // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                            // returns early if condition code is not satisfied

  uint32_t src = deflex(flex);                                        // deflex the flex operand into a single value
  switch(op) {                                                        // check opcode and execute instruction
    case syntax::MOV:
      if (set) registers.setFlags(registers[dest], src, src);
      registers[dest] = src;
      break;
    case syntax::MVN:
      if (set) registers.setFlags(registers[dest], -src, -src);
      registers[dest] = -src;
      break;
    case syntax::CMP:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] - src, '-');
      break;
    case syntax::CMN:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] + src, '+');
      break;
    case syntax::TST:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] & src);
      break;
    case syntax::TEQ:
      registers.setFlags(registers[dest], src, (uint64_t)registers[dest] ^ src);
      break;
  } 

  return true;
}

/**
 * TODO: implement arithmetic with carry (ADC SBC and RSC)
 * Executes any tri-operand arithmetic opereration (besides shifts)
 */
bool Emulator::executeTriOperand(syntax::TriOperandNode* instruction) {
  auto [op, cond, set, dest, src, flex] = instruction->unpack();    // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                          // returns early if condition code is not satisfied

  uint32_t n = registers[src];
  uint32_t m = deflex(flex);                                        // deflex the flex operand into a value
  int result;
  switch (op) {                                                      // check opcode and execute instruction
    case syntax::AND:
      if (set) registers.setFlags(n, m, (uint64_t)n & m);
      result = n & m;
      break;
    case syntax::EOR:
      if (set) registers.setFlags(n, m, (uint64_t)n ^ m);
      result = n ^ m;
      break;
    case syntax::ORR:
      if (set) registers.setFlags(n, m, (uint64_t)n | m);
      result = n | m;
      break;
    case syntax::ADD:
      if (set) registers.setFlags(n, m, (uint64_t)n + m, '+');
      result = n + m;
      break;
    case syntax::SUB:
      if (set) registers.setFlags(n, m, (uint64_t)n - m, '-');
      result = n - m;
      break;
    case syntax::RSB:
      if (set) registers.setFlags(m, n, (uint64_t)m - n, '-');
      result = m - n;
    case syntax::BIC:
      if (set) registers.setFlags(m, n, (uint64_t)n & (~m));
      result = n & (~m);
  } 

  registers[dest] = result;
  return true;
}

/**
 * TODO: implement ASR and ROR
 * Executes a shift operation
 */ 
bool Emulator::executeShift(syntax::ShiftNode* instruction) {
  auto [op, cond, set, dest, src1, src2] = instruction->unpack();     // unpack the instruction
  if (!registers.checkFlags(cond)) return false;                            // returns early if condition code is not satisfied

  uint32_t n = registers[src1];
  uint32_t m;
  if (src2.index() == 1) m = registers[std::get<syntax::REGISTER>(src2)];
  else m = std::get<int>(src2);

  int result;
  switch (op) {                                                        // check opcode and execute instruction
    case syntax::LSL:
      if (set) registers.setFlags(n, m, n << m);
      result = n << m;
      break;
    case syntax::LSR:
      if (set) registers.setFlags(n, m, n >> m);
      result = n >> m;
      break;
    case syntax::ASR:
    case syntax::ROR:
    default:
      break;
  }

  registers[dest] = result;
  return true;
}

/**
 * TODO: implement
 * Executes a branch operation
 */ 
bool Emulator::executeBranch(syntax::BranchNode* instruction) {
  auto [op, cond, to] = instruction->unpack();
  if (!registers.checkFlags(cond)) return false;                          // returns early if condition code is not satisfied

  int address;
  if (to.index() == 0) address = registers[std::get<syntax::REGISTER>(to)];
  else address = memory.label(std::get<std::string>(to));
  
  switch (op) {
    case syntax::B:
      // std::cout << "B" << std::endl;
      registers[syntax::PC] = address;
      break;
    case syntax::BL:
      // std::cout << "BL" << std::endl;
      registers[syntax::LR] = registers[syntax::PC] + 32;
      registers[syntax::PC] = address;
      break;
    case syntax::BX:
      // std::cout << "BX" << std::endl;
      registers[syntax::PC] = address;
      break;
  }

  return true;
}

/**
 * Switches the emulator mode so that it knows to parse data or text.
 */
void Emulator::mode(MODE mode) {
  _mode = mode;
}

// void Emulator::toggleEditor() {
//   if (editor_w->visible()) editor_w->hide();
//   else editor_w->show();
// }