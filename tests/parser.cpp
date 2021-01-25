#include <catch2/catch_all.hpp>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/parser/syntax.h"

using namespace lexer;
using namespace syntax;

/**
 * Tests for the Syntax namespace
 */
TEST_CASE("Syntax Tests", "[parser, syntax]") {

  SECTION("Syntactically Correct Statements") {

        SECTION("Statement #1") {
            std::string program = "mov r1, #1";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            BiOperandNode* node;
            REQUIRE_NOTHROW(node = new BiOperandNode(lexer.getTokens()));

            CHECK(node->op() == MOV);
            CHECK(node->setFlags() == false);
            CHECK(node->cond() == AL);
            CHECK(node->Rd() == R1);
            CHECK(node->flex().isImm());
            CHECK(std::get<int>(node->flex().Rm()) == 1);
        }

        SECTION("Statement #2") {
            std::string program = "add r0, r1, #0xf";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            TriOperandNode* node;
            REQUIRE_NOTHROW(node = new TriOperandNode(lexer.getTokens()));

            CHECK(node->op() == ADD);
            CHECK(node->setFlags() == false);
            CHECK(node->cond() == AL);
            CHECK(node->Rd() == R0);
            CHECK(node->Rn() == R1);
            CHECK(node->flex().isImm());
            CHECK(std::get<int>(node->flex().Rm()) == 15);
        }

        SECTION("Statement #3") {
            std::string program = "rorsle pc, r12, #0b10011";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            ShiftNode* node;
            REQUIRE_NOTHROW(node = new ShiftNode(lexer.getTokens()));

            CHECK(node->shiftOp() == ROR);
            CHECK(node->setFlags() == true);
            CHECK(node->cond() == LE);
            CHECK(node->Rd() == PC);
            CHECK(node->Rn() == R12);
            CHECK(std::get<int>(node->Rs()));
            CHECK(std::get<int>(node->Rs()) == 19);
        }

        SECTION("Statement #4") {
            std::string program = "bpl label_name";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            BranchNode* node;
            REQUIRE_NOTHROW(node = new BranchNode(lexer.getTokens()));

            CHECK(node->op() == B);
            CHECK(node->setFlags() == false);
            CHECK(node->cond() == PL);
            CHECK(std::get<std::string>(node->Rd()) == "label_name");
        }

        SECTION("Statement #5") {
            std::string program = "label_name: .string \"ascii text\"";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            AllocationNode* node;
            REQUIRE_THROWS_AS(node = new AllocationNode(lexer.getTokens()), DeveloperError);
        }

        SECTION("Statement #6") {
            std::string program = "mov r1, lr, lsl #027";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            BiOperandNode* node;
            REQUIRE_NOTHROW(node = new BiOperandNode(lexer.getTokens()));

            CHECK(node->op() == MOV);
            CHECK(node->setFlags() == false);
            CHECK(node->cond() == AL);
            CHECK(node->Rd() == R1);
            CHECK(node->flex().isReg());
            CHECK(std::get<syntax::REGISTER>(node->flex().Rm()) == LR);
            CHECK(node->flex().shifted());
            CHECK(node->flex().shiftedByImm());
            CHECK(node->flex().shift() == LSL);
            CHECK(std::get<int>(node->flex().Rs()) == 23);
            
        }

        SECTION("Statement #7") {
            std::string program = "rscsvc pc, lr, sp, ror #0d12";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            TriOperandNode* node;
            REQUIRE_NOTHROW(node = new TriOperandNode(lexer.getTokens()));

            CHECK(node->op() == RSC);
            CHECK(node->setFlags() == true);
            CHECK(node->cond() == VC);
            CHECK(node->Rd() == PC);
            CHECK(node->Rn() == LR);
            CHECK(node->flex().isReg());
            CHECK(std::get<syntax::REGISTER>(node->flex().Rm()) == SP);
            CHECK(node->flex().shifted());
            CHECK(node->flex().shiftedByImm());
            CHECK(node->flex().shift() == ROR);
            CHECK(std::get<int>(node->flex().Rs()) == 12);
        }

        SECTION("Statement #8") {
            std::string program = "asrcc r0, r12, r7";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            ShiftNode* node;
            REQUIRE_NOTHROW(node = new ShiftNode(lexer.getTokens()));

            CHECK(node->shiftOp() == ASR);
            CHECK(node->setFlags() == false);
            CHECK(node->cond() == CC);
            CHECK(node->Rd() == R0);
            CHECK(node->Rn() == R12);
            CHECK(std::get<syntax::REGISTER>(node->Rs()));
            CHECK(std::get<syntax::REGISTER>(node->Rs()) == R7);
        }

        SECTION("Statement #9") {
            std::string program = "ldrge r0, =variable";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            LoadStoreNode* node;
            REQUIRE_THROWS_AS(node = new LoadStoreNode(lexer.getTokens()), DeveloperError);
        }

        SECTION("Statement #10") {
            std::string program = "str r0, [sp, #4]!";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            LoadStoreNode* node;
            REQUIRE_THROWS_AS(node = new LoadStoreNode(lexer.getTokens()), DeveloperError);
        }

        SECTION("Statement #11") {
            std::string program = "label:";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            LabelNode* node;
            REQUIRE_NOTHROW(node = new LabelNode(lexer.getTokens()));

            CHECK(node->identifier() == "label");
        }

    }

    SECTION("Syntax Errors") {

        SECTION("Error #1") {

        }
    }

}

