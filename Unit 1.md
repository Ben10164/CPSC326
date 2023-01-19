# Notes for Unit 1 (or whenever I think we change topics)

## Lecture 1

### Course Overview

* Deep dive into programming language (PL) design & implementation
  * implement made up "MyPL" lang.
  * explore functional programming

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
* total of 700
  * HW (9x): First 7 @30 & Last 2 @20 = 250
  * Quizzes (Highest 10x):  10@10 = 100
  * Exams (2x): 2@100 = 200
  * Attendance: 60 (2 per lecture)
  * Final Project: 90
* To pass the course, you must get at least 60% for Exams+Quizzes and for Homework+Final Project
* Late policy: (Don't get behind...)
  * First 6 hw assignments (NOT 7!)
    * one opportunity to get it regraded (up to 70%)

### What we are using

* cmake
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

### MyPL

#### Info help with hw-1

* To run it, it'll be like python
  * `./mypl myProg.mypl`
* HW1 will be to make a shell that can take in all the flags
  * like `./mypl --help`
* *NOTE* use ctrl-d to end the mode if no file given and in type mode (wow i am bad at describing that, but it makes sense to me)

#### Notes

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
        * null can be used for `== !=` and initializations, but nothing else
          * 5 + null is an error
          * 5 <= null is an error
* Comparators
  * `==, !=, <, <=, >, >=`
  * `or and not`
  * all expressions can be parenthesizes
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
    * cant mix the types
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