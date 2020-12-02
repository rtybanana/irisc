/**
 * @file gui.h
 * Holds an abstract class defining common functionality of GUI windows and static functions 
 * which are useful for handling them.
 * @author Rory Pinkney
 * @date 28/10/20
 */

#ifndef IRISC_GUI_H
#define IRISC_GUI_H

#include <FL/Fl.H>
#include <FL/fl_ask.H>

namespace gui {

  static void close_cb(Fl_Widget *widget, void *) {
    int result = fl_choice("Are you sure you want to exit?", "Yes", "No", 0);
    if (result == 0) Fl::awake(new int(0));
  }
  
}

#endif  // IRISC_GUI_H