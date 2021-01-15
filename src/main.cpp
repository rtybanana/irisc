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
#include "cmd/repl.h"

int main(int argc, char* argv[]) {
    bool is_headless;
    std::string not_headless = "--gui";
    if (argc == 1) is_headless = true;
    else if (argc == 2 && not_headless == argv[1]) is_headless = false;
    else {
        if (argc > 2) std::cout << "\033[91mArgs Error\033[0m: Too many arguments!" << std::endl;
        else std::cout << "\033[91mArgs Error\033[0m: Unrecognised argument '" << argv[2] << "'." << std::endl;
        return 1;
    }

    Fl::lock();
    vm::Emulator emulator(is_headless);
    replxx::Replxx rx;
    cmd::REPL repl(rx);
    std::thread repl_t(&cmd::REPL::cmdloop, repl, std::ref(emulator));

    while (true && !is_headless) {
        Fl::wait();
        if (Fl::thread_message()) {
            repl_t.detach();
            repl_t.~thread();
            break;
        }
    }

    if (repl_t.joinable()) repl_t.join();

    // std::cout << "pre-final cleanup" << std::endl;

    // final cleanup
    // while(Fl::first_window()) delete Fl::first_window();
    std::cout << "\nGoodbye" << std::endl;

    return 0;
}
