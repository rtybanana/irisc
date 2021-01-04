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
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Editor.H>
#include "../emulator.h"

namespace vm {

  class Emulator;

  class TextBuffer : public Fl_Text_Buffer {
    public:
      TextBuffer() : Fl_Text_Buffer() {}
      // std::vector<std::string> lines();
  };

  class Editor : public Fl_Group {
    private:
      // Fl_Window* window;
      Emulator* emulator;
      Fl_Text_Buffer* stylebuf;
      std::map<std::string, char> styleMap;
      bool cursorHidden;
      void fetchStyles();
      
    public:
      Editor(int, int, Emulator*);
      TextBuffer* textbuf;
      Fl_Text_Editor* editor;
      // void toggle();
      void blink();
      void highlight();
      void highlightLine(int);
      void wavyLine(std::vector<syntax::ErrorNode>);
      void run();
      void stop();
      void step();
      void speed(double);
  };
}

#endif //IRISC_EDITOR_H