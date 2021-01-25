#include <regex>
#include <string>
#include <vector>
#include <numeric>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <utility>
#include <iomanip>
#include <iostream>
#include <thread>
#include <chrono>
#include <FL/Fl.H>

#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../parser/constants.h"
#include "../emulator/constants.h"
// #include "editor.h"

#include "repl.h"
#include "util.h"

using Replxx = replxx::Replxx;
using namespace cmd;

REPL::REPL(Replxx& rx) : rx(rx), ops(), complexOps(), conds(), regs() {
	rx.install_window_change_handler();

	// the path to the history file
	history_file = "./replxx_history.txt";

	// load the history file if it exists
	rx.history_load(history_file);

	// set the max history size
	rx.set_max_history_size(128);

	// set the max number of hint rows to show
	rx.set_max_hint_rows(3);

	// set the callbacks
	using namespace std::placeholders;
	rx.set_completion_callback(std::bind( &cmd::REPL::hook_completion, this, _1, _2));
	rx.set_highlighter_callback(std::bind( &cmd::REPL::hook_color, this, _1, _2));
	rx.set_hint_callback(std::bind( &cmd::REPL::hook_hint, this, _1, _2, _3));

	// other api calls
	// rx.set_word_break_characters( " \t.,-%!;:=*~^'\"/?<>|[](){}" );
	rx.set_completion_count_cutoff( 128 );
	rx.set_double_tab_completion( false );
	rx.set_complete_on_empty( true );
	rx.set_beep_on_ambiguous_completion( false );
	rx.set_no_color( false );

	// showcase key bindings
	rx.bind_key_internal( Replxx::KEY::BACKSPACE,                      "delete_character_left_of_cursor" );
	rx.bind_key_internal( Replxx::KEY::DELETE,                         "delete_character_under_cursor" );
	rx.bind_key_internal( Replxx::KEY::LEFT,                           "move_cursor_left" );
	rx.bind_key_internal( Replxx::KEY::RIGHT,                          "move_cursor_right" );
	rx.bind_key_internal( Replxx::KEY::UP,                             "history_previous" );
	rx.bind_key_internal( Replxx::KEY::DOWN,                           "history_next" );
	rx.bind_key_internal( Replxx::KEY::PAGE_UP,                        "history_first" );
	rx.bind_key_internal( Replxx::KEY::PAGE_DOWN,                      "history_last" );
	rx.bind_key_internal( Replxx::KEY::HOME,                           "move_cursor_to_begining_of_line" );
	rx.bind_key_internal( Replxx::KEY::END,                            "move_cursor_to_end_of_line" );
	rx.bind_key_internal( Replxx::KEY::TAB,                            "complete_line" );
	rx.bind_key_internal( Replxx::KEY::control( Replxx::KEY::LEFT ),   "move_cursor_one_word_left" );
	rx.bind_key_internal( Replxx::KEY::control( Replxx::KEY::RIGHT ),  "move_cursor_one_word_right" );
	rx.bind_key_internal( Replxx::KEY::control( Replxx::KEY::UP ),     "hint_previous" );
	rx.bind_key_internal( Replxx::KEY::control( Replxx::KEY::DOWN ),   "hint_next" );
	rx.bind_key_internal( Replxx::KEY::control( Replxx::KEY::ENTER ),  "commit_line" );
	rx.bind_key_internal( Replxx::KEY::control( 'R' ),                 "history_incremental_search" );
	rx.bind_key_internal( Replxx::KEY::control( 'W' ),                 "kill_to_begining_of_word" );
	rx.bind_key_internal( Replxx::KEY::control( 'U' ),                 "kill_to_begining_of_line" );
	rx.bind_key_internal( Replxx::KEY::control( 'K' ),                 "kill_to_end_of_line" );
	rx.bind_key_internal( Replxx::KEY::control( 'Y' ),                 "yank" );
	rx.bind_key_internal( Replxx::KEY::control( 'L' ),                 "clear_screen" );
	rx.bind_key_internal( Replxx::KEY::control( 'D' ),                 "send_eof" );
	rx.bind_key_internal( Replxx::KEY::control( 'C' ),                 "abort_line" );
	rx.bind_key_internal( Replxx::KEY::control( 'T' ),                 "transpose_characters" );
#ifndef _WIN32
	rx.bind_key_internal( Replxx::KEY::control( 'V' ),                 "verbatim_insert" );
	rx.bind_key_internal( Replxx::KEY::control( 'Z' ),                 "suspend" );
#endif
	rx.bind_key_internal( Replxx::KEY::meta( Replxx::KEY::BACKSPACE ), "kill_to_whitespace_on_left" );
	rx.bind_key_internal( Replxx::KEY::meta( 'p' ),                    "history_common_prefix_search" );
	rx.bind_key_internal( Replxx::KEY::meta( 'n' ),                    "history_common_prefix_search" );
	rx.bind_key_internal( Replxx::KEY::meta( 'd' ),                    "kill_to_end_of_word" );
	rx.bind_key_internal( Replxx::KEY::meta( 'y' ),                    "yank_cycle" );
	rx.bind_key_internal( Replxx::KEY::meta( 'u' ),                    "uppercase_word" );
	rx.bind_key_internal( Replxx::KEY::meta( 'l' ),                    "lowercase_word" );
	rx.bind_key_internal( Replxx::KEY::meta( 'c' ),                    "capitalize_word" );
	rx.bind_key_internal( 'a',                                         "insert_character" );
	rx.bind_key_internal( Replxx::KEY::INSERT,                         "toggle_overwrite_mode" );
	rx.bind_key( Replxx::KEY::F1, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F1>", _1 ) );
	rx.bind_key( Replxx::KEY::F2, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F2>", _1 ) );
	rx.bind_key( Replxx::KEY::F3, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F3>", _1 ) );
	rx.bind_key( Replxx::KEY::F4, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F4>", _1 ) );
	rx.bind_key( Replxx::KEY::F5, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F5>", _1 ) );
	rx.bind_key( Replxx::KEY::F6, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F6>", _1 ) );
	rx.bind_key( Replxx::KEY::F7, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F7>", _1 ) );
	rx.bind_key( Replxx::KEY::F8, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F8>", _1 ) );
	rx.bind_key( Replxx::KEY::F9, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F9>", _1 ) );
	rx.bind_key( Replxx::KEY::F10, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F10>", _1 ) );
	rx.bind_key( Replxx::KEY::F11, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F11>", _1 ) );
	rx.bind_key( Replxx::KEY::F12, std::bind( &cmd::REPL::message, this, std::ref( rx ), "<F12>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F2 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F2>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F3 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F3>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F4 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F4>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F5 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F5>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F6 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F6>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F1 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F1>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F7 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F7>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F8 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F8>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F9 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F9>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F10 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F10>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F11 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F11>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::F12 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-F12>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F1 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F1>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F2 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F2>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F3 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F3>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F4 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F4>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F5 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F5>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F6 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F6>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F7 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F7>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F8 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F8>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F9 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F9>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F10 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F10>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F11 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F11>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::F12 ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-F12>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::TAB ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Tab>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::HOME ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-Home>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::HOME ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Home>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::END ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-End>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::END ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-End>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::PAGE_UP ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-PgUp>", _1 ) );
	rx.bind_key( Replxx::KEY::control( Replxx::KEY::PAGE_DOWN ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<C-PgDn>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::LEFT ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Left>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::RIGHT ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Right>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::UP ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Up>", _1 ) );
	rx.bind_key( Replxx::KEY::shift( Replxx::KEY::DOWN ), std::bind( &cmd::REPL::message, this, std::ref( rx ), "<S-Down>", _1 ) );

	fetchTokens();
}

void REPL::fetchTokens() {
	highlights = regex_color;
	for (auto const& [op, i] : syntax::opMap) {
		ops.push_back(op);
		for (auto flag : {"", "s"}) {
			for (auto const& [cond, i] : syntax::condMap) {
				complexOps.push_back(op + flag + cond);
				highlights.push_back({op + flag + cond, replxx::Replxx::Color::BRIGHTRED});
			}
		}
	}

	for (auto const& [op, i] : syntax::shiftMap) {
		ops.push_back(op);
		for (auto flag : {"", "s"}) {
			for (auto const& [cond, i] : syntax::condMap) {
				complexOps.push_back(op + flag + cond);
				highlights.push_back({op + flag + cond, replxx::Replxx::Color::BRIGHTRED});
			}
		}
	}

	for (auto const& [reg, i] : syntax::regMap) {
		regs.push_back(reg);
		highlights.push_back({reg, replxx::Replxx::Color::BRIGHTBLUE});
	}
}

void REPL::cmdloop(vm::Emulator &emulator) {
	// Editor editor(emulator);
	std::cout << "\e[95miRISC\e[0m 0.0.1  [22nd Nov, 2020]" << std::endl;
  std::cout << "Type \"help\" for more information.\n" << std::endl;

	// set the repl prompt
	std::string prompt {"\033[95miRISC\033[0m:~$ "};

	// main repl loop
	for (;;) {
		// display the prompt and retrieve input from the user
		char const* cinput{ nullptr };

		do {
			cinput = rx.input(prompt);
		} while ( ( cinput == nullptr ) && ( errno == EAGAIN ) );

		if (cinput == nullptr) {
			break;
		}

		// change cinput into a std::string
		// easier to manipulate
		std::string input {cinput};

		if (input == "") continue;

		// quit command
		if (input == "exit" || input == "x"){
			rx.history_add(input);
			break;
		}

		if (input == "reset" || input == "!") {
			rx.history_add(input);
			emulator.reset();
			std::cout << "\tiRISC emulator has been reset." << std::endl;
			// emulator.printRegisters();
		}

		// help command
		else if(input == "help" || input == "h"){
			rx.history_add(input);
			std::cout << "\n" << "                            Welcome to " << 
															"\033[3m" << "i" << "\033[0m" << "\033[1m" << "RISC"  << "\033[0m ";
			std::cout << "\n" << "             The Interactive Reduced Instruction-Set Computer                  \n\n";

			std::cout <<         "To use this interactive console, just type in regular ARMv7 assembly code and     \n";
			std::cout <<         "hit enter. You can also make use of the following commands: \n\n";

			std::cout << 				 " \033[1;3;4mARMv7 Instruction\033[0m  Performs the instruction and prints the result to the console. \n";
			std::cout <<         " registers/r    "       <<       "    Prints the current registers to the console.\n";
			std::cout <<         " assemble/a     "       <<       "    Prints the assembly code for the previous instruction.\n";
			std::cout <<         " clear/c        "       <<       "    Clears the terminal window. \n";
			std::cout <<         " reset/!        "       <<       "    Resets the registers to their default values. \n";
			std::cout <<         " exit/x         "       <<       "    Exits the interactive RISC program.\n\n";
		}

		// clear screen commands
		else if(input == "clear" || input == "c"){
			rx.history_add(input);
			std::cout << std::string(50, '\n');
		}

		// else if (input == ":e" || input == ":editor") {
		// 	// emulator.toggleEditor();
		// }

		else if (input == "registers" || input == "r") {
			emulator.printRegisters();
		}

		else if (input == "cpsr" || input == "flags" || input == "f") {
			emulator.printCPSR();
		}

		else if (input == "assemble" || input == "a") {
			emulator.assembleLast();
		}

		else if (input == "operations") {
			for (auto [name, id] : syntax::opMap) std::cout << "\t\e[91m" << std::setw(3) << name << "\e[0m | " << syntax::opExplain.at(id) << std::endl;
		}

		else if (input == "conditions") {
			for (auto [name, id] : syntax::condMap) if (name.size() > 0) std::cout << "\t\e[91m" << std::setw(2) << name << "\e[0m | " << syntax::condTitle.at(id) << ". " << syntax::condShortExplain.at(id) << std::endl;
		}

		// Parse as assembler
		else {
			emulator.execute(input);
			rx.history_add(input);							// add to history regardless
		}	
	}

	rx.history_sync(history_file);
	Fl::awake(new int(0));
}


Replxx::completions_t REPL::hook_completion(std::string const& context, int& contextLen) {
	Replxx::completions_t completions;

	int tokens = 1;
	std::for_each(context.begin(), context.end(), [&tokens](char e){ if (isspace(e)) tokens++; });

	int utf8ContextLen( context_len( context.c_str() ) );
	int prefixLen( context.length() - utf8ContextLen );
	if ( ( prefixLen > 0 ) && ( context[prefixLen - 1] == '\\' ) ) {
		-- prefixLen;
		++ utf8ContextLen;
	}
	contextLen = utf8str_codepoint_len( context.c_str() + prefixLen, utf8ContextLen );

	std::string prefix { context.substr(prefixLen) };
	if (prefix.size() >= 1) {
		if (prefix[0] == '.') {
			for (auto const& e : directives) {
				if (e.compare(0, prefix.size(), prefix) == 0) {
					completions.emplace_back(e.c_str());
				}
			}
		}

		else if (tokens == 1) {									// operation completion
			if (prefix.size() < 3) {
				for (auto const& e : ops) {
					if (e.compare(0, prefix.size(), prefix) == 0) {
						completions.emplace_back(e.c_str());
					}
				}
			}
			else {
				for (auto const& e : complexOps) {
					if (e.compare(0, prefix.size(), prefix) == 0) {
						completions.emplace_back(e.c_str());
					}
				}
			}
		}
	}

	return completions;
}

Replxx::hints_t REPL::hook_hint(std::string const& context, int& contextLen, Replxx::Color& color) {
	Replxx::hints_t hints;

	// std::cout << context << std::endl;
	int tokens = 1;
	std::for_each(context.begin(), context.end(), [&tokens](char e){ if (isspace(e)) tokens++; });
	// std::cout << tokens << std::endl;
	
	// only show hint if prefix is at least 'n' chars long
	// or if prefix begins with a specific character

	int utf8ContextLen( context_len( context.c_str() ) );
	int prefixLen( context.length() - utf8ContextLen );
	contextLen = utf8str_codepoint_len( context.c_str() + prefixLen, utf8ContextLen );
	std::string prefix { context.substr(prefixLen) };
	// std::cout << "\n  prefic: " << prefix << std::endl;

	if (prefix.size() >= 1) {
		if (prefix[0] == '.') {
			for (auto const& e : directives) {
				if (e.compare(0, prefix.size(), prefix) == 0) {
					hints.emplace_back(e.c_str());
				}
			}
		}

		else if (tokens == 1) {									// operation completion
			if (prefix.size() < 3) {
				for (auto const& e : ops) {
					if (e.compare(0, prefix.size(), prefix) == 0) {
						hints.emplace_back(e.c_str());
					}
				}
			}
			else {
				for (auto const& e : complexOps) {
					if (e.compare(0, prefix.size(), prefix) == 0) {
						hints.emplace_back(e.c_str());
					}
				}
			}
		}
	}
	

	// set hint color to green if single match found
	// if (hints.size() == 1) {
	// 	color = Replxx::Color::BRIGHTBLUE;
	// }

	return hints;
}

void REPL::hook_color(std::string const& context, Replxx::colors_t& colors) {
	std::vector<std::string> tokens;
	std::vector<int> offsets;
	std::regex words_regex("[\\s,]|[^\\s,]+");
	auto words_begin = std::sregex_iterator(context.begin(), context.end(), words_regex);
	auto words_end = std::sregex_iterator();
	for (std::sregex_iterator i = words_begin; i != words_end; ++i) tokens.push_back(i->str());

	// highlight matching regex sequences
	int offset = 0;
	for (std::string token : tokens) {
		for (auto const& e : highlights) {
			size_t pos {0};
			std::string str = token;
			std::smatch match;

			while(std::regex_match(str, match, std::regex(e.first))) {
				std::string c{ match[0] };
				std::string prefix( match.prefix().str() );
				pos += utf8str_codepoint_len( prefix.c_str(), static_cast<int>( prefix.length() ) );
				int len( utf8str_codepoint_len( c.c_str(), static_cast<int>( c.length() ) ) );

				for (int i = 0; i < len; ++i) {
					colors.at(pos + i + offset) = e.second;
				}

				pos += len;
				str = match.suffix();
			}
		}

		offset += token.size();
	}
	
}

Replxx::ACTION_RESULT REPL::message( Replxx& replxx, std::string s, char32_t ) {
	replxx.invoke( Replxx::ACTION::CLEAR_SELF, 0 );
	replxx.print( "%s\n", s.c_str() );
	replxx.invoke( Replxx::ACTION::REPAINT, 0 );
	return ( Replxx::ACTION_RESULT::CONTINUE );
}