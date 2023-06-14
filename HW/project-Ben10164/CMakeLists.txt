
project(MyPL_2023)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")
# include_directories("test")

# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


add_executable(default_value_tests tests/default_value_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp
  src/vm.cpp src/vm_instr.cpp src/var_table.cpp src/code_generator 
  src/semantic_checker.cpp src/simple_parser.cpp src/symbol_table.cpp)
target_link_libraries(default_value_tests ${GTEST_LIBRARIES} pthread) 

add_executable(keyword_argument_tests tests/keyword_argument_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp
  src/vm.cpp src/vm_instr.cpp src/var_table.cpp src/code_generator
  src/semantic_checker.cpp src/simple_parser.cpp src/symbol_table.cpp)
target_link_libraries(keyword_argument_tests ${GTEST_LIBRARIES} pthread)

# create mypl target
add_executable(mypl src/token.cpp src/mypl_exception.cpp src/lexer.cpp
  src/simple_parser.cpp src/ast_parser.cpp src/print_visitor.cpp
  src/symbol_table.cpp src/semantic_checker.cpp src/vm_instr.cpp
  src/vm.cpp src/var_table.cpp src/code_generator.cpp src/mypl.cpp)


# Extra tests
add_executable(token_tests old_tests/token_tests.cpp src/token.cpp)
target_link_libraries(token_tests ${GTEST_LIBRARIES} pthread)

add_executable(lexer_tests old_tests/lexer_tests.cpp src/token.cpp
  src/mypl_exception.cpp src/lexer.cpp)
target_link_libraries(lexer_tests ${GTEST_LIBRARIES} pthread)

add_executable(simple_parser_tests old_tests/simple_parser_tests.cpp src/token.cpp
  src/mypl_exception.cpp src/lexer.cpp src/simple_parser.cpp)
target_link_libraries(simple_parser_tests ${GTEST_LIBRARIES} pthread)

add_executable(ast_parser_tests old_tests/ast_parser_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp)
target_link_libraries(ast_parser_tests ${GTEST_LIBRARIES} pthread)

add_executable(parser_syntax_tests old_tests/parser_syntax_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp)
target_link_libraries(parser_syntax_tests ${GTEST_LIBRARIES} pthread)

add_executable(semantic_checker_tests old_tests/semantic_checker_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp
  src/symbol_table.cpp src/semantic_checker.cpp)
target_link_libraries(semantic_checker_tests ${GTEST_LIBRARIES} pthread)

add_executable(vm_tests old_tests/vm_tests.cpp src/mypl_exception.cpp
  src/vm_instr.cpp src/vm.cpp)
target_link_libraries(vm_tests ${GTEST_LIBRARIES} pthread)

add_executable(code_generator_tests old_tests/code_generator_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp
  src/vm.cpp src/vm_instr.cpp src/var_table.cpp src/code_generator)
target_link_libraries(code_generator_tests ${GTEST_LIBRARIES} pthread)