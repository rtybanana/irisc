/**
 * @file editor.h
 * Contains implementation of a code editor window which can be used to run multi-line 
 * ARMv7 scripts line-by-line.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#include <map>
#include <vector>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>

namespace ui {
  class TextBuffer : public Fl_Text_Buffer {
    public:
      TextBuffer() : Fl_Text_Buffer() {}
      std::vector<std::string> lines();
  };

  class Editor {
    private:
      Fl_Window* window;
      Fl_Text_Buffer* stylebuf;
      std::map<std::string, char> styleMap;
      bool cursorHidden;
      void fetchStyles();
      
    public:
      Editor();
      TextBuffer* textbuf;
      Fl_Text_Editor* editor;
      void toggle();
      void blink();
      void highlight();
  };
}

