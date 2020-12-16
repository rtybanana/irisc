#include "editor.h"
#include "constants.h"
#include "../parser/constants.h"
#include <regex>
#include <iostream>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

using namespace ui;

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  if (nInserted || nDeleted) {
    Editor *w = (Editor *)v;
    w->editor->show_insert_position();
    w->highlight();
  }
}

void run_cb(Fl_Widget* widget, void* v) {
  Editor* editor = (Editor*) v;
  editor->textbuf->lines();
}

void cursorBlink(void* window) {
  Editor* editor = (Editor*)window;
  editor->blink();
  Fl::repeat_timeout(0.5, cursorBlink, window);
}

Editor::Editor() : cursorHidden(false) {
  Fl::lock();
    window = new Fl_Window(280,350,"Editor");
    window->color(fl_rgb_color(uchar(35)));

    editor = new Fl_Text_Editor(10, 40, 260, 300);
    Fl_Box* border = new Fl_Box(9, 39, 262, 302);
    textbuf = new TextBuffer();
    stylebuf = new Fl_Text_Buffer();

    editor->buffer(textbuf);
    editor->cursor_style(Fl_Text_Display::SIMPLE_CURSOR);
    editor->cursor_color(FL_WHITE);
    editor->textfont(FL_COURIER);      // style buffer
    editor->box(FL_NO_BOX);
    editor->color(fl_rgb_color(uchar(20)));
    editor->textcolor(FL_WHITE);
    editor->textsize(15);
    editor->selection_color(FL_YELLOW);

    border->box(FL_BORDER_FRAME);
    border->color(fl_rgb_color(uchar(150)));

    int stylesize = sizeof(styles)/sizeof(styles[0]);
    editor->highlight_data(stylebuf, styles, stylesize, 'A', 0, 0);

    textbuf->tab_distance(4);
    textbuf->add_modify_callback(changed_cb, this);
    textbuf->call_modify_callbacks();
    
    Fl::add_timeout(0.5, cursorBlink, this);

    Fl_Group* btns = new Fl_Group(10, 10, 260, 25);
    Fl_Button* run = new Fl_Button(215, 10, 25, 25, "@+1>");
    Fl_Button* ffw = new Fl_Button(245, 10, 25, 25, "@+1>>");
    run->box(FL_BORDER_FRAME);
    run->color(ui::grey);
    run->labelcolor(fl_rgb_color(uchar(0x0b), uchar(0xad), uchar(0x0b)));
    ffw->box(FL_BORDER_FRAME);
    ffw->color(ui::grey);;
    ffw->labelcolor(fl_rgb_color(uchar(0x0b), uchar(0xad), uchar(0x0b)));
    run->callback(run_cb, this);

    Fl_Box* space = new Fl_Box(10, 10, 200, 25);
    btns->resizable(space);
    btns->end();

    window->end();
    window->resizable(editor);
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

void Editor::blink() {
  cursorHidden = !cursorHidden;
  editor->show_cursor(!cursorHidden);
}

void Editor::toggle() {
  if (window->visible()) window->hide();
  else window->show();
}


/**
 * TextBuffer class extension
 */
std::vector<std::string> TextBuffer::lines() {
  std::string text(this->text());
  std::cout << text << std::endl;
  // int index = 0;
  // for (char c : text) {
  //   std::cout << c << std::endl;
  // }

  return {"", ""};
}