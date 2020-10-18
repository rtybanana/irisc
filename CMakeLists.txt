cmake_minimum_required(VERSION 3.0.0)
project(iRISC VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

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
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
