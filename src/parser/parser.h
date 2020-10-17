/**
 * @file parser.h
 * Contains the implementation of the parser which takes tokens and ensures that they 
 * obey the syntax rules of ARMv7 assembly language
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_PARSER_H
#define IRISC_PARSER_H

#include "../lexer/lexer.h"
#include "syntax.h"

namespace parser {

  class Parser {
    public:
      Parser(lexer::Lexer&);
      lexer::Lexer& lexer;
      // lexer::Token currentToken;
      // lexer::Token nextToken;
      // void advanceToken();

      syntax::InstructionNode* parseSingle();
    
    // private:
    //   syntax::Node parseSingle();
    //   syntax::Node parseMultiple();
  };
}

#endif //IRISC_PARSER_H