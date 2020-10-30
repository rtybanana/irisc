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

add_executable(
  iRISC 
  src/main.cpp
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
  src/emulator/gui/stack.cpp
  src/emulator/gui/stack.h
  src/emulator/gui/heap.cpp
  src/emulator/gui/heap.h
  src/emulator/gui/instruction.cpp
  src/emulator/gui/instruction.h
  src/emulator/gui/gui.h
  src/emulator/gui/widgets/hoverbox.cpp
  src/emulator/gui/widgets/hoverbox.h
)

TARGET_LINK_LIBRARIES(iRISC ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(iRISC ${OPENGL_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
