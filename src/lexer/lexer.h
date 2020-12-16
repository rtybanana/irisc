/**
 * @file lexer.h
 * Contains the implementation lexer which uses a DFA to classify token types.
 * @author Rory Pinkney
 * @date 6/10/20
 */

#ifndef IRISC_LEXER_H
#define IRISC_LEXER_H

#include <string>
#include <vector>
#include "token.h"

namespace lexer {

  enum TRANSITION {
    HASHTAG = 0,
    EQUALS,
    ALPHA_B,
    ALPHA_D,
    ALPHA_X,
    A_TO_F,
    G_TO_Z,
    ZERO,
    ONE,
    TWO_SEVEN,
    EIGHT_NINE,
    UNDERSCORE,
    COLON,
    PUNCT,
    ENDLINE,
    OTHER
  };
  
  class Lexer {
    private:
      const unsigned int e = 10;
      const unsigned int f_states[11] =  
        /*               S0  S1  S2  S3  S4  S5  S6  S7  s8  S9  Se */
                        { 0,  1,  1,  1,  1,  0,  1,  1,  1,  1,  0 };
      const unsigned int t_table[16][10] = {
        /*               S0  S1  S2  S3  S4  S5  S6  S7  S8  S9 */
        /*   '#'   */  {  5,  e,  e,  e,  e,  e,  e,  e,  e,  e  },
        /*   '='   */  {  2,  e,  e,  e,  e,  e,  e,  e,  e,  e  },
        /*   'b'   */  {  1,  1,  2,  e,  e,  e,  8,  7,  e,  e  },
        /*   'd'   */  {  1,  1,  2,  e,  e,  e,  4,  7,  e,  e  },
        /*   'x'   */  {  1,  1,  2,  e,  e,  e,  7,  e,  e,  e  },
        /*   A-f   */  {  1,  1,  2,  e,  e,  e,  e,  7,  e,  e  },
        /*   G-z   */  {  1,  1,  2,  e,  e,  e,  e,  e,  e,  e  },
        /*   '0'   */  {  e,  1,  2,  e,  4,  6,  6,  7,  8,  e  },
        /*   '1'   */  {  e,  1,  2,  e,  4,  4,  6,  7,  8,  e  },
        /*   2-7   */  {  e,  1,  2,  e,  4,  4,  6,  7,  e,  e  },
        /*   8+9   */  {  e,  1,  2,  e,  4,  4,  e,  7,  e,  e  },
        /*   '_'   */  {  2,  2,  2,  e,  e,  e,  e,  e,  e,  e  },
        /*   ':'   */  {  e,  3,  3,  e,  e,  e,  e,  e,  e,  e  },
        /*  PUNCT  */  {  9,  e,  e,  e,  e,  e,  e,  e,  e,  e  },
        /* ENDLINE */  {  9,  e,  e,  e,  e,  e,  e,  e,  e,  e  },
        /*  OTHER  */  {  e,  e,  e,  e,  e,  e,  e,  e,  e,  e  }
      };

      unsigned int current_token = 0;
      std::vector<Token> tokens;

      int nextState(int, char);
      bool hasToken(std::string&, unsigned int&);
      Token nextToken(std::string&, unsigned int&, unsigned int&);

    public:
      Lexer(std::string&);
      
      Token peekToken();
      Token nextToken();
      std::vector<Token> getTokens();
      ~Lexer();
  };
}

#endif //IRISC_LEXER_H