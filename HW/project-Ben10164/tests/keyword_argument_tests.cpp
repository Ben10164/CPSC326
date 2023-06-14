//----------------------------------------------------------------------
// FILE: code_generate_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic code generator tests
//----------------------------------------------------------------------



#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "mypl_exception.h"
#include "lexer.h"
#include "ast_parser.h"
#include "vm.h"
#include "code_generator.h"

using namespace std;


streambuf* stream_buffer;


void change_cout(stringstream& out)
{
  stream_buffer = cout.rdbuf();
  cout.rdbuf(out.rdbuf());
}

void restore_cout()
{
  cout.rdbuf(stream_buffer);
}

string build_string(initializer_list<string> strs)
{
  string result = "";
  for (string s : strs)
    result += s + "\n";
  return result;
}

TEST(BasicKeywordArgumentTests, Test1) {
  stringstream in(build_string({
    "void test(int i, int j){"
    "   print(i)"
    "   print(j)"
    "}",
    "void main() {",
    "   test(5, 10)",
    "   test(i = 5, j = 10)",
    "   test(j = 5, i = 10)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("510510105", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test2) {
  stringstream in(build_string({
    "void test(int i, int j){"
    "   print(i)"
    "   print(j)"
    "}",
    "void main() {",
    "   test(i = 5, 10)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  try{
    ASTParser(Lexer(in)).parse().accept(generator);
    FAIL();
  }catch(MyPLException& ex){
    string err = ex.what();
    EXPECT_EQ("Parser Error:  found ')' at line 3, column 18", err);
  }
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test3) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4){",
    "    return v1 + v2 + v3 + v4",
    "}",
    "",
    "void main() {",
    "  int val = sum(v1 = 10, v2 = 20, v3 = 30, v4 = 40)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("100", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test4) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4){",
    "    return v1 + v2 + v3 + v4",
    "}",
    "",
    "void main() {",
    "  int val = sum(v2 = 10, v1 = 20, v4 = 30, v3 = 40)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("100", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test5) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4=40, int v5 = 100){",
    "    return v1 + v2 + v3",
    "}",
    "",
    "void main() {",
    "  int val = sum(v2 = 10, v1 = 20, v3 = 40)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("70", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test6) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4=40, int v5 = 100){",
    "    return v1 + v2 + v3",
    "}",
    "",
    "void main() {",
    "  int val = sum(v1 = 40, v2 = 10, v3 = 20, v5=1000)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("70", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test7) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4=40, int v5 = 100){",
    "    return v1 + v2 + v3 + v4 + v5",
    "}",
    "",
    "void main() {",
    "  int val = sum(v1 = 40, v2 = 10, v3 = 20, v5=1000, v4=10)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1080", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test8) {
  stringstream in(build_string({
    "int sum(int v1, int v2, int v3, int v4=40, int v5 = 100){",
    "    return v1 + v2 + v3 + v4 + v5",
    "}",
    "",
    "void main() {",
    "  int val = sum(v5 = 40, v4 = 10, v3 = 20, v2=1000, v1=10)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1080", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test9) {
  stringstream in(build_string({
    "int something(bool tr, int num){",
    "    return num",
    "}",
    "",
    "void main() {",
    "  print(something(num = 5, tr = false))",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("5", out.str());
  restore_cout();
}

TEST(BasicKeywordArgumentTests, Test10) {
  stringstream in(build_string({
    "int something(bool tr, int num, double amen){",
    "    return num",
    "}",
    "",
    "void main() {",
    "  print(something(amen = 2.0, num = 5, tr = false))",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("5", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

