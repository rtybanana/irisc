cmake_minimum_required(VERSION 3.0.0)
project(iRISC VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(FLTK_DIR /home/rory/downloads/fltk-1.3.5/build)
set(OpenGL_GL_PREFERENCE "LEGACY")

include(CTest)
enable_testing()

find_package(FLTK REQUIRED NO_MODULE)
find_package(OpenGL REQUIRED)
include_directories(/usr/local/include/replxx)

add_executable(
  irisc 
  WIN32 MACOSX_BUNDLE
  src/main.cpp
  src/cmd/repl.cpp
  src/cmd/repl.h
  src/cmd/util.c
  src/cmd/util.h
  src/cmd/constants.h
  src/lexer/lexer.cpp
  src/lexer/lexer.h
  src/lexer/token.cpp
  src/lexer/token.h
  src/parser/parser.cpp
  src/parser/parser.h
  src/parser/syntax.cpp
  src/parser/syntax.h
  src/parser/constants.h
  src/emulator/emulator.cpp
  src/emulator/emulator.h
  src/emulator/gui/registers.cpp
  src/emulator/gui/registers.h
  src/emulator/gui/memory.cpp
  src/emulator/gui/memory.h
  src/emulator/gui/instruction.cpp
  src/emulator/gui/instruction.h
  src/emulator/gui/editor.cpp
  src/emulator/gui/editor.h
  src/emulator/gui/constants.h
  src/widgets/hoverbox.cpp
  src/widgets/hoverbox.h
)

target_include_directories(irisc PUBLIC ${FLTK_INCLUDE_DIRS})

target_link_libraries(irisc fltk)
target_link_libraries(irisc ${OPENGL_LIBRARIES})
target_link_libraries(irisc replxx)



find_package(Catch2 REQUIRED)

add_executable(
  tests 
  tests/lexer.cpp
)

target_link_libraries(tests Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
