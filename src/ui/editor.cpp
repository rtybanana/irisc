#include "editor.h"
#include "constants.h"
#include "../parser/constants.h"
#include <regex>
#include <iostream>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>

using namespace ui;

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  if (nInserted || nDeleted) {
    Editor *w = (Editor *)v;
    w->editor->show_insert_position();
    w->highlight();
  }
}

void cursorBlink(void* window) {
  Editor* editor = (Editor*)window;
  editor->blink();
  Fl::repeat_timeout(0.5, cursorBlink, window);
}

Editor::Editor() : cursorHidden(false) {
  Fl::set_color(FL_RED, uchar(255), uchar(85), uchar(85));
  Fl::set_color(FL_BLUE, uchar(100), uchar(100), uchar(255));

  Fl::lock();
    window = new Fl_Window(280,350,"Editor");
    window->color(FL_BLACK);

    editor = new Fl_Text_Editor(10, 30, 260, 310);
    Fl_Box* border = new Fl_Box(9, 29, 262, 312);
    textbuf = new Fl_Text_Buffer();
    stylebuf = new Fl_Text_Buffer();

    editor->buffer(textbuf);
    editor->cursor_style(Fl_Text_Display::SIMPLE_CURSOR);
    editor->cursor_color(FL_WHITE);
    editor->textfont(FL_COURIER);      // style buffer
    editor->box(FL_NO_BOX);
    editor->color(FL_BLACK);
    editor->textcolor(FL_WHITE);
    editor->textsize(15);
    editor->selection_color(FL_YELLOW);

    border->box(FL_BORDER_FRAME);
    border->color(FL_WHITE);

    int stylesize = sizeof(styles)/sizeof(styles[0]);
    editor->highlight_data(stylebuf, styles, stylesize, 'A', 0, 0);

    textbuf->tab_distance(4);
    textbuf->add_modify_callback(changed_cb, this);
    textbuf->call_modify_callbacks();
    
    Fl::add_timeout(0.5, cursorBlink, this);

    window->end();
    window->resizable(editor);
    // window->show();
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
      std::cout << match << " at position " << c2 << " of size " << c1 << " | start next check from " << basepoint << std::endl;
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