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
* [Lecture 16](#lecture-16)
  * [Implementing Call Instructions](#implementing-call-instructions)
  * [Implementing RET Instructions](#implementing-ret-instructions)
* [Monday 8](#monday-8)
  * [Heap Alloc](#heap-alloc)
  * ["Struct" Heap](#struct-heap)
  * ["Array" heap](#array-heap)
  * [VM Instruction examples](#vm-instruction-examples)

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

## Lecture 16

### Implementing Call Instructions

Example: f(...) =calls=> g1(...) =calls=> g2(...)

1. Get name for g

    ```cpp
    string g = get<string>(instr.operand().value()); // all call instructions have an operand
    ```

2. Instantiate a new frame & set its frame info

    ```cpp
    shared_ptr<VMFrame> new_frame = make_shared<VMFrame>();
    new_frame->info = frame_info[g];
    ```

3. Then push it onto the call_stack

    ```cpp
    call_stack.push(new_frame); // this is now g2
    ```

4. Copy arg-count # of args into g2

    ```cpp
    VMValue x = frame->operand_stack.top();
    new_frame->operand_stack.push(x);
    frame->operand_stack.pop();
    ```

### Implementing RET Instructions

1. Grab return value

    ```cpp
    VMValue v = frame->operand_stack.top();
    ```

2. Pop frame

    ```cpp
    call_stack.pop();
    frame = call_stack.top();
    ```

3. If frame exists (it wont exist if you are returning from main)

    ```cpp
    // push return value to the top of the stack
    frame->operand_stack.push(v);
    ```

## Monday 8

### Heap Alloc

* Heap Details
  * Seperated into struct & array (object) heaps
  * Each object (struct or array) assigned a unique object id
    * First object id is 2023 in MyPL

```cpp
Node n1 = new Node
// generates some space in the heap,
// then there is an object ID that is created

// n1 -> 2023
Node n2 = new Node
// n2 -> 2024

// is n1 the same object as n2
// their ids are compared
if(n1 == n2){
    // doesnt happen
}

n1 = n2
// n2 -> 2023
if(n1 == n2){
    // happens
}
```

### "Struct" Heap

* Each struct object is represented as a map (field- value pairs)
  * `unordered_map<string, VMValue>`

    ```cpp
    struct Node {
        int val
        Node next
    }

    {"val": 10, "next": nullptr}
    {"val": 0, "next": 2023}
    ```

* Heap is a OID to obj map
  * `unordered_map<int, unordered_map<string, VMValue>> struct_heap`
* add, set, get fields

```cpp
ALLOCS()

struct_heap[next_obj_id] = {};
frame->operand_stack.push(next_obj_id); // put the object IDs on the top of the operand stack
++next_obj_id;
```

### "Array" heap

* Each array object is represented as a vector
  * You push onto the stack the size of the array
  * Then you allocate size
  * Then you initialize the elements
* Assigned a fixed number of slots on allocation
* Given default value of each slot
* Can get & set values by index
* `unordered_map<int,vector<VMValue>> array_heap`

```cpp
array int xs = new int[10]
// allocates a new vector of size 10, fills the values with nulls
```

```cpp
ALLOCA()
// ...pop def.value, pop size

VMValue val = frame->operand_stack.top();
frame->operand_stack.pop();
int size = get<int>(frame->operand_stack.top());
frame->operand_stack.pop();

array_heap[next_obj_id] = vector<VMValue>(size, val); // allocate. each element has a value of val
frame->operand_stack.push(next_obj_id);
++next_obj_id;
```

### VM Instruction examples

1. Show how the operand stack changes after each instruction

    ```cpp
    Frame 'main'
    0: PUSH(42)
    |    |
    |    |
    | 42 |
    |____|
    1: PUSH(37)
    |    |
    | 37 |
    | 42 |
    |____|
    2: ADD()
    |    |
    |    |
    | 79 |   (42 + 37)
    |____|
    3: STORE(0)
    |    |
    |    |
    |    |
    |____|   0->79
    4: LOAD(0)
    |    |
    |    |
    | 79 |
    |____|   0->79
    5: WRITE()
    |    |
    |    |
    |    |
    |____|
    ```

1. Translate to MyPL ... assume var 0 is called "x"

    ```cpp
    void main(){
        int x = 42 + 37
        print(x)
    }
    ```

1. Same as 1:

    ```cpp
    Frame 'main'
    0: PUSH(0)
    |    |
    |    |
    |  0 |
    |____|
    1: STORE(0)
    |    |
    |    |
    |    |
    |____|   0->0
    2: LOAD(0)
    |    |
    |    |
    |  0 |
    |____|
    3: PUSH(2)
    |    |
    |  2 |
    |  0 |
    |____|
    4: CMPLT()
    |    |
    |    |
    |true|
    |____|
    5: JMPF(11)
    |    |
    |    |
    |    |   Doesnt jump
    |____|   0->0 
    6: LOAD(0)
    |    |
    |    |
    |  0 |
    |____|   0->0
    7: PUSH(1)
    |    |
    |  1 |
    |  0 |
    |____|   0->0
    8: ADD()
    |    |
    |    |
    |  1 |
    |____|   0->0
    9: STORE(0)
    |    |
    |    |
    |    |
    |____|   0->1
    10: JMP(2)
    |    |
    |    |
    |  1 |
    |____|   0->1
    ...
    this continues until 0->2
    11: NOP()
    12: LOAD(0)
    |    |
    |    |
    |  2 |
    |____|   0->2
    13: WRITE()
    |    |
    |    |
    |    |
    |____|   
    Ouputs(2)
    ```

1. Translate to MyPL, w/ 0->'i'

    ```cpp
    void main(){
        int i = 0
        while(i<2){
            i = i + 1
        }
        print(i)
    }
    ```

1. Translate to VM instructions

    ```cpp
    void main(){
        string s = input()
        if(s=="blue"){
            print("correct")
        elseif(s=="green"){
            print("correct")
        else{
            print("wrong")
        }
    }
    ```

    ```cpp
    Frame 'main'
    0: READ()
    1: STORE(0)
    2: LOAD(0)
    3: PUSH("blue")
    4: CMPEQ()
    5: JMPF(9) //jumps to the elseif
    6: PUSH("correct")
    7: WRITE()
    8: JMP(18) // ending
    9: LOAD(0)
    10: PUSH("green")
    11: CMPEQ()
    12: JMPF(16) // jumps to the else
    13: PUSH("correct")
    14: write()
    15: JMP(18) // ending
    16: PUSH("wrong")
    17: WRITE()
    18: NOP()
    ```
