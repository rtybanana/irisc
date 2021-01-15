#include <catch2/catch_all.hpp>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/parser/syntax.h"

using namespace lexer;

/**
 * Tests for the Syntax namespace
 */
TEST_CASE("Syntax Tests") {

  SECTION("Lexically Correct Statements") {

        SECTION("Statement #1") {
            std::string program = "mov r1, #1";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::BiOperandNode(lexer.getTokens()));
        }

        SECTION("Statement #2") {
            std::string program = "add r0, r1, #0xf";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::TriOperandNode(lexer.getTokens()));
        }

        SECTION("Statement #3") {
            std::string program = "rorsle pc, r12, #0b100101";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::TriOperandNode(lexer.getTokens()));
        }

        SECTION("Statement #4") {
            std::string program = "ble label_name";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::BranchNode(lexer.getTokens()));
        }

        SECTION("Statement #5") {
            std::string program = "label_name: .string \"ascii text\"";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_THROWS_AS(syntax::AllocationNode(lexer.getTokens()), DeveloperError);
        }

        SECTION("Statement #6") {
            std::string program = "mov r1, lr, lsl #027";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::BiOperandNode(lexer.getTokens()));
        }

        SECTION("Statement #7") {
            std::string program = "rscsvc pc, lr, sp, ror #0d12";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_NOTHROW(syntax::TriOperandNode(lexer.getTokens()));
        }

        SECTION("Statement #8") {
            std::string program = "ldrge r0, =variable";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_THROWS_AS(syntax::LoadStoreNode(lexer.getTokens()), DeveloperError);
        }

        SECTION("Statement #9") {
            std::string program = "str r0, [sp, #4]!";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            REQUIRE_THROWS_AS(syntax::LoadStoreNode(lexer.getTokens()), DeveloperError);
        }

    }

}

TEST_CASE("Parser Tests", "[parser]") {

}