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

  // OPCODES
  enum OPERATION {
    // arithmetic operations (shifts assemble to a MOV opcode with an additional shift)
    AND =  0,  EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN,
    // load/store instructions
    LDR = 16,  STR, 
    // branch instructions
    B   = 32,  BL, BX
  };

  static std::map<std::string, OPERATION> opMap {
    // arithmetic instructions
    {"and", AND}, {"eor", EOR}, {"sub", SUB}, {"rsb", RSB},
    {"add", ADD}, {"adc", ADC}, {"sbc", SBC}, {"rsc", RSC},
    {"tst", TST}, {"teq", TEQ}, {"cmp", CMP}, {"cmn", CMN},
    {"orr", ORR}, {"mov", MOV}, {"bic", BIC}, {"mvn", MVN},
    
    //load/store instructions
    {"ldr", LDR}, {"str", STR},

    // branch instructions
    {"bx",  BX }, {"bl",  BL }, { "b",  B  }
  };

  // CONDITION codes
  enum CONDITION {
    EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL
  };

  static std::map<std::string, CONDITION> condMap {
    {"eq", EQ}, {"ne", NE}, {"cs", CS}, {"cc", CC},
    {"mi", MI}, {"pl", PL}, {"vs", VS}, {"vc", VC},
    {"hi", HI}, {"ls", LS}, {"ge", GE}, {"lt", LT}, 
    {"gt", GT}, {"le", LE}, {"al", AL}, { "" , AL}
  };

  // REGISTER operands
  enum REGISTER {
    R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC
  };

  static std::map<std::string, REGISTER> regMap {
    { "r0", R0 }, { "r1", R1 }, { "r2", R2 }, { "r3", R3 },
    { "r4", R4 }, { "r5", R5 }, { "r6", R6 }, { "r7", R7 },
    { "r8", R8 }, { "r9", R9 }, {"r10", R10}, {"r11", R11}, 
    {"r12", R12}, { "sp", SP }, { "lr", LR }, { "pc", PC }
  };

  // SHIFT operations
  enum SHIFT {
    LSL, LSR, ASR, ROR
  };

  static std::map<std::string, SHIFT> shiftMap {
    {"lsl", LSL}, {"lsr", LSR}, {"asr", ASR}, {"ror", ROR}
  };

  // LOAD/STORE sizes
  enum SIZE {
    BYTE, HALFWORD, WORD
  };

  static std::map<std::string, SIZE> sizeMap {
    { "b", BYTE }, { "h", HALFWORD }
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
      uint32_t parseImmediate(lexer::Token, unsigned int);
      uint32_t parseImmediate(lexer::Token, unsigned int, unsigned int&);

    private:
      uint64_t parseImmediate(lexer::Token);
  };

  class InstructionNode : public Node {
    public:
      InstructionNode(std::vector<lexer::Token>);
      virtual uint32_t assemble() = 0;
      OPERATION op() const { return _op; };
      CONDITION cond() const { return _cond; };
      bool setFlags() const { return _setFlags; };

    protected:
      OPERATION _op;
      CONDITION _cond;
      bool _setFlags;

      std::tuple<std::string, std::string, std::string> splitOpCode(lexer::Token);
  };

  class BranchNode : public InstructionNode {
    public:
      BranchNode(std::vector<lexer::Token>);
      uint32_t assemble() override;

    protected:
      std::variant<std::monostate, REGISTER, std::string> _Rd;
      uint32_t _offset;
  };

  class FlexOperand : public Node {
    public: 
      FlexOperand();
      FlexOperand(std::vector<lexer::Token>, unsigned int);
      std::variant<std::monostate, REGISTER, int> Rm() const { return _Rm; };
      std::variant<std::monostate, REGISTER, int> Rs() const { return _Rs; };
      SHIFT shift() const { return _shift; };
      unsigned int immShift() const { return _immShift; };
      std::tuple<std::variant<std::monostate, REGISTER, int>, 
                 std::optional<SHIFT>, 
                 std::variant<std::monostate, REGISTER, int>, 
                 unsigned int> 
        unpack() const { return {_Rm, _shift, _Rs, _immShift}; };

      bool isReg() const { return _Rm.index() == 1; };
      bool isImm() const { return _Rm.index() == 2; };
      bool shifted() const { return _Rs.index() > 0; };
      bool shiftedByReg() const { return _Rs.index() == 1; };
      bool shiftedByImm() const { return _Rs.index() == 2; };

    protected:
      std::variant<std::monostate, REGISTER, int> _Rm;
      std::variant<std::monostate, REGISTER, int> _Rs;
      SHIFT _shift;
      unsigned int _immShift = 0;
    
    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm(unsigned int immBits = 8);
      void parseShift();
  };

  class BiOperandNode : public InstructionNode {
    public:
      BiOperandNode(std::vector<lexer::Token>);
      uint32_t assemble() override;
      REGISTER Rd() const { return _Rd; };
      FlexOperand flex() const { return _flex; };
      std::tuple<OPERATION, CONDITION, bool, REGISTER, FlexOperand> unpack() const { return {_op, _cond, _setFlags, _Rd, _flex}; };

    protected:
      REGISTER _Rd;
      FlexOperand _flex;
  };

  class TriOperandNode : public InstructionNode {
    public:
      TriOperandNode(std::vector<lexer::Token>);
      unsigned int assemble() override;
      REGISTER Rd() const { return _Rd; };
      REGISTER Rn() const { return _Rn; };
      FlexOperand flex() const { return _flex; };
      std::tuple<OPERATION, CONDITION, bool, REGISTER, REGISTER, FlexOperand> unpack() const { return {_op, _cond, _setFlags, _Rd, _Rn, _flex}; };

    protected:
      REGISTER _Rd;
      REGISTER _Rn;
      FlexOperand _flex;
  };

  class ShiftNode : public InstructionNode {
    public:
      ShiftNode(std::vector<lexer::Token>);
      unsigned int assemble() override;
      REGISTER Rd() const { return _Rd; };
      REGISTER Rn() const { return _Rn; };
      std::variant<std::monostate, REGISTER, int> Rs() const { return _Rs; };
      std::tuple<OPERATION, CONDITION, bool, REGISTER, REGISTER, std::variant<std::monostate, REGISTER, int>> unpack() const { return {_op, _cond, _setFlags, _Rd, _Rn, _Rs}; };

    protected:
      REGISTER _Rd;
      REGISTER _Rn;
      std::variant<std::monostate, REGISTER, int> _Rs;
      unsigned int _immShift = 0;

    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm();
  };

  class LoadStoreNode : public InstructionNode {
    public:
      LoadStoreNode(std::vector<lexer::Token>);
      unsigned int assemble() override;

    protected:
      SIZE _size;
      REGISTER _Rd;
      REGISTER _Rn;
      std::variant<std::monostate, REGISTER, int> _Rs;

    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm();
  };
}

#endif //IRISC_SYNTAX_H
