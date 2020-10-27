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


void repl(vm::Emulator emulator) {

    // Greeting
    std::cout << "\033[3m" << "i" << "\033[0m" << "\033[1m" << "RISC"  << "\033[0m " << "0.0.1" << std::endl;
    std::cout << "Type \":h\" for more information." << std::endl;

    // indefinite prompt for user input
    for(;;){

        // variables for user input
        std::string input_line;
        std::string program;
        bool file_load = false;
        bool expr = false;

        // prompt
        std::cout << "\n>>> _\b";
        std::getline(std::cin, input_line);

        // remove leading and trailing whitespace
        input_line = std::regex_replace(input_line, std::regex("^ +| +$"), "$1");

        if (input_line == "") continue;

        // quit command
        if (input_line == ":q"){
            break;
        }

        if (input_line == ":r" || input_line == ":reset") {
            emulator.reset();
        }

        // help command
        else if(input_line == ":h"){

            std::cout << "\n" << "                            Welcome to " << 
                                    "\033[3m" << "i" << "\033[0m" << "\033[1m" << "RISC"  << "\033[0m ";
			std::cout << "\n" << "             The Interactive Reduced Instruction-Set Computer                  \n\n";

            std::cout <<         "To use this interactive console, just type in regular ARMv7 assembly code and     \n";
            std::cout <<         "hit enter. You can also make use of the following commands: \n\n";

            std::cout <<         " :load \e[1;3;4mfile-path\e[0m  ";
            std::cout <<         "Loads variable and function declarations from a specified\n";
            std::cout <<         std::setw(18);
            std::cout <<         "" << "file into memory, e.g.\n";
            std::cout <<         std::setw(18);
            std::cout <<         "" << ">>> #load ~/hello_world.s\n\n";
            std::cout <<         " :q               Exits the interactive RISC program.\n\n";
            std::cout <<         " :c               Clears the terminal window." << std::endl;
        }

        // clear screen commands
        else if(input_line == ":c"){
            std::cout << std::string(50, '\n');
        }

        // Parse as assembler
        else {
            try {
                lexer::Lexer lexer(input_line);
                parser::Parser parser(lexer);
                syntax::InstructionNode* node = parser.parseSingle();
                unsigned int instruction = node->assemble();
                emulator.execute(node);
                emulator.printRegisters();
            }
            
            // Catch exception and print error
            catch(const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    Fl::awake(new int(0));
}

int main() {
    Fl::lock();
    vm::Emulator emulator;
    std::thread repl_t(repl, emulator);

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

    return 0;
}