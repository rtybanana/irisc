cmake_minimum_required(VERSION 3.0.0)
project(iRISC VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(CTest)
enable_testing()

FIND_PACKAGE(FLTK REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(/usr/local/include/replxx)

add_executable(
  iRISC 
  src/main.cpp
  src/ui/repl.cpp
  src/ui/repl.h
  src/ui/editor.cpp
  src/ui/editor.h
  src/ui/util.c
  src/ui/util.h
  src/ui/constants.h
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
  src/emulator/windows/registers.cpp
  src/emulator/windows/registers.h
  src/emulator/windows/stack.cpp
  src/emulator/windows/stack.h
  src/emulator/windows/heap.cpp
  src/emulator/windows/heap.h
  src/emulator/windows/instruction.cpp
  src/emulator/windows/instruction.h
  src/emulator/windows/gui.h
  src/widgets/hoverbox.cpp
  src/widgets/hoverbox.h
)

TARGET_LINK_LIBRARIES(iRISC ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(iRISC ${OPENGL_LIBRARIES})
TARGET_LINK_LIBRARIES(iRISC replxx)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
