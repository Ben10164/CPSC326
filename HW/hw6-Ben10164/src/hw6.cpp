//----------------------------------------------------------------------
// FILE: hw6.cpp
// DATE: CPSC326, Spring 2023
// AUTH: 
// DESC:
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <memory>
#include "mypl_exception.h"
#include "vm.h"


using namespace std;


void vm_test_1(VM& vm);         // example program


void usage(const string& command, ostream& out)
{
  out << "Usage: " << command << " [option] " << endl;
  out << "Options:" << endl;
  out << "  --ir       print intermediate (code) representation" << endl;
}


int main(int argc, char** argv)
{
  // convert arguments to strings
  string args[argc];
  for (int i = 0; i < argc; ++i)
    args[i] = string(argv[i]);
  
  // check for too many command line args
  if (argc > 2) {
    usage(args[0], cerr);
    return 1;
  }

  bool inter_mode = false;
  
  // check if in lexer or print mode
  if (argc > 1 && args[1] == "--ir")
    inter_mode = true;
  else if (argc > 1) {
    usage(args[0], cerr);
    return 1;
  }
    
  if (inter_mode) {
    try {
      VM vm;
      vm_test_1(vm);
      cout << to_string(vm) << endl;
    } catch (MyPLException& ex) {
      cerr << ex.what() << endl;            
    }
  }
  else {
    try {
      VM vm;
      vm_test_1(vm);
      vm.run();
    } catch (MyPLException& ex) {
      cerr << ex.what() << endl;            
    }
  }

}

