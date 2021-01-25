#include <chrono>
#include <thread>

#include <catch2/catch_all.hpp>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/emulator/emulator.h"
#include "../src/emulator/gui/registers.h"
#include "../src/emulator/gui/memory.h"
#include "../src/emulator/gui/instruction.h"
#include "../src/emulator/gui/editor.h"

using namespace lexer;
using namespace syntax;
using namespace vm;

TEST_CASE("Registers Tests", "[emulator, registers]") {
    Registers registers(0, 0, true);

    registers[syntax::R0] = 6;
    CHECK(registers[0] == (uint32_t)6);

    registers[syntax::R12] = 12341512;
    CHECK(registers[12] == (uint32_t)12341512);

    registers[syntax::R12] = -999999;
    CHECK(registers[12] == (uint32_t)-999999);
}

TEST_CASE("Emulator Tests", "[emulator]") {

    SECTION("Syntactically Correct Statements") {

        Emulator emulator;

        SECTION("Statement #1") {
            std::string program = "mov r1, #1";
            REQUIRE_NOTHROW(emulator.execute(program));

            CHECK(emulator.registerValue(R1) == 1);
        }

        SECTION("Statement #2") {
            std::string program = "add r0, r1, #0xf";
            REQUIRE_NOTHROW(emulator.execute(program));
            
            CHECK(emulator.registerValue(R0) == 0xf);
        }

        SECTION("Statement #3") {
            std::string program = "rorsle pc, r12, #0b10011";
            REQUIRE_NOTHROW(emulator.execute(program));

            CHECK(emulator.registerValue(PC) == 0);                         // not executed
        }

        SECTION("Statement #4") {
            std::string program = "bpl label_name";
            REQUIRE_NOTHROW(emulator.execute(program));
        }

        SECTION("Statement #5") {
            std::string program = "label_name: .string \"ascii text\"";
            REQUIRE_NOTHROW(emulator.execute(program));
        }

        SECTION("Statement #6") {
            std::string program = "mov r1, lr, lsl #027";
            REQUIRE_NOTHROW(emulator.execute(program));

            CHECK(emulator.registerValue(R1) == 0); 
        }

        SECTION("Statement #7") {
            std::string program = "rscsvc pc, lr, sp, ror #0d12";
            REQUIRE_NOTHROW(emulator.execute(program));

            CHECK(emulator.registerValue(PC) == -1);
        }

        SECTION("Statement #8") {
            std::string program = "asrcc r0, r12, r7";
            REQUIRE_NOTHROW(emulator.execute(program));

            CHECK(emulator.registerValue(R0) == 0);
        }

        SECTION("Statement #9") {
            std::string program = "ldrge r0, =variable";
            REQUIRE_NOTHROW(emulator.execute(program));
        }

        SECTION("Statement #10") {
            std::string program = "str r0, [sp, #4]!";
            REQUIRE_NOTHROW(emulator.execute(program));
        }

        SECTION("Statement #11") {
            std::string program = "label:";
            REQUIRE_NOTHROW(emulator.execute(program));
        }

    }

    SECTION("Multi-line Programs") {

        Emulator emulator;

        // simple procedural program
        SECTION("Program #1") {
            std::string program = 
                "mov r0, #6\n"
                "mov r1, #17\n"
                "add r0, r0, r1"
            ;

            std::cout << "\n\nExecuting Program #1: \n\n" << program << std::endl;
            
            emulator.step(program);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 6);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R1) == 17);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 23);
        }

        // loop control structure
        SECTION("Program #2") {
            std::string program = 
                "loop:\n"
                "\tmvns r0, #6\n"
                "\tmov r1, #17\n"
                "\tadds r0, r0, r1\n"
                "\tb loop"
            ;

            std::cout << "\n\nExecuting Program #2: \n\n" << program << std::endl;

            emulator.step(program);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == -6);
            CHECK(emulator.cpsrValue(N) == true);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R1) == 17);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 11);
            CHECK(emulator.cpsrValue(C) == true);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(PC) == 0);                         // next instruction is the back to the first

            emulator.stop();
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

            std::cout << "\n\nExecuting Program #3: \n\n" << program << std::endl;

            emulator.step(program);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 6);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R1) == 17);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(PC) == 0);
            CHECK(emulator.registerValue(LR) == 160);                           // address of instruction following branch to add_fn

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 23);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(PC) == 160);                           // instruction following the branch to the add_fn

            emulator.stop();
        }
        
        // comparisions and conditional execution
        SECTION("Program #4") {
            std::string program = 
                "main:\n"
                "\tmvn r0, #0d7\n"
                "\tmov r1, #012\n"
                "\n"
                "\tcmn r0, #0b111\n"
                "\taddeq r0, r0, r1\n"
                "\tsubne r0, r0, r1\n"
            ;

            std::cout << "\n\nExecuting Program #4: \n\n" << program << std::endl;

            emulator.step(program);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == -7);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R1) == 10);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.cpsrValue(Z) == true);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 3);

            emulator.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CHECK(emulator.registerValue(R0) == 3);                             // value doesn't change because instruction is not executed

            emulator.stop();
        }

    }

}