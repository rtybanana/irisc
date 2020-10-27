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
  src/emulator/emulator.cpp
  src/emulator/emulator.h
  src/emulator/registers.cpp
  src/emulator/registers.h
  src/emulator/stack.cpp
  src/emulator/stack.h
  src/emulator/heap.cpp
  src/emulator/heap.h
)

TARGET_LINK_LIBRARIES(iRISC ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(iRISC ${OPENGL_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
