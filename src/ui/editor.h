/**
 * @file editor.h
 * Contains implementation of a code editor window which can be used to run multi-line 
 * ARMv7 scripts line-by-line.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_EDITOR_H
#define IRISC_EDITOR_H

#include <map>
#include <vector>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include "../emulator/emulator.h"

namespace vm {
  class Emulator;
}

namespace ui {
  class TextBuffer : public Fl_Text_Buffer {
    public:
      TextBuffer() : Fl_Text_Buffer() {}
      // std::vector<std::string> lines();
  };

  class Editor {
    private:
      Fl_Window* window;
      vm::Emulator& emulator;
      Fl_Text_Buffer* stylebuf;
      std::map<std::string, char> styleMap;
      bool cursorHidden;
      void fetchStyles();
      
    public:
      Editor(vm::Emulator&);
      TextBuffer* textbuf;
      Fl_Text_Editor* editor;
      void toggle();
      void blink();
      void highlight();
      void highlightLine(int);
      void run();
      void stop();
  };
}

#endif //IRISC_EDITOR_H