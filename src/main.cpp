#include <iostream>
// #include <ncurses.h>
#include <fstream>
#include <regex>
#include <iomanip>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "parser/syntax.h"

int main() {

    // Greeting
    std::cout << "\033[3m" << "i" << "\033[0m" << "\033[1m" << "RISC"  << "\033[0m " << "0.0.1" << std::endl;
    std::cout << "Type \":h\" for more information." << std::endl;

    // int k = 0;
    // indefinite prompt for user input
    for(;;){

        // k = 0;

        // switch((k=getch())) {
        // case KEY_UP:
        //     std::cout << "Up" << std::endl;//key up
        //     break;
        // case KEY_DOWN:
        //     std::cout << "Down" << std::endl;   // key down
        //     break;
        // case KEY_LEFT:
        //     std::cout << "Left" << std::endl;  // key left
        //     break;
        // case KEY_RIGHT:
        //     std::cout << "Right" << std::endl;  // key right
        //     break;
        // default:
        //     std::cout << "null" << std::endl;  // not arrow
        //     break;
        // }

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
        if(input_line == ":q"){
            break;
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
        }

        try {
            lexer::Lexer lexer(input_line);
            parser::Parser parser(lexer);
            syntax::InstructionNode* node = parser.parseSingle();
            unsigned int instruction = node->assemble();
            
        }

        // Catch exception and print error
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}