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
#include "constants.h"
#include <vector>
#include <map>
#include <variant>

namespace syntax {

  class Node {
    public:
      Node();
      Node(std::vector<lexer::Token>);
      Node(std::vector<lexer::Token>, unsigned int);
      std::vector<lexer::Token> statement() const { return _statement; };
      // FAMILY family() const { return _family; };
      std::string toString();
      virtual ~Node();

    protected:
      std::vector<lexer::Token> _statement;
      unsigned int currentToken = 0;
      // FAMILY _family;
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
      virtual std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() = 0;
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
      std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() override;
      std::tuple<OPERATION, CONDITION, std::variant<REGISTER, std::string>> unpack() const { return {_op, _cond, _Rd}; };

    protected:
      std::variant<REGISTER, std::string> _Rd;
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
      std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() override;
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
      std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() override;
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
      std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() override;
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
      std::tuple<uint32_t, std::vector<std::tuple<std::string, std::string, int>>> assemble() override;
      
    protected:
      SIZE _size;
      REGISTER _Rd;
      REGISTER _Rn;
      std::variant<std::monostate, REGISTER, int> _Rs;

    private:
      std::variant<std::monostate, REGISTER, int> parseRegOrImm();
  };

  // Node which contains heap allocation information for user defined variables
  class DirectiveNode : public Node {
    public:
      DirectiveNode(std::vector<lexer::Token>);
      DIRECTIVE directive;
      bool isText() const { return directive == TEXT; };
      bool isData() const { return directive == DATA; };  
      bool isGlobal() const { return directive == GLOBAL; };     
  };

  // Node which contains heap allocation information for user defined variables
  class AllocationNode : public Node {
    public:
      AllocationNode(std::vector<lexer::Token>);
      std::string identifier() const { return _identifier; };
      std::variant<size_t, uint8_t, uint16_t, uint32_t, std::string> value() const { return _value; };
      std::string printValue() const;
      
    protected:
      std::string _identifier;
      std::variant<size_t, uint8_t, uint16_t, uint32_t, std::string> _value;
      lexer::Token makeImmediate(lexer::Token);
  };

  // Node which contains heap allocation information for user defined variables
  class LabelNode : public Node {
    public:
      LabelNode(std::vector<lexer::Token>);
      std::string identifier() const { return _identifier; };
      
    protected:
      std::string _identifier;
  };
}

#endif //IRISC_SYNTAX_H
