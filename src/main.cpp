#include <iostream>
#include <thread>
#include <fstream>
#include <regex>
#include <iomanip>
#include <FL/Fl.H>

#include "emulator/emulator.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "parser/syntax.h"
#include "ui/repl.h"

#define STR(x)   #x
#define SHOW_DEFINE(x) printf("%s=%s\n", #x, STR(x))

int main() {
    SHOW_DEFINE(FL_ABI_VERSION);

    Fl::lock();
    vm::Emulator emulator;
    replxx::Replxx rx;
    ui::REPL repl(rx);
    std::thread repl_t(&ui::REPL::loop, repl, std::ref(emulator));

    while (true) {
        Fl::wait();
        if (Fl::thread_message()) {
            repl_t.detach();
            repl_t.~thread();
            break;
        }
    }

    if (repl_t.joinable()) repl_t.join();

    // final cleanup
    while(Fl::first_window()) delete Fl::first_window();
    std::cout << "\nGoodbye" << std::endl;

    return 0;
}
