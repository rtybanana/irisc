/**
 * @file syntax.h
 * Contains the definitions of the nodes in the abstract syntax tree for ARMv7 assembly language
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_SYNTAX_H
#define IRISC_SYNTAX_H

#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include <vector>
#include <map>
// #include <optional>
#include <variant>


namespace syntax {
  enum OPERATION {
    // arithmetic operations (shifts assemble to a MOV opcode with an additional shift)
    AND =  0,  EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN,
    // load/store instructions
    LDR = 16,  STR, 
    // branch instructions
    B   = 32,  BL, BX
  };

  static std::map<std::string, OPERATION> opMap {
    {"mov", MOV}, {"cmp", CMP},
    {"add", ADD}, {"sub", SUB},
    {"ldr", LDR}, {"str", STR},
    // {"lsl", LSL}, {"lsr", LSR},
    {"bx" , BX }, {"bl" , BL },
    {"b"  , B  },
  };

  enum CONDITION {
    EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL
  };

  static std::map<std::string, CONDITION> condMap {
    {"eq", EQ}, {"ne", NE}, {"cs", CS}, {"cc", CC},
    {"mi", MI}, {"pl", PL}, {"vs", VS}, {"vc", VC},
    {"hi", HI}, {"ls", LS}, {"ge", GE}, {"lt", LT}, 
    {"gt", GT}, {"le", LE}, {"al", AL}, { "" , AL}
  };

  enum REGISTER {
    R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC
  };

  static std::map<std::string, REGISTER> regMap {
    { "r0", R0 }, { "r1", R1 }, { "r2", R2 }, { "r3", R3 },
    { "r4", R4 }, { "r5", R5 }, { "r6", R6 }, { "r7", R7 },
    { "r8", R8 }, { "r9", R9 }, {"r10", R10}, {"r11", R11}, 
    {"r12", R12}, { "sp", SP }, { "lr", LR }, { "pc", PC }
  };

  enum SHIFT {
    LSL, LSR, ASR, ROR
  };

  static std::map<std::string, SHIFT> shiftMap {
    { "lsl", LSL }, { "lsr", LSR }
  };

  class Node {
    public:
      Node();
      Node(std::vector<lexer::Token>);
      Node(std::vector<lexer::Token>, unsigned int);
      virtual ~Node();

    protected:
      std::vector<lexer::Token> statement;
      unsigned int currentToken = 0;
      lexer::Token nextToken();
      lexer::Token peekToken();
      bool hasToken();
      bool parseComma(lexer::Token);
      REGISTER parseRegister(lexer::Token);
      int parseImmediate(lexer::Token, unsigned int);
  };

  class InstructionNode : public Node {
    public:
      InstructionNode(std::vector<lexer::Token>);
      virtual unsigned int assemble() = 0;

    protected:
      OPERATION op;
      CONDITION cond;
      bool setFlags;

      std::tuple<std::string, std::string, std::string> splitOpCode(lexer::Token);
  };

  class BranchNode : public InstructionNode {
    public:
      BranchNode(std::vector<lexer::Token>);
      unsigned int assemble() override;

    protected:
      REGISTER Rd;
      std::string label = nullptr;
      unsigned int address;
  };

  class FlexOperand : public Node {
    public: 
      FlexOperand();
      FlexOperand(std::vector<lexer::Token>, unsigned int);
      std::variant<std::monostate, REGISTER, int> Rm() const { return _Rm; };
      std::variant<std::monostate, REGISTER, int> Rs() const { return _Rs; };
      SHIFT shift() const { return _shift; };

    protected:
      std::variant<std::monostate, REGISTER, int> _Rm;
      std::variant<std::monostate, REGISTER, int> _Rs;
      SHIFT _shift;
    
    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm(unsigned int immBits = 8);
      void parseShift();
  };

  class BiOperandNode : public InstructionNode {
    public:
      BiOperandNode(std::vector<lexer::Token>);
      unsigned int assemble() override;

    protected:
      REGISTER Rd;
      FlexOperand Rm;
  };

  class TriOperandNode : public InstructionNode {
    public:
      TriOperandNode(std::vector<lexer::Token>);
      unsigned int assemble() override;

    protected:
      REGISTER Rd;
      REGISTER Rn;
      FlexOperand Rm;
  };

  class ShiftOperation : public InstructionNode {
    public:
      ShiftOperation(std::vector<lexer::Token>);
      unsigned int assemble() override;

    protected:
      REGISTER Rd;
      REGISTER Rn;
      std::variant<std::monostate, REGISTER, int> Rs;


    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm();
  };
}

#endif //IRISC_SYNTAX_H
