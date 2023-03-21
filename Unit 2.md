# Unit 2 <!-- omit in toc -->

## Table of Contents <!-- omit in toc -->

* [Lecture 14](#lecture-14)
  * [Interpretation Review](#interpretation-review)
  * [PL "Virtual Machines"](#pl-virtual-machines)
  * [A "bytecode" VM](#a-bytecode-vm)
  * [MyPL VM](#mypl-vm)
* [Monday 7](#monday-7)
  * [Exam Review](#exam-review)
    * [Q1](#q1)
    * [Q2](#q2)
* [Lecture 15](#lecture-15)
  * [MyPL VM Instructions](#mypl-vm-instructions)
    * [a) Constants \& Variables](#a-constants--variables)
    * [b) Arithmetic Ops](#b-arithmetic-ops)
    * [c) Logical Ops](#c-logical-ops)
    * [d) Relational Ops](#d-relational-ops)
    * [e) Jumps (Branching)](#e-jumps-branching)
    * [f) Special Instructions](#f-special-instructions)
    * [g) Functions](#g-functions)
    * [h) Heap Instructions (Arrays, Structs)](#h-heap-instructions-arrays-structs)
    * [i) Built-in Functions](#i-built-in-functions)
  * [MYPL VM Instructions Example](#mypl-vm-instructions-example)
  * [Basic MyPL VM Architecture (vm.h/vm.cpp)](#basic-mypl-vm-architecture-vmhvmcpp)

## Lecture 14

### Interpretation Review

* Abstract Syntax Tree (AST) Interpreters
  * Execute the program directly form the AST
* Bytecode Interpreters (aka VMs)
  * Intermediate representation is bytecode
  * Interpreter runs bytecode directly
* Just-in-time Compiler (JIT)
  * Instead of interpreting bytecode, this generates and runs machine code
  * Monitor running code (e.g., frequent "hot spots") and optimize accordingly

### PL "Virtual Machines"

Implements and "abstract computer (CPU)"

* Like a computer, has memory, instruction set, etc.
* Instructions are very similar to assembly
  * but often simpler & higher-level
  * load (vals into memory), store (vals into memory), add (vals in memory), jump, etc.

### A "bytecode" VM

(not the MyPL vm)

* Instructions are encoded in binary
  * e.g., in JVM (`JavaVirtualMachine`), binary is stored in `.class` files

```s
ADD 3 4 # 0110 (opcode) 0011 (operand 1) 0100 (operand 2)
# 01100110100
```

### MyPL VM

* Based on the JVM `JavaVirtualMachine` Architecture
  * Stack based Machine
    * Instead of registers, this uses a stack
* Via API calls, not encoding/decoding
* Take shortcuts
* Performs minimal error checking

1 - DataTypes/Values

* Uses c++ types (int, string, double, ...)
* uses nullptr for MyPL null values

2 - Abstract Stack Machine

* Instead of registers, the VM has "operand" stack

![Abstract Stack Machine Example](images/AbstractStackMachine.png)

The VM Components...

* Operand stack
* Memory for storing variables `<vector>`
* Heap storage
* Function-call stack
  * "Frames"

3 - MyPL Instruction Set...  
Note: `OP(v)` says `v` supplied directly to instruction: `PUSH(v)` means `v` is supplied to the instruction instead of provided by the operand stack

* Constants & Variables
  * `PUSH(v)`, push the constant onto the stack
  * `POP`, pops a value from the stack
  * `LOAD(i)`, i is the index/address of the memory that we want to push onto the stack
    * `grabs value at i, pushes value onto stack`
  * `STORE(i)`
    * `POP v, store v at mem address i`

  ```cpp
  x = 0 // generates a store instruction
  y = x // generates a load instruction
  ```

* Arithmetic Instructions
  * `ADD`
    * `pop x, pop y, push(y+x)`
  * `SUB`
    * `pop x, pop y, push(y-x)`
    * note how the order is y-x, with x being the first thing popped
  * `MUL`
    * `pop x, pop y, push(y*x)`
  * `DIV`
    * `pop x, pop y, push(y/x)`
* Logical Operators
  * `AND`
    * `pop x, pop y, push(y&&x)`
  * `OR`
    * `pop x, pop y, push(y||x)`
  * `NOT`
    * `pop x, push(!x)`
* Relational (Comparison) operators
  * `CMPLT` - Compare Lessthan
    * `pop x, pop y, push(y<x)`
  * `CMPLE`
    * `y<=x`
  * `CMPGT`
    * `y>x`
  * `GMPGE`
    * `y>=x`
  * `CMPEQ`
    * `y==x`
  * `CMPNE`
    * `y!=x`
* Jumps
  * `JMP(i)`
    * jumps to instruction i
  * `JMPF(i)`
    * `pop x, if x is false, jumps to i`

## Monday 7

### Exam Review

#### Q1

a.  
Source Code -> Lexical Analysis ---Token-Stream---> Syntactic Analysis ---AST---> Semantic Analysis ---AST---> (Backend)

b.  

* Compiler: High Level -> Low Level
* Transpiler: High Level -> High Level
* Interpreter: High Level -> IR Executed by VM

c.  

Is this LL(k)?

```txt
<e> ::= <b> IF <b> ELSE <b> | <b>
<b> ::= <b> <= <b> | <b> AND <b> | ID
```

* No (Left side recursion)

Rewrite it as LL(1)

```txt
<e> ::= <b> <e_tail>
<e_tail> ::= IF <b> ELSE <b> | empty
<b> ::= ID <b_tail>
<b_tail> ::= <= <b> | AND <b> | empty
```

#### Q2

a. Using the previous grammar, derive `x if x <=y else y` (left most)

```xml
<e> -> <b> IF <b> ELSE <b>
    -> ID IF <b> ELSE <b>
    -> ID IF <b> <= <b> ELSE <b>
    -> ID IF ID <= <b> ELSE <b>
    -> ID IF ID <= ID ELSE <b>
    -> ID IF ID <= ID ELSE ID
```

b. Using the previous grammar, derive `x if x <=y else y` (right most)

```xml
<e> -> <b> IF <b> ELSE <b>
    -> <b> IF <b> ELSE ID
    -> <b> IF <b> <= <b> ELSE ID
    -> <b> IF <b> <= ID ELSE ID
    -> <b> IF ID <= ID ELSE ID
    -> ID IF ID <= ID ELSE ID
```

c. Draw the parse tree

![Parse_Tree](images/SolExam.jpg)
[Image taken from my Exam]

d. Write an LL(1) grammar to represent well-formed floating point values that cannot have a leading zero and that are not required to have a digit before or after the decimal point

```xml
(<d>+ is <d><d>* (one or more))

<f> ::= <p><d>*.<d>*|.<d>+|0.<d>*
<p> ::= 1|2|3|...|9
<d> ::= 0 | <p>
```

e. Another grammar that includes path expressions, functions, and array accessing.

```xml
<p> ::= ID <r> <p`> 
<p`> ::= .<p> | empty
<r> ::= LBRACKET <expr> RBRACKET | LPAREN <a> RPAREN | empty
<a> ::= <expr> (COMMA <expr>* | empty
```

## Lecture 15

### MyPL VM Instructions

* Operand Stack
  * push and pop values
  * Operand - argument to an instruction
  * most instructions will take their arguments off the operand stack, then place their results onto the operand stack

* [a) Constants \& Variables](#a-constants--variables)
* [b) Arithmetic Ops](#b-arithmetic-ops)
* [c) Logical Ops](#c-logical-ops)
* [d) Relational Ops](#d-relational-ops)
* [e) Jumps (Branching)](#e-jumps-branching)
* [f) Special Instructions](#f-special-instructions)
* [g) Functions](#g-functions)
* [h) Heap Instructions (Arrays, Structs)](#h-heap-instructions-arrays-structs)
* [i) Built-in Functions](#i-built-in-functions)

#### a) Constants & Variables

```cpp
PUSH(v)
POP()
STORE(i) 
    POP, but moved to be stored at address i
LOAD(i)
    PUSHES the value stored at address i
```

#### b) Arithmetic Ops

```cpp
ADD ...
    POP(x), POP(y), PUSH(y+x)
SUB ...
    POP(x), POP(y), PUSH(y-x)
MUL ...
    POP(x), POP(y), PUSH(y*x)
DIV ...
    POP(x), POP(y), PUSH(y/x)
```

#### c) Logical Ops

```cpp
AND ...
    POP(x), POP(y), PUSH(y && x)
OR ...
    POP(x), POP(y), PUSH(y || x)
NOT ...
    POP(x), PUSH(!x)
```

#### d) Relational Ops

```cpp
CMPLT ...
    POP(x), POP(y), PUSH(y<x)
CMPLE ...
    POP(x), POP(y), PUSH(y<=x)

CMPGT ...
    POP(x), POP(y), PUSH(y>x)

CMPGE ...
    POP(x), POP(y), PUSH(y>=x)

GMPEQ ...
    POP(x), POP(y), PUSH(y==x)

CMPNE ...
    POP(x), POP(y), PUSH(y!=x)
```

#### e) Jumps (Branching)

```cpp
JMP(i)
    Jumps to some index
    Instructions are numbered starting at 0
JMPF(i) ...
    POP(x), IF(x==false){JMP(i)}
```

#### f) Special Instructions

```cpp
DUP() ...
    POP(x), PUSH(x), PUSH(x)
NOP() ...
    Whenever we have a loop, at the very end we put a NOP()
    This allows us to know where to jump to
```

#### g) Functions

```cpp
CALL(f) ...
    calls a function f
RET() ...
    exits function, passes back value at the top of the stack
```

#### h) Heap Instructions (Arrays, Structs)

```cpp
ALLOCS() ...
    Allocates a struct object on the heap, PUSH OID x
        OIDs in MyPL are ints, they start at 2023
        First OID is 2023, next is 2024, next is 2025, ...
ALLOCA() ...
    Allocates an array object on the heap, PUSH OID x
ADDF(f) ...
    POP(OID(x)), add field f to obj(x)
    adds a field to a struct object
    f is the field
SETF(f) ...
    POP(OID(x)), POP(y), obj(x).f = y
GETF(f) ...
    POP(OID(x)), PUSH(obj(x).f)
SETI()
    POP(x), POP(y), POP(z), Set obj(z)[y] = x
    Sets the index 
    a[0] = "abc"
    First you push the OID (a) 
    Then you push the index expression (0)
    Then you push the RHS ("abc")
    Then you call SETI()
GETI() ...
    POP(x), POP(y), PUSH(obj(y)[x])
```

#### i) Built-in Functions

```cpp
WRITE() ...
    POP(x), outputs x
READ() ...
    PUSH(x)
    x is from std::in
SLEN() ...
    POP(str(x)), PUSH(len(x))
ALEN() ...
    POP(OID(x)), PUSH(len(obj(x))
GETC() ...
    POP(str(x)), POP(int(y)), PUSH(x[y])
TOINT() ...
    POP(x), PUSH(to_int(x))
TODBL() ...
    POP(x), PUSH(to_double(x))
TOSTR() ...
    POP(x), PUSH(to_string(x))
CONCAT() ...
    POP(x), POP(y), PUSH(y+x) string s
```

### MYPL VM Instructions Example

Generate VM Instructions for:

```cpp
// i : index 0
// j : index 1
while(i<j){
    j=j+1
}
```

```cpp
0 LOAD(0) // loads i
1 LOAD(1) // loads j
2 CMPLT() // compares the top two items on the stack (pushes result)
3 JMPF(9) // Jump to the end (outside the loop) if the val on top the stack is false
4 LOAD(1) // loads j
5 PUSH(1) // pushes 1
6 ADD() // adds the top two items on the stack (pushes result)
7 STORE(1) // stores top of stack (j+1)
8 JMP(0)
9 NOP()
```

### Basic MyPL VM Architecture (vm.h/vm.cpp)

![VM-Components](images/VM-Components.png)

![VM-Stack-Frame](images/VM-Stack-Frame.png)
(Blue = Frame Info)
