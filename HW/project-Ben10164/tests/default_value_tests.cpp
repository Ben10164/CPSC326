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
#include "simple_parser.h"
#include "semantic_checker.h"

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

TEST(BasicDefaultValues, Test1) {
  stringstream in(build_string({
    "int test(int i = 0){"
    "   return i"
    "}",
    "void main() {",
    "   print(test())",
    "   print(test(5))",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("05", out.str());
  restore_cout();
}

TEST(BasicDefaultValues, Test2) {
  stringstream in(build_string({
    "void test(bool b1, bool b2 = false){"
    "   if (b1 == b2){print(\"Oh My!\")}else{print(\"Oh No!\")}"
    "}",
    "void main() {",
    "   test(false)",
    "   test(true)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("Oh My!Oh No!", out.str());
  restore_cout();
}

TEST(BasicDefaultValues, Test3) {
  stringstream in(build_string({
    "void test(int i, int j = 5, int p){"
    "   print(i-j)"
    "}",
    "void main() {",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  try{
    ASTParser(Lexer(in)).parse().accept(generator);
    FAIL();
  }catch(MyPLException& ex){
    string err = ex.what();
    EXPECT_EQ("Parser Error: Default values must be at the end of functions parameters. found ')' at line 1, column 34", err);
  }
  restore_cout();
}


TEST(BasicDefaultValues, Test4) {
  stringstream in(build_string({
    "void println(string s) {",
    "  print(concat(s, \"\\n\"))",
    "}",
    "",
    "int f1(int x, int y=5) {",
    "  int z = x + y",
    "  println(\"... in f1\")",
    "  return z",
    "}",
    "",
    "string f2(string x) {",
    "  println(concat(\"... in f2, x = \", x)) ",
    "  return concat(x, x)",
    "}",
    "",
    "string f3(string x) {",
    "  x = f2(x)",
    "  println(concat(\"... in f3, after f2, x = \", x))",
    "  return x",
    "}",
    "",
    "void main() {",
    "  int r1 = f1(3, 4)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "    ",
    "  r1 = f1(x=3, y=4)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "",
    "  r1 = f1(y=4,x=3)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "",
    "  string r2 = f3(\"ab\")",
    "  println(concat(\"Should be abab: \", r2))",
    "",
    "  r1 = f1(2)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("... in f1\nShould be 7: 7\n... in f1\nShould be 7: 7\n... in f1\nShould be 7: 7\n... in f2, x = ab\n... in f3, after f2, x = abab\nShould be abab: abab\n... in f1\nShould be 7: 7\n", out.str());
  restore_cout();
}

TEST(BasicDefaultValues, Test5) {
  stringstream in(build_string({
    "int sum(int v1, int v2 = 10){",
    "  return v1 + v2",
    "}",
    "",
    "void main() {",
    "  int val = sum(v1=15)",
    "  print(val)",
    "}"
  }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("25", out.str());
  restore_cout();
}


TEST(BasicDefaultValues, Test6) {
  stringstream in(build_string({
    "int sum(int v1, int v2 = 20, int v3 = 30, int v4 = 40){",
    "    return v1 + v2 + v3 + v4",
    "}",
    "",
    "void main() {",
    "  int val = sum(10)",
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

TEST(BasicDefaultValueParserTests, Test1) {
  stringstream in(build_string({
    "void println(string s) {",
    "  print(concat(s, \"\\n\"))",
    "}",
    "",
    "int f1(int x, int y=5) {",
    "  int z = x + y",
    "  println(\"... in f1\")",
    "  return z",
    "}",
    "",
    "void main() {",
    "  r1 = f1(2)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "}"
  }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSemanticCheckerTests, Test1) {
  stringstream in(build_string({
    "void println(string s) {",
    "  print(concat(s, \"\\n\"))",
    "}",
    "",
    "int f1(int x, int y=5) {",
    "  int z = x + y",
    "  println(\"... in f1\")",
    "  return z",
    "}",
    "",
    "void main() {",
    "  int r1 = f1(2)",
    "  println(concat(\"Should be 7: \", to_string(r1)))",
    "}"
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, Test2) {
  stringstream in(build_string({
        "int f(int x, int y) {}",
        "void main() {f(z=10)}",
      }));
  SemanticChecker checker;
  try {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  } catch (MyPLException& ex) {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicASTParserTests, Test1) {
  stringstream in(build_string({
"    int sum(int v1, int v2 = 10, int v3 = 20){",
"        return v1 + v2",
"    }",
"",
"    void main() {",
"        int val = sum(10)",
"      print(val)",
"    }"
      }));
  Program p = ASTParser(Lexer(in)).parse();
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