void vm_test_1(VM& vm)
{
  //----------------------------------------------------------------------
  // TODO: Implement the following MyPL program as VM instructions.
  //----------------------------------------------------------------------
  
  /*======================================================================

     bool is_prime(int n) {
       var m = n / 2
       var v = 2
       while (v <= m) {
         var r = n / v
         var p = r * v
         if (p == n) {
           return false
         }
         v = v + 1
       }
       return true
     }
    
     void main() {
       print("Please enter integer values to sum (prime number to quit)\n")
       int sum = 0
       print(">> Enter an int: ")
       int val = to_int(read())
       while (not is_prime(val)) {
         sum = sum + val
         print(">> Enter an int: ")
         val = to_int(read())
       }
       print(The sum is: ")
       print(sum)
       print("\nGoodbye!\n")  
     }

  ======================================================================*/  
    VMFrameInfo is_prime {"is_prime", 1};    
    is_prime.instructions.push_back(VMInstr::STORE(0)); // 0: store n (0)
    // var m = n / 2
    is_prime.instructions.push_back(VMInstr::LOAD(0)); // 1: load n
    is_prime.instructions.push_back(VMInstr::PUSH(2)); // 2: push 2
    is_prime.instructions.push_back(VMInstr::DIV()); // 3: push the result of n / 2
    is_prime.instructions.push_back(VMInstr::STORE(1)); // 4: store the result in m (1)
    // var v = 2
    is_prime.instructions.push_back(VMInstr::PUSH(2)); // 5: push 2
    is_prime.instructions.push_back(VMInstr::STORE(2)); // 6: store 2 in v (2)
    // while(v <= m){
    is_prime.instructions.push_back(VMInstr::LOAD(2)); // 7: load v
    is_prime.instructions.push_back(VMInstr::LOAD(1)); // 8: load m
    is_prime.instructions.push_back(VMInstr::CMPLE()); // 9: v <= m
    is_prime.instructions.push_back(VMInstr::JMPF(29)); // 10: exit the while loop 
    //  var r = n / v
    is_prime.instructions.push_back(VMInstr::LOAD(0)); // 11: load n
    is_prime.instructions.push_back(VMInstr::LOAD(2)); // 12: load v
    is_prime.instructions.push_back(VMInstr::DIV()); // 13: n / v
    is_prime.instructions.push_back(VMInstr::STORE(3)); // 14: store in r (3)
    //  var p = r * v
    is_prime.instructions.push_back(VMInstr::LOAD(3)); // 15: load r
    is_prime.instructions.push_back(VMInstr::LOAD(2)); // 16: load v
    is_prime.instructions.push_back(VMInstr::MUL()); // 17: r * v
    is_prime.instructions.push_back(VMInstr::STORE(4)); // 18: store in p (4)
    //  if( p == n){
    is_prime.instructions.push_back(VMInstr::LOAD(4)); // 19: load p
    is_prime.instructions.push_back(VMInstr::LOAD(0)); // 20: load n
    is_prime.instructions.push_back(VMInstr::CMPEQ()); // 21: p == n
    //      return false
    is_prime.instructions.push_back(VMInstr::JMPF(24)); // 22: jumps over the jump to false
    is_prime.instructions.push_back(VMInstr::JMP(32)); // 23: jumps to the false (outside the loop) //////// noooooooo
    //  }
    //  v = v + 1
    is_prime.instructions.push_back(VMInstr::LOAD(2)); // 24: load v
    is_prime.instructions.push_back(VMInstr::PUSH(1)); // 25: push 1
    is_prime.instructions.push_back(VMInstr::ADD()); // 26: v + 1
    is_prime.instructions.push_back(VMInstr::STORE(2)); // 27: store in v (2)
    // now it needs to loop back!
    is_prime.instructions.push_back(VMInstr::JMP(7)); // 28: jump to when its loading the values for the cmp
    // }
    is_prime.instructions.push_back(VMInstr::NOP()); // 29: 
    // return true
    is_prime.instructions.push_back(VMInstr::PUSH(true)); // 30: push true 
    is_prime.instructions.push_back(VMInstr::JMP(34)); // 31: jump past the push false to the ret() 
    is_prime.instructions.push_back(VMInstr::NOP()); // 32:
    is_prime.instructions.push_back(VMInstr::PUSH(false)); // 33: 
    is_prime.instructions.push_back(VMInstr::NOP()); // 34: 
    is_prime.instructions.push_back(VMInstr::RET()); // 35:

    VMFrameInfo main {"main", 0};    
    //  print("Please enter integer values to sum (prime number to quit)\n")
    main.instructions.push_back(VMInstr::PUSH("Please enter integer values to sum (prime number to quit)\n"));
    main.instructions.push_back(VMInstr::WRITE());
    //  int sum = 0
    main.instructions.push_back(VMInstr::PUSH(0));
    main.instructions.push_back(VMInstr::STORE(0)); // store in sum (0)
    //  print(">> Enter an int: ")
    main.instructions.push_back(VMInstr::PUSH(">> Enter an int: "));
    main.instructions.push_back(VMInstr::WRITE());
    //  int val = to_int(read())
    main.instructions.push_back(VMInstr::READ());
    main.instructions.push_back(VMInstr::TOINT());
    main.instructions.push_back(VMInstr::STORE(1)); // store in val (1)
    //  while (not is_prime(val)) {
    main.instructions.push_back(VMInstr::LOAD(1)); 
    main.instructions.push_back(VMInstr::CALL("is_prime"));
    main.instructions.push_back(VMInstr::JMPF(13)); // jump past the jump out of the loop
    main.instructions.push_back(VMInstr::JMP(24)); // jump out of the loop
    main.instructions.push_back(VMInstr::NOP()); // 13: skipped jumping out the loop
    //      sum = sum + val
    main.instructions.push_back(VMInstr::LOAD(0));
    main.instructions.push_back(VMInstr::LOAD(1));
    main.instructions.push_back(VMInstr::ADD());
    main.instructions.push_back(VMInstr::STORE(0));
    //      print(">> Enter an int: ")
    main.instructions.push_back(VMInstr::PUSH(">> Enter an int: "));
    main.instructions.push_back(VMInstr::WRITE());
    //      val = to_int(read())
    main.instructions.push_back(VMInstr::READ());
    main.instructions.push_back(VMInstr::TOINT());
    main.instructions.push_back(VMInstr::STORE(1));
    main.instructions.push_back(VMInstr::JMP(9));
    //  }
    main.instructions.push_back(VMInstr::NOP()); // 24: outside the loop
    //  print(The sum is: ")
    main.instructions.push_back(VMInstr::PUSH("The sum is: "));
    main.instructions.push_back(VMInstr::WRITE());
    //  print(sum)
    main.instructions.push_back(VMInstr::LOAD(0));
    main.instructions.push_back(VMInstr::WRITE());
    //  print("\nGoodbye!\n")  
    main.instructions.push_back(VMInstr::PUSH("\nGoodbye!\n"));
    main.instructions.push_back(VMInstr::WRITE());
    // handle the lack of return
    main.instructions.push_back(VMInstr::PUSH(nullptr));
    main.instructions.push_back(VMInstr::RET());

    vm.add(is_prime);
    vm.add(main);
}
