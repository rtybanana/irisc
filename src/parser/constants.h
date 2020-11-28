/**
 * @file syntax/constants.h
 * Holds various enums useful for translating tokens into machine code. Also holds maps which are used as 
 * look up tables to quickly generate explanations and titles for syntax and machine code.
 * @author Rory Pinkney
 * @date 28/10/20
 */

#ifndef IRISC_SYNTAX_CONSTANTS_H
#define IRISC_SYNTAX_CONSTANTS_H

#include <map>

namespace syntax {

  //******************************************************************************************
  // OPERATIONS
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

  static std::map<OPERATION, std::string> opTitle {
    {AND, "Bitwise AND"}, {EOR, "Bitwise XOR"}, 
    {SUB, "Subtraction"}, {RSB, "Reverse Subtraction"},
    {ADD, "Addition"}, {ADC, "Add with Carry"},
    {SBC, "Subtract with Carry"}, {RSC, "Reverse Subtraction with Carry"},
    {TST, "Test"}, {TEQ, "Test Equivalence"},
    {CMP, "Compare"}, {CMN, "Compare Negative"},
    {ORR, "Bitwise OR"}, {MOV, "Move"},
    {BIC, "Bit Clear"}, {MVN, "Move Negative"}
  };

  static std::map<OPERATION, std::string> opExplain {
    {AND, "Performs a bitwise AND operation and stores the result."}, 
    {EOR, "Performs a bitwise exclusive OR operation and stores the result."}, 
    {SUB, "Performs an arithmetic subtraction from left to right and stores the result."}, 
    {RSB, "Performs an arithmetic subtraction from right to left and stores the result."},
    {ADD, "Performs an arithmetic addition and stores the result."}, 
    {ADC, "???"},
    {SBC, "???"}, 
    {RSC, "???"},
    {TST, "Performs a bitwise AND operation, sets the CPSR flags and discards the result."},
    {TEQ, "Performs a bitwise XOR operation, sets the CPSR flags and discards the result."},
    {CMP, "Performs an arithmetic subtraction, sets the CPSR flags and discards the result."},
    {CMN, "Performs an arithmetic addition, sets the CPSR flags and discards the result."},
    {ORR, "Performs a bitwise OR operation and stores the result."},
    {MOV, "Stores the second operand value in the destination register."},
    {BIC, "Performs a bitwise AND operation with the complement of the second operand."},
    {MVN, "Stores the additive inverse of the second operand value in the destination register."}
  };


  //*******************************************************************************************
  // CONDITIONS
  enum CONDITION {
    EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL
  };

  static std::map<std::string, CONDITION> condMap {
    {"eq", EQ}, {"ne", NE}, {"cs", CS}, {"cc", CC},
    {"mi", MI}, {"pl", PL}, {"vs", VS}, {"vc", VC},
    {"hi", HI}, {"ls", LS}, {"ge", GE}, {"lt", LT}, 
    {"gt", GT}, {"le", LE}, {"al", AL}, { "" , AL}
  };

  static std::map<CONDITION, std::string> condTitle {
    {EQ, "Equal"}, {NE, "Not Equal"}, 
    {CS, "Unsigned More Than or Equal to"}, {CC, "Unsigned Less Than"},
    {MI, "Negative"}, {PL, "Positive or Zero"},
    {VS, "Overflow"}, {VC, "No Overflow"},
    {HI, "Unsigned More Than"}, {LS, "Unsigned Less Than or Equal to"},
    {GE, "Signed More Than or Equal to"}, {LT, "Signed Less Than"},
    {GT, "Signed More Than"}, {LE, "Signed Less Than or Equal to"},
    {AL, "Always"}
  };

