#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include <string>
#include "../src/lexer/lexer.h"
#include "../src/lexer/token.h"
#include "../src/error.h"

TEST_CASE("Lexically Valid Strings", "[lexer]") {

    SECTION("Token Validation") {
        
        SECTION("Operations") {
            std::string program = "mov ADD Bx cmP rsb Tst BIC bl oRr SUb teQ";
            REQUIRE_NOTHROW(lexer::Lexer(program));

            lexer::Lexer lexer(program);
            std::vector<lexer::Token> tokens = lexer.getTokens();
            CHECK(tokens[0].type()  == lexer::BI_OPERAND);
            CHECK(tokens[1].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[2].type()  == lexer::BRANCH);
            CHECK(tokens[3].type()  == lexer::BI_OPERAND);
            CHECK(tokens[4].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[5].type()  == lexer::BI_OPERAND);
            CHECK(tokens[6].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[7].type()  == lexer::BRANCH);
            CHECK(tokens[8].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[9].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[10].type() == lexer::BI_OPERAND);
        }

        SECTION("Conditions and Set Flags") {
            std::string program = "moveq sUbspl ADDNE blle cmpge ANDsal bHI bicvc ORrSVs tEqSmI";
            REQUIRE_NOTHROW(lexer::Lexer(program));

            lexer::Lexer lexer(program);
            std::vector<lexer::Token> tokens = lexer.getTokens();
            CHECK(tokens[0].type()  == lexer::BI_OPERAND);
            CHECK(tokens[1].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[2].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[3].type()  == lexer::BRANCH);
            CHECK(tokens[4].type()  == lexer::BI_OPERAND);
            CHECK(tokens[5].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[6].type()  == lexer::BRANCH);
            CHECK(tokens[7].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[8].type()  == lexer::TRI_OPERAND);
            CHECK(tokens[9].type()  == lexer::BI_OPERAND);
        }

    }

}

TEST_CASE("Lexical Errors", "[lexer]" ) {

    SECTION("Single Lines") {

        SECTION("Short input line") {
            std::string program = "mov r0, ##0";
            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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
            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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
            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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

            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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

            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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

            REQUIRE_THROWS_AS(lexer::Lexer(program), LexicalError);

            try {
                lexer::Lexer lexer(program);
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