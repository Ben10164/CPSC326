# Notes for Unit 1 (or whenever I think we change topics)

## Table of Contents <!-- omit in toc -->

* [Notes for Unit 1 (or whenever I think we change topics)](#notes-for-unit-1-or-whenever-i-think-we-change-topics)
  * [Lecture 1](#lecture-1)
    * [Course Overview](#course-overview)
    * [Goals](#goals)
    * [Some logistics](#some-logistics)
    * [Grading](#grading)
    * [What we are using](#what-we-are-using)
    * [Expectations](#expectations)
    * [MyPL Usage](#mypl-usage)
      * [Info help with hw-1](#info-help-with-hw-1)
      * [Intro Notes / Usage](#intro-notes--usage)
  * [Monday 1](#monday-1)
    * [Exercises](#exercises)
  * [Lecture 2](#lecture-2)
    * [Compilation](#compilation)
      * [Front End Compilation](#front-end-compilation)
      * [PL Backend Steps](#pl-backend-steps)
    * [Interpretation](#interpretation)
    * [MyPL Implementation](#mypl-implementation)
    * [Paragraph Example](#paragraph-example)
    * [Types of Interpreters](#types-of-interpreters)
    * ["Transpiler"](#transpiler)
    * [Lexical Analysis (Lexer)](#lexical-analysis-lexer)

## Lecture 1

### Course Overview

* Deep dive into programming language (PL) design & implementation
  * Implement made up "MyPL" language.
  * Explore functional programming

### Goals

* More progressive experience
* better understanding how compilers/interpreters work
* better understanding of language design
  * why are some constructs the way they are
* exposure to different "paradigms"
  * different ways to think about programming, or programming approaches

### Some logistics

* The website `https://cs.gonzaga.edu/faculty/bowers/courses/cpsc326/` contains all the links needed for the course
* The discussion section will be for us to ask questions and work together
  * this is going to be really good and useful
* Canvas will be used to post grades

### Grading

* Every assignment is worth a certain amount of points
* Total of 700
  * HW (9x): First 7 @30 & Last 2 @20 = 250
  * Quizzes (Highest 10x): 10@10 = 100
  * Exams (2x): 2@100 = 200
  * Attendance: 60 (2 per lecture)
  * Final Project: 90
* To pass the course, you must get at least 60% for Exams+Quizzes and for Homework+Final Project
* Late policy: (Don't get behind...)
  * First 6 HW assignments (NOT 7!)
    * one opportunity to get it regraded (up to 70%)

### What we are using

* CMake
* make
* g++ (v20)
* Google Test (gtest)
* gdb (useful)
* Valgrind (optional)

### Expectations

* Everyone should take their responsibility for their own learning
  * **SEEK HELP IF YOU DON'T UNDERSTAND**
    * piazza, office hours, discussion lecture, email, etc...
* Everyone should engage and participate
  * put away distractions
  * take notes
* start assignments early
  * "it gets difficult"
    * crap...

### MyPL Usage

#### Info help with hw-1

* To run it, it'll be like python
  * `./mypl myProg.mypl`
* HW1 will be to make a shell that can take in all the flags
  * like `./mypl --help`
* *NOTE* use ctrl-d to end the mode if no file given and in type mode (wow i am bad at describing that, but it makes sense to me)

#### Intro Notes / Usage

* Strongly typed
  * this means that all type errors are detected before rune-time
* Comments
  * \# and only single line
* Primitive types / base types
  * `int double bool char string void`
    * `void` used for functions
  * Values
    * `1, 2, 4` - ints
    * `13.1, 1.11` - doubles
      * `0.0` is allowed, `.0` is not
    * `true, false` - bool
    * `'a'`, 'b'` - char
    * `"this is right!"` - string
      * cant go onto a new line. i.e. cant do stuff like python's `""" multiline"""`
    * null
      * any variable can be set to null
* Variable declarations:
  * type, name, initializer
    * `int x = 0`
    * `char y = 'a'`
    * `string s = null`
    * `int u = null`
      * **Note**
        * null can be used for `== !=` and initialization, but nothing else
          * 5 + null is an error
          * 5 <= null is an error
* Comparators
  * `==, !=, <, <=, >, >=`
  * `or and not`
  * all expressions can be parenthesized
    * `(x<=y) and not (y>z)`
* Assignment stunts

```cpp
x = 5
a = 22
```

* for loops

```cpp
for (int i = 0; i < 10; i = i + 1){ 
    ...
}
```

* While loops

```cpp
while (x>5){ 
    ...
} 
```

* If statements
  
```cpp
if (true){ 
    ...
}
else if (false){ (may have been elseif with no space) 
    ...
}
```

* Arithmetic
  * `+, -, * /`
    * can't mix the types
      * int op int
      * double op double
      * `5+2.1` is a type error
    * `to_int(string)`
    * `to_double(string or int)`
    * `to_string(anything)`

* Functions
  * they have a return type

```cpp
int f(int x, int y){
    return x + y
}
```

* Additional notes
  * No negative symbol
  * No semicolon
  * `to_int(string)`
  * `to_double(string or int)`
  * `to_string(anything)`
  * `concat(__,__)`
  * example of stuff `int r = f(3,4)`
  * no increment or decrement
  * `struct` (see example)
  * `print(...)` (see example)

```cpp
struct Node{
    int val,
    Node next
}

void println(int x){
    print(x)
    print('\n')
}
```

## Monday 1

```cpp
void print_leaves(const Node* root){
    if(root == nullptr)
        return; // probably wont happen
    if(root->left)
        print_leaves(root->left)
    if(root->right)
        print)leaves(root->right)
    if(!root->left && !root->right)
        cout << root->val << endl;
}
```

### Exercises

1. Write a MyPL function that computes the sum of a given array of integer values

    ```c
    int sum(array int xs){
        int sum = 0
        for(int i = 0; i < length(xs); i = i + 1){
            sum = sum + xs[i]
        }
        return sum
    }
    ```

1. Write a MyPL program that repeatedly prompts a user for a number until thy enter "-1", and then prints out the sum of the numbers they entered (excluding -1)

    ```c
    void main(){
        int NEG.ONE = 0 - 1
        int sum = 0
        int val = 0
        while(val != NEG.ONE){
            print("Please enter an int (-1 to quit): ")
            val = to_int(input())
            if(val != NEG.ONE){
                sum = sum + val
            }
        }
        print("The sum is: ")
        print(sum)
    }
    ```

1. Implement Merge Sort in MyPL

    ```c
    void merge_sort(array int A, int start, int end){
        if(start < end){
            int mid = (start + end) / 2
            merge_sort(A, start, mid)   # recursive step
            merge_sort(A, mid + 1, end) # recursive step
            merge(A, start, mid, end)   # merge sublists
        }

## Lecture 2

### Compilation

* Compilation:
  * Uses the design principle: Seperation of Concerns
  * Typical assembly line, parts come in and people have specific jobs
    * each box/step has a specific thing that it does

#### Front End Compilation

* Source Program
  * Source Program feeds into the Lexical Analyzer (Lexer)
* Lexer
  * Lexer takes the source input, does some processing, and identifies "Tokens"
    * basic part of the programming language (keyword, or program symbol)
  * Reads until a token, then sends it out!
    * If there is an error, the lexer can abort
      * instead of reading everything and then checking for tokens, lexer does it token at a time in case there is an error
      * Designed to be efficient
    * Why do we care about compilers to be fast?
      * You can't develop as fast if it takes a long time to compile the code
  * Lexer outputs a token "stream"

* Parser
  * Token stream goes to the Syntax Analysis (Parser)
  * The parser doesn't have to worry about the source code itself, because it is only working with wellformed tokens.
  * The parser figures out how the tokens go together
    * if the tokens can't, that is a syntax error
  * Parser Ouputs an "Abstract Syntax Tree" (AST)

  ```c
  3 + x * 5 will be seen as 
                                  +
                                /   \
                               3     *
                                    / \
                                   x   5
  ```

* AST goes to the Semantic Analysis (Static analyzer, type checker)
  * are you using variables you declared
  * do the types match
  * function/var not defined
  * `type error` or `use before def error`
  * Outputs the AST and sometimes type info (type annotations)
    * Type info could be annotating the `x` in the AST as an `int`

#### PL Backend Steps

* Intermediate Code Generation
  * AST generates Intermediate code
  * Outputs Intermediate Code, also known as IR (Intermediate Representation)

* Optimization
  * This is the brain
  * No one understands some of these
  * Outputs more IR

* Machine Code Generation
  * Once the IR is optimized, it is used to create an executable
  * Outputs the Executable

### Interpretation

* Backend
  * Same steps as compilation up until the AST + type info (front end)
  * passed into the intermediate representation (Byte code)
  * Optimized
  * Outputs IR (byte code)
    * `.class` in java (`javac` does all of these steps and ouputs the `.class`)

* Interpreter (Program)
  * takes in the byte code and runs it
    * `java`

### MyPL Implementation

* MyPL will do what python does
* produces ByteCode that will be run in a virtual machine
  * we will be making this VM
* ByteCode will just be a bunch of instructions

### Paragraph Example

* Lexer: "Identifies the words and punctuation from a paragraph of text"
* Syntax: "checks for the grammar"
* Sematic: "Does it make sense"

### Types of Interpreters

1. AST Interpreter
    * Execute the AST directly
    * Modifiers will modify the tree itself rather than generate more IR

1. ByteCode Interpreter (VMs)
    * Virtual Machine in this sense, is a program that can run ByteCode
    * VMs came before programming languages!
    * IR is ByteCode
    * Interpreter runs the ByteCode directly
    * "Write once, run anywhere"

1. Just-in-time Compilation (JIT)
    * Hybrid of the previous two
    * "Hot Spots"
      * As the JVM is running (Program is running), it is analyzing the byte code and the patterns being called
      * Figures out on the fly which portions of the ByteCode should be converted to machine code

### "Transpiler"

Converts from one language into another language

* Goes from one high level language into another
  * Compiler goes from a high level language to a low level language

### Lexical Analysis (Lexer)

* Lexer outputs `tokens`
* `Token` = the smallest meaningful unit of a programming language
  * "Special Words" ("Reserved Words")
    * `int`, `enum`, `if`, `for`, `while`, `true`, etc.
  * Opperators & Punctuations
    * `!`, `==`, `%`, `;`, `+`, `(`, etc.
  * Literals (Constants)
    * `42`, `3.14`, `"foo"`, etc
  * Names (Identifiers) [IDs]
    * `int x;`
      * `x` is the identifier
  * Tokens have `types` & "Lexemes"
    * `types`: what are we representing
      * LPAREN, RPAREN, INT_TYPE, INT_VALUE, ID, etc.
    * "Lexemes": what it represents
      * "(", ")", "int", "42", "x"
