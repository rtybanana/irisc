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
#include "repl/repl.h"

int main() {
    Fl::lock();
    vm::Emulator emulator;
    replxx::Replxx rx;
    irepl::REPL repl(rx);
    std::thread repl_t(&irepl::REPL::loop, repl, std::ref(emulator));

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
