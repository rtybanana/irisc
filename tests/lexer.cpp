#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include "../src/lexer/lexer.h"
#include "../src/lexer/token.h"
#include "../src/error.h"

using namespace lexer;

/**
 * Tests for the Token class
 */
TEST_CASE("Token Tests", "[lexer, Token]") {
    
    /**
     * Test that each of the different instructions are all categorised correctly by the lexer
     */
    SECTION("Operations") {
        CHECK(Token(1, "mov").type() == BI_OPERAND);
        CHECK(Token(1, "mvn").type() == BI_OPERAND);
        CHECK(Token(1, "cmp").type() == BI_OPERAND);
        CHECK(Token(1, "cmn").type() == BI_OPERAND);
        CHECK(Token(1, "tst").type() == BI_OPERAND);
        CHECK(Token(1, "teq").type() == BI_OPERAND);

        CHECK(Token(1, "add").type() == TRI_OPERAND);
        CHECK(Token(1, "sub").type() == TRI_OPERAND);
        CHECK(Token(1, "and").type() == TRI_OPERAND);
        CHECK(Token(1, "orr").type() == TRI_OPERAND);
        CHECK(Token(1, "bic").type() == TRI_OPERAND);
        CHECK(Token(1, "eor").type() == TRI_OPERAND);
        CHECK(Token(1, "rsb").type() == TRI_OPERAND);
        CHECK(Token(1, "adc").type() == TRI_OPERAND);
        CHECK(Token(1, "sbc").type() == TRI_OPERAND);
        CHECK(Token(1, "rsc").type() == TRI_OPERAND);

        CHECK(Token(1, "b").type() == BRANCH);
        CHECK(Token(1, "bl").type() == BRANCH);
        CHECK(Token(1, "bx").type() == BRANCH);
    }

    /**
     * Test for all the different set flag suffixes on every instruction (not branches) to ensure
     * they are still all categorised correctly.
     */
    SECTION("Flags") {
        CHECK(Token(1, "movs").type() == BI_OPERAND);
        CHECK(Token(1, "mvns").type() == BI_OPERAND);
        CHECK(Token(1, "cmps").type() == BI_OPERAND);
        CHECK(Token(1, "cmns").type() == BI_OPERAND);
        CHECK(Token(1, "tsts").type() == BI_OPERAND);
        CHECK(Token(1, "teqs").type() == BI_OPERAND);

        CHECK(Token(1, "adds").type() == TRI_OPERAND);
        CHECK(Token(1, "subs").type() == TRI_OPERAND);
        CHECK(Token(1, "ands").type() == TRI_OPERAND);
        CHECK(Token(1, "orrs").type() == TRI_OPERAND);
        CHECK(Token(1, "bics").type() == TRI_OPERAND);
        CHECK(Token(1, "eors").type() == TRI_OPERAND);
        CHECK(Token(1, "rsbs").type() == TRI_OPERAND);
        CHECK(Token(1, "adcs").type() == TRI_OPERAND);
        CHECK(Token(1, "sbcs").type() == TRI_OPERAND);
        CHECK(Token(1, "rscs").type() == TRI_OPERAND);
    }

    /**
     * Test for all the different conditions on different instructions
     */
    SECTION("Conditions") {
        CHECK(Token(1, "moveq").type() == BI_OPERAND);
        CHECK(Token(1, "mvnne").type() == BI_OPERAND);
        CHECK(Token(1, "cmpcs").type() == BI_OPERAND);
        CHECK(Token(1, "cmncc").type() == BI_OPERAND);
        CHECK(Token(1, "tstmi").type() == BI_OPERAND);
        CHECK(Token(1, "teqpl").type() == BI_OPERAND);

        CHECK(Token(1, "addvs").type() == TRI_OPERAND);
        CHECK(Token(1, "subvc").type() == TRI_OPERAND);
        CHECK(Token(1, "andhi").type() == TRI_OPERAND);
        CHECK(Token(1, "orrls").type() == TRI_OPERAND);
        CHECK(Token(1, "bicge").type() == TRI_OPERAND);
        CHECK(Token(1, "eorlt").type() == TRI_OPERAND);
        CHECK(Token(1, "rsbgt").type() == TRI_OPERAND);
        CHECK(Token(1, "adcle").type() == TRI_OPERAND);
        CHECK(Token(1, "sbcal").type() == TRI_OPERAND);
        CHECK(Token(1, "rsc").type() == TRI_OPERAND);
    }

    /**
     * Test all the branch conditions in their own section because they are a little tricky 
     * due to the differing instruction length
     */
    SECTION("Branch Conditions") {
        CHECK(Token(1, "beq").type() == BRANCH);
        CHECK(Token(1, "bleq").type() == BRANCH);
        CHECK(Token(1, "bxeq").type() == BRANCH);

        CHECK(Token(1, "bne").type() == BRANCH);
        CHECK(Token(1, "blne").type() == BRANCH);
        CHECK(Token(1, "bxne").type() == BRANCH);

        CHECK(Token(1, "bcs").type() == BRANCH);
        CHECK(Token(1, "blcs").type() == BRANCH);
        CHECK(Token(1, "bxcs").type() == BRANCH);

        CHECK(Token(1, "bcc").type() == BRANCH);
        CHECK(Token(1, "blcc").type() == BRANCH);
        CHECK(Token(1, "bxcc").type() == BRANCH);

        CHECK(Token(1, "bmi").type() == BRANCH);
        CHECK(Token(1, "blmi").type() == BRANCH);
        CHECK(Token(1, "bxmi").type() == BRANCH);

        CHECK(Token(1, "bpl").type() == BRANCH);
        CHECK(Token(1, "blpl").type() == BRANCH);
        CHECK(Token(1, "bxpl").type() == BRANCH);

        CHECK(Token(1, "bvs").type() == BRANCH);
        CHECK(Token(1, "blvs").type() == BRANCH);
        CHECK(Token(1, "bxvs").type() == BRANCH);

        CHECK(Token(1, "bvc").type() == BRANCH);
        CHECK(Token(1, "blvc").type() == BRANCH);
        CHECK(Token(1, "bxvc").type() == BRANCH);

        CHECK(Token(1, "bhi").type() == BRANCH);
        CHECK(Token(1, "blhi").type() == BRANCH);
        CHECK(Token(1, "bxhi").type() == BRANCH);

        CHECK(Token(1, "bls").type() == BRANCH);
        CHECK(Token(1, "blls").type() == BRANCH);
        CHECK(Token(1, "bxls").type() == BRANCH);

        CHECK(Token(1, "bge").type() == BRANCH);
        CHECK(Token(1, "blge").type() == BRANCH);
        CHECK(Token(1, "bxge").type() == BRANCH);

        CHECK(Token(1, "blt").type() == BRANCH);
        CHECK(Token(1, "bllt").type() == BRANCH);
        CHECK(Token(1, "bxlt").type() == BRANCH); 

        CHECK(Token(1, "bgt").type() == BRANCH);
        CHECK(Token(1, "blgt").type() == BRANCH);
        CHECK(Token(1, "bxgt").type() == BRANCH);

        CHECK(Token(1, "ble").type() == BRANCH);
        CHECK(Token(1, "blle").type() == BRANCH);
        CHECK(Token(1, "bxle").type() == BRANCH);

        CHECK(Token(1, "bal").type() == BRANCH);
        CHECK(Token(1, "blal").type() == BRANCH);
        CHECK(Token(1, "bxal").type() == BRANCH);
    }

}

