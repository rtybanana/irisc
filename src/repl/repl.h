#ifndef IRISC_REPL_H
#define IRISC_REPL_H

#include <thread>
#include "replxx.hxx"
#include "../emulator/emulator.h"
#include "constants.h"

namespace irepl {
  // prototypes


  class REPL {
    private: 
      replxx::Replxx& rx;
      std::string history_file;
      std::vector<std::string> ops;
      std::vector<std::string> regs;
      std::vector<std::string> conds;
      void fetchTokens();
    
    public:
      REPL(replxx::Replxx&);
      void loop(vm::Emulator);
      replxx::Replxx::completions_t hook_completion(std::string const& context, int& contextLen);
      replxx::Replxx::hints_t hook_hint(std::string const& context, int& contextLen, replxx::Replxx::Color& color);
      void hook_color(std::string const& str, replxx::Replxx::colors_t& colors, std::vector<std::pair<std::string, replxx::Replxx::Color>> const& user_data);
      replxx::Replxx::ACTION_RESULT message(replxx::Replxx&, std::string, char32_t);
  };
}

#endif //IRISC_REPL_H