TEST_CASE("Parser Tests", "[parser]") {

    SECTION("Parse Single") {

        SECTION("Statement #1") {
            std::string program = "mov r1, #1";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<BiOperandNode*>(node));
        }

        SECTION("Statement #2") {
            std::string program = "add r0, r1, #0xf";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<TriOperandNode*>(node));
        }

        SECTION("Statement #3") {
            std::string program = "rorsle pc, r12, #0b10011";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<ShiftNode*>(node));
        }

        SECTION("Statement #4") {
            std::string program = "ble label_name";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<BranchNode*>(node));
        }

        SECTION("Statement #5") {
            std::string program = "label_name: .string \"ascii text\"";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_THROWS_AS(node = parser.parseSingle(), DeveloperError);
            // CHECK(dynamic_cast<AllocationNode*>(node));
        }

        SECTION("Statement #6") {
            std::string program = "mov r1, lr, lsl #027";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<BiOperandNode*>(node));
        }

        SECTION("Statement #7") {
            std::string program = "rscsvc pc, lr, sp, ror #0d12";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<TriOperandNode*>(node));
        }

        SECTION("Statement #8") {
            std::string program = "asrcc r0, r12, r7";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<ShiftNode*>(node));
        }

        SECTION("Statement #9") {
            std::string program = "ldrge r0, =variable";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_THROWS_AS(node = parser.parseSingle(), DeveloperError);
            // CHECK(dynamic_cast<LoadStoreNode*>(node));
        }

        SECTION("Statement #10") {
            std::string program = "str r0, [sp, #4]!";
            REQUIRE_NOTHROW(Lexer(program));
            
            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_THROWS_AS(node = parser.parseSingle(), DeveloperError);
            // CHECK(dynamic_cast<LoadStoreNode*>(node));
        }

        SECTION("Statement#11") {
            std::string program = "label:";
            REQUIRE_NOTHROW(Lexer(program));

            Lexer lexer(program);
            Parser parser(lexer);
            Node* node;
            REQUIRE_NOTHROW(node = parser.parseSingle());
            CHECK(dynamic_cast<LabelNode*>(node));
        } 

    }


    SECTION("Parse Multiple") {

        SECTION("Syntactically Correct Programs") {
            
            // simple procedural program
            SECTION("Program #1") {
                std::string program = 
                    "mov r0, #6\n"
                    "mov r1, #17\n"
                    "add r0, r0, r1"
                ;

                REQUIRE_NOTHROW(Lexer(program));
                Lexer lexer(program);
                Parser parser(lexer);

                std::vector<Node*> nodes;
                REQUIRE_NOTHROW(nodes = parser.parseMultiple());

                CHECK(dynamic_cast<BiOperandNode*>(nodes[0]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[1]));
                CHECK(dynamic_cast<TriOperandNode*>(nodes[2]));
            }

            // loop control structure
            SECTION("Program #2") {
                std::string program = 
                    "loop:\n"
                    "\tmvn r0, #6\n"
                    "\tmvn r1, #17\n"
                    "\tsubs r0, r0, r1\n"
                    "\tb loop"
                ;

                REQUIRE_NOTHROW(Lexer(program));
                Lexer lexer(program);
                Parser parser(lexer);

                std::vector<Node*> nodes;
                REQUIRE_NOTHROW(nodes = parser.parseMultiple());
                
                CHECK(dynamic_cast<LabelNode*>(nodes[0]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[1]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[2]));
                CHECK(dynamic_cast<TriOperandNode*>(nodes[3]));
                CHECK(dynamic_cast<BranchNode*>(nodes[4]));
            }

            // function declaration and branching with bl
            SECTION("Program #3") {
                std::string program = 
                    "add_fn:\n"
                    "\tadd r0, r0, r1\n"
                    "\tbx lr\n"
                    "\n"
                    "main:\n"
                    "\tmoval r0, #0d6\n"
                    "\tmov r1, #0x11\n"
                    "\tbl add_fn"
                ;

                REQUIRE_NOTHROW(Lexer(program));
                Lexer lexer(program);
                Parser parser(lexer);

                std::vector<Node*> nodes;
                REQUIRE_NOTHROW(nodes = parser.parseMultiple());
                
                CHECK(dynamic_cast<LabelNode*>(nodes[0]));
                CHECK(dynamic_cast<TriOperandNode*>(nodes[1]));
                CHECK(dynamic_cast<BranchNode*>(nodes[2]));
                CHECK(dynamic_cast<LabelNode*>(nodes[3]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[4]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[5]));
                CHECK(dynamic_cast<BranchNode*>(nodes[6]));
            }
            
            // comparisions and conditional execution
            SECTION("Program #4") {
                std::string program = 
                    "main:\n"
                    "\tmoval r0, #0d6\n"
                    "\tmov r1, #0x11\n"
                    "\n"
                    "\tcmp r0, #0b110\n"
                    "\taddeq r0, r0, r1\n"
                    "\tsubne r0, r0, r1\n"
                ;

                REQUIRE_NOTHROW(Lexer(program));
                Lexer lexer(program);
                Parser parser(lexer);

                std::vector<Node*> nodes;
                REQUIRE_NOTHROW(nodes = parser.parseMultiple());
                
                CHECK(dynamic_cast<LabelNode*>(nodes[0]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[1]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[2]));
                CHECK(dynamic_cast<BiOperandNode*>(nodes[3]));
                CHECK(dynamic_cast<TriOperandNode*>(nodes[4]));
                CHECK(dynamic_cast<TriOperandNode*>(nodes[5]));
            }
        }
    }
}