  static std::map<CONDITION, std::string> condExplain {
    {EQ, "The instruction is only executed if the zero flag (Z) is set."}, 
    {NE, "The instruction is only executed if the zero flag (Z) is clear."}, 
    {CS, "The instruction is only executed if the carry flag (C) is set."}, 
    {CC, "The instruction is only executed if the carry flag (C) is clear."},
    {MI, "The instruction is only executed if the negative flag (N) is set."}, 
    {PL, "The instruction is only executed if the negative flag (N) is clear."},
    {VS, "The instruction is only executed if the overflow flag (V) is set."}, 
    {VC, "The instruction is only executed if the overflow flag (V) is clear."},
    {HI, "The instruction is only executed if the carry flag (C) is set AND the zero flag (Z) is clear."},
    {LS, "The instruction is only executed if the carry flag (C) is clear OR the zero flag (Z) is set."},
    {GE, "The instruction is only executed if the negative flag (N) and the overflow flag (V) are the same."},
    {LT, "The instruction is only executed if the negative flag (N) and the overflow flag (V) are different."},
    {GT, "The instruction is only executed if the zero flag (Z) is clear and the negative (N) and overflow (V) flags are the same."},
    {LE, "The instruction is only executed if the zero flag (Z) is set and the negative (N) and overflow (V) flags are different."},
    {AL, "The instruction is executed unconditionally. This is the default condition."}
  };


  //*******************************************************************************************
  // REGISTERS
  enum REGISTER {
    R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC
  };

  static std::map<std::string, REGISTER> regMap {
    { "r0", R0 }, { "r1", R1 }, { "r2", R2 }, { "r3", R3 },
    { "r4", R4 }, { "r5", R5 }, { "r6", R6 }, { "r7", R7 },
    { "r8", R8 }, { "r9", R9 }, {"r10", R10}, {"r11", R11}, 
    {"r12", R12}, { "sp", SP }, { "lr", LR }, { "pc", PC }
  };

  static std::map<REGISTER, std::string> regTitle {
    {R0, "Register 0"}, {R1, "Register 1"}, {R2, "Register 2"}, {R3, "Register 3"}, 
    {R4, "Register 4"}, {R5, "Register 5"}, {R6, "Register 6"}, {R7, "Register 7"}, 
    {R8, "Register 8"}, {R9, "Register 9"}, {R10, "Register 10"}, {R11, "Register 11"},  
    {R12, "Register 12"}, {SP, "Stack Pointer"}, {LR, "Link Register"}, {PC, "Program Counter"}, 
  };

  static std::map<REGISTER, std::string> regExplain {
    {R0, "General Purpose Register\n"}, {R1, "General Purpose Register\n"}, 
    {R2, "General Purpose Register\n"}, {R3, "General Purpose Register\n"}, 
    {R4, "General Purpose Register\n"}, {R5, "General Purpose Register\n"}, 
    {R6, "General Purpose Register\n"}, {R7, "General Purpose Register\n"}, 
    {R8, "General Purpose Register\n"}, {R9, "General Purpose Register\n"}, 
    {R10, "General Purpose Register\n"}, {R11, "General Purpose Register\n"},  
    {R12, "General Purpose Register\n"}, 
    {SP, "The memory address for the top of the stack."}, 
    {LR, "The return address of the current function."}, 
    {PC, "The memory address of the current instruction."}, 
  };

  //**********************************************************************************************
  // SHIFTS
  enum SHIFT {
    LSL, LSR, ASR, ROR
  };

  static std::map<std::string, SHIFT> shiftMap {
    {"lsl", LSL}, {"lsr", LSR}, {"asr", ASR}, {"ror", ROR}
  };

  static std::map<SHIFT, std::string> shiftTitle {
    {LSL, "Logical Shift Left"}, {LSR, "Logical Shift Right"}, 
    {ASR, "Arithmetic Shift Right"}, {ROR, "Rotate Right"}
  };

  // LOAD/STORE sizes
  enum SIZE {
    BYTE, HALFWORD, WORD
  };

  static std::map<std::string, SIZE> sizeMap {
    { "b", BYTE }, { "h", HALFWORD }
  };


  //*******************************************************************************************
  // CPSR FLAGS
  static std::string flagsExplain[] = {
    "Do not set the CPSR flags based on the result of this instruction.",
    "Set the CPSR flags based on the result of this instruction."
  };
}

#endif // IRISC_SYNTAX_CONSTANTS_H