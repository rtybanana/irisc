#include "editor.h"
#include "constants.h"
#include "../../parser/constants.h"
#include <regex>
#include <iostream>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Hor_Nice_Slider.H>

using namespace vm;

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  if (nInserted || nDeleted) {
    Editor *editor = (Editor *)v;
    editor->editor->show_insert_position();
    editor->highlight();
  }
}

void run_cb(Fl_Widget* widget, void* v) {
  Editor* editor = (Editor*) v;
  editor->run();
}

void stop_cb(Fl_Widget* widget, void* v) {
  Editor* editor = (Editor*) v;
  editor->stop();
}

void step_cb(Fl_Widget* widget, void* v) {
  Editor* editor = (Editor*) v;
  editor->step();
}

void speed_cb(Fl_Widget* widget, void* v) {
  Fl_Hor_Nice_Slider* slider = (Fl_Hor_Nice_Slider*)widget;
  Editor* editor = (Editor*)v;
  editor->speed(slider->value());
}

void cursorBlink(void* window) {
  Editor* editor = (Editor*)window;
  editor->blink();
  Fl::repeat_timeout(0.5, cursorBlink, window);
}

Editor::Editor(int x, int y, vm::Emulator* emulator) : Fl_Group(x, y, 890, 380), cursorHidden(false), emulator(emulator) {
  // emulator.setEditor(this);           // link editor and emulator

  Fl::lock();
    // window = new Fl_Window(280,350,"Editor");
    // window->

    // this->color(fl_rgb_color(uchar(35)));

    editor = new Fl_Text_Editor(10, 40, 870, 374);
    Fl_Box* border = new Fl_Box(9, 39, 872, 376);
    textbuf = new TextBuffer();
    stylebuf = new Fl_Text_Buffer();

    editor->buffer(textbuf);
    editor->cursor_style(Fl_Text_Display::SIMPLE_CURSOR);
    editor->cursor_color(FL_WHITE);
    editor->textfont(FL_COURIER);      // style buffer
    editor->box(FL_NO_BOX);
    editor->color(fl_rgb_color(uchar(20)));
    editor->textcolor(FL_WHITE);
    editor->textsize(Fl_Fontsize(20));
    editor->selection_color(FL_YELLOW);
    editor->linenumber_width(30);
    editor->linenumber_fgcolor(vm::light);
    editor->linenumber_bgcolor(fl_rgb_color(uchar(35)));

    border->box(FL_BORDER_FRAME);
    border->color(fl_rgb_color(uchar(150)));

    int stylesize = sizeof(styles)/sizeof(styles[0]);
    editor->highlight_data(stylebuf, styles, stylesize, 'A', 0, 0);

    textbuf->tab_distance(4);
    textbuf->add_modify_callback(changed_cb, this);
    textbuf->call_modify_callbacks();
    
    Fl::add_timeout(0.5, cursorBlink, this);

    Fl_Group* ctrl = new Fl_Group(10, 10, 870, 25);
    Fl_Hor_Nice_Slider* spd = new Fl_Hor_Nice_Slider(670, 10, 120, 25);
    Fl_Button* stp = new Fl_Button(795, 10, 25, 25, "@-4square");
    Fl_Button* run = new Fl_Button(825, 10, 25, 25, "@+1>");
    Fl_Button* fwd = new Fl_Button(855, 10, 25, 25, "@+1>|");
    spd->bounds(2.5, 0.05);
    spd->value(1);
    spd->color(vm::dark);
    stp->box(FL_BORDER_FRAME);
    stp->color(vm::grey);
    stp->labelcolor(fl_rgb_color(uchar(0xad), uchar(0x0b), uchar(0x0b)));
    run->box(FL_BORDER_FRAME);
    run->color(vm::grey);
    run->labelcolor(fl_rgb_color(uchar(0x0b), uchar(0xad), uchar(0x0b)));
    fwd->box(FL_BORDER_FRAME);
    fwd->color(vm::grey);
    fwd->labelcolor(fl_rgb_color(uchar(0x0b), uchar(0xad), uchar(0x0b)));

    run->callback(run_cb, this);
    stp->callback(stop_cb, this);
    fwd->callback(step_cb, this);
    spd->callback(speed_cb, this);

    Fl_Box* space = new Fl_Box(10, 10, 660, 25);
    ctrl->resizable(space);
    ctrl->end();

    end();
    resizable(editor);
  Fl::unlock();

  Fl::awake();
  fetchStyles();
};

void Editor::fetchStyles() {
	for (auto const& [op, i] : syntax::opMap) {
		for (auto flag : {"", "s"}) {
			for (auto const& [cond, i] : syntax::condMap) {
				styleMap.insert({op + flag + cond, 'B'});
			}
		}
	}

	for (auto const& [reg, i] : syntax::regMap) {
		styleMap.insert({reg, 'C'});
	}

  for (auto const& directive : directives) {
    styleMap.insert({directive, 'D'});
  }
};

void Editor::highlight() {
  std::string input = textbuf->text();
  std::regex re("[, \\n\\t]+");
  
  std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
  std::sregex_token_iterator end;

  std::string style;
  int basepoint = 0;
  for (; it != end; ++it) {
    std::string match = it->str();
    int c1 = match.size();                    // the length of the word
    int c2 = input.find(match, basepoint);    // the starting position of the word
    for (int i = 0; i < c2 - basepoint; i++) style += ' ';
    basepoint = c1 + c2;                      // start point for the find function for the next loop
    if (styleMap.contains(match)) {
      char styleType = styleMap.at(match);
      // std::cout << match << " at position " << c2 << " of size " << c1 << " | start next check from " << basepoint << std::endl;
      for (int i = 0; i < c1; i++) style += styleType;
    }
    else for (int i = 0; i < c1; i++) style += ' ';
  }

  stylebuf->text(style.c_str());
}

void Editor::highlightLine(int lineNumber) {
  editor->selection_color(FL_YELLOW);

  if (lineNumber == -1) {
    textbuf->highlight(0, 0);
    return;
  }
  int lineStart = textbuf->skip_lines(0, lineNumber - 1);
  int lineEnd = textbuf->line_end(lineStart);
  textbuf->highlight(lineStart, lineEnd);
}

void Editor::wavyLine(std::vector<syntax::ErrorNode> errors) {
  // editor->selection_color(FL_RED);

  // for (syntax::ErrorNode error : errors) {
  //   int lineStart = textbuf->skip_lines(0, error.error(). - 1);
  //   int lineEnd = textbuf->line_end(lineStart);
  // }
}

void Editor::blink() {
  cursorHidden = !cursorHidden;
  editor->show_cursor(!cursorHidden);
}

void Editor::run() {
  std::string program(textbuf->text());
  emulator->run(program);
}

void Editor::stop() {
  emulator->stop();
}

void Editor::step() {
  //
}

void Editor::speed(double delay) {
  emulator->speed(delay);
}
