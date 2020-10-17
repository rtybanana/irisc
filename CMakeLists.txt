cmake_minimum_required(VERSION 3.0.0)
project(iRISC VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

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

target_link_libraries(iRISC ${CURSES_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