/**
 * Tests for the Lexer class
 */
TEST_CASE("Lexer Tests", "[lexer, Lexer]") {

    /**
     * Tests on lexically correct statements to ensure that the tokens are correctly identified
     */
    SECTION("Lexically Correct Statements") {

        SECTION("Statement #1") {
            std::string program = "mov r1, #1";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {BI_OPERAND, REGISTER, COMMA, IMM_DEC};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #2") {
            std::string program = "add r0, r1, #0xf";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {TRI_OPERAND, REGISTER, COMMA, REGISTER, COMMA, IMM_HEX};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #3") {
            std::string program = "rorsle pc, r12, #0b100101";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {SHIFT, REGISTER, COMMA, REGISTER, COMMA, IMM_BIN};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #4") {
            std::string program = "ble label_name";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {BRANCH, OP_LABEL};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #5") {
            std::string program = "label_name: .string \"ascii text\"";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {LABEL, DIRECTIVE, STRING};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #6") {
            std::string program = "mov r1, lr, lsl #027";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {BI_OPERAND, REGISTER, COMMA, REGISTER, COMMA, SHIFT, IMM_OCT};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #7") {
            std::string program = "rscsvc pc, lr, sp, ror #0d12";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {TRI_OPERAND, REGISTER, COMMA, REGISTER, COMMA, REGISTER, COMMA, SHIFT, IMM_DEC};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #8") {
            std::string program = "ldrge r0, =variable";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {LOAD_STORE, REGISTER, COMMA, VARIABLE};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

        SECTION("Statement #9") {
            std::string program = "str r0, [sp, #4]!";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            std::vector<Token> tokens = lexer.getTokens();
            std::vector<TOKEN> required = {LOAD_STORE, REGISTER, COMMA, OPEN_SQR, REGISTER, COMMA, IMM_DEC, CLOSE_SQR, EXCLAMATION};

            REQUIRE(tokens.size() == required.size());
            for (int i = 0; i < tokens.size(); i++) CHECK(tokens[i].type() == required[i]);
        }

    }

    /**
     * Tests for correctly reported lexical errors in ill-formed statements and
     */
    SECTION("Lexical Errors") {

        SECTION("Single Lines") {

            SECTION("Short input line") {
                std::string program = "mov r0, ##0";
                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 10.");
                    CHECK(e.constructHelper() == "1: mov r0, #\e[1;3;4m#\e[0m0");
                    CHECK(e.lineNumber() == 1);
                    CHECK(e.symbolIndex() == 9);
                }
            }

            SECTION("Long input line") {
                std::string program = "mov r0, r0, r0, r0, r0, r0, r0, r0, ???, r0, r0, r0, r0, r0, r0, r0, #0";
                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 37.");
                    CHECK(e.constructHelper() == "1: mov r0, r0, r0, r0, r0, r0, r0, r0, \e[1;3;4m?\e[0m??, r0, r0, r0, r0, r0, r0, r0, #0");
                    CHECK(e.lineNumber() == 1);
                    CHECK(e.symbolIndex() == 36);
                }
            }

            SECTION("Very long input line") {
                std::string program = "mov r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, ~r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, #0";
                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 149.");
                    CHECK(e.constructHelper() == "1: mov r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, \e[1;3;4m~\e[0mr0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, r0, #0");
                    CHECK(e.lineNumber() == 1);
                    CHECK(e.symbolIndex() == 148);
                }
            }
        }

        SECTION("Multiple Lines") {

            SECTION("1 in 100 lines") {
                std::string good = "mov r0, #0\n";
                std::string bad = "@mov r0, #0\n";
                std::string program;
                for (int i = 0; i < 100; i++) {
                    if (i == 37) program += bad;
                    else program += good;
                }

                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 1.");
                    CHECK(e.constructHelper() == "38: \e[1;3;4m@\e[0mmov r0, #0");
                    CHECK(e.lineNumber() == 38);
                    CHECK(e.symbolIndex() == 0);
                }
            }

            SECTION("1 in 1000 lines") {
                std::string good = "mov r4, r0\n";
                std::string bad = "mov ???, #0x0f\n";
                std::string program;
                for (int i = 0; i < 1000; i++) {
                    if (i == 632) program += bad;
                    else program += good;
                }

                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 5.");
                    CHECK(e.constructHelper() == "633: mov \e[1;3;4m?\e[0m??, #0x0f");
                    CHECK(e.lineNumber() == 633);
                    CHECK(e.symbolIndex() == 4);
                }
            }

            SECTION("100 in 100 lines") {
                std::string bad1 = "%% r?4, r0\n";
                std::string bad2 = "mov ~~, lr\n";
                std::string program;
                for (int i = 0; i < 100; i++) {
                    if (i % 2 == 0) program += bad1;
                    else program += bad2;
                }

                REQUIRE_THROWS_AS(Lexer(program), LexicalError);

                try {
                    Lexer lexer(program);
                }
                catch (const LexicalError &e){
                    CHECK(e.msg() == "Invalid token starting at position 1.");
                    CHECK(e.constructHelper() == "1: \e[1;3;4m%\e[0m% r?4, r0");
                    CHECK(e.lineNumber() == 1);
                    CHECK(e.symbolIndex() == 0);
                }
            }
        }
    }
}
