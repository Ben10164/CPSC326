# Unit 2 <!-- omit in toc -->

## Table of Contents <!-- omit in toc -->

* [Lecture 14](#lecture-14)
  * [Interpretation Review](#interpretation-review)
  * [PL "Virtual Machines"](#pl-virtual-machines)
  * [A "bytecode" VM](#a-bytecode-vm)
  * [MyPL VM](#mypl-vm)

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
