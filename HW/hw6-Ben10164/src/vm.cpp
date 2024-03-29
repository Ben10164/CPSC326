//----------------------------------------------------------------------
// FILE: vm.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: 
// DESC: 
//----------------------------------------------------------------------

#include <iostream>
#include "vm.h"
#include "mypl_exception.h"


using namespace std;


void VM::error(string msg) const
{
  throw MyPLException::VMError(msg);
}


void VM::error(string msg, const VMFrame& frame) const
{
  int pc = frame.pc - 1;
  VMInstr instr = frame.info.instructions[pc];
  string name = frame.info.function_name;
  msg += " (in " + name + " at " + to_string(pc) + ": " +
    to_string(instr) + ")";
  throw MyPLException::VMError(msg);
}


string to_string(const VM& vm)
{
  string s = "";
  for (const auto& entry : vm.frame_info) {
    const string& name = entry.first;
    s += "\nFrame '" + name + "'\n";
    const VMFrameInfo& frame = entry.second;
    for (int i = 0; i < frame.instructions.size(); ++i) {
      VMInstr instr = frame.instructions[i];
      s += "  " + to_string(i) + ": " + to_string(instr) + "\n"; 
    }
  }
  return s;
}


void VM::add(const VMFrameInfo& frame)
{
  frame_info[frame.function_name] = frame;
}


void VM::run(bool DEBUG)
{
  // grab the "main" frame if it exists
  if (!frame_info.contains("main"))
    error("No 'main' function");
  shared_ptr<VMFrame> frame = make_shared<VMFrame>();
  frame->info = frame_info["main"];
  call_stack.push(frame);

  // run loop (keep going until we run out of instructions)
  while (!call_stack.empty() and frame->pc < frame->info.instructions.size()) {

    // get the next instruction
    VMInstr& instr = frame->info.instructions[frame->pc];

    // increment the program counter
    ++frame->pc;

    // for debugging
    if (DEBUG) {
      // TODO
      cerr << endl << endl;
      cerr << "\t FRAME.........: " << frame->info.function_name << endl;
      cerr << "\t PC............: " << (frame->pc - 1) << endl;
      cerr << "\t INSTR.........: " << to_string(instr) << endl;
      cerr << "\t NEXT OPERAND..: ";
      if (!frame->operand_stack.empty())
        cerr << to_string(frame->operand_stack.top()) << endl;
      else
        cerr << "empty" << endl;
      cerr << "\t NEXT FUNCTION.: ";
      if (!call_stack.empty())
        cerr << call_stack.top()->info.function_name << endl;
      else
        cerr << "empty" << endl;
    }

    //----------------------------------------------------------------------
    // Literals and Variables
    //----------------------------------------------------------------------

    if (instr.opcode() == OpCode::PUSH) {
      frame->operand_stack.push(instr.operand().value());
    }
    else if (instr.opcode() == OpCode::POP) {
      frame->operand_stack.pop();
    }
    else if(instr.opcode() == OpCode::LOAD){
        frame->operand_stack.push(frame->variables[get<int>(instr.operand().value())]);
    } 
    else if(instr.opcode() == OpCode::STORE){
        if(frame->variables.size() > get<int>(instr.operand().value())){
            frame->variables[get<int>(instr.operand().value())] = frame->operand_stack.top(); 
        }else{
            frame->variables.push_back(frame->operand_stack.top());
        }
        frame->operand_stack.pop();
    }
    
    //----------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::ADD) {
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(add(y, x));
    }
    else if(instr.opcode() == OpCode::SUB){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(sub(y, x));
    }
    else if(instr.opcode() == OpCode::MUL){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(mul(y, x)); 
    }
    else if(instr.opcode() == OpCode::DIV){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(div(y, x));
    }
    else if(instr.opcode() == OpCode::AND){
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        bool b = get<bool>(x) && get<bool>(y);
        frame->operand_stack.push(b);        
    }
    else if(instr.opcode() == OpCode::OR){
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        bool b = get<bool>(x) || get<bool>(y);
        frame->operand_stack.push(b);
    }
    else if(instr.opcode() == OpCode::NOT){
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        if(holds_alternative<bool>(x)){
            if(get<bool>(x) == true){
                frame->operand_stack.push(false);
            }else{
                frame->operand_stack.push(true);
            }
        }else{
            error("Only bools can have NOT used on them.", *frame.get());
        }
    }
    else if(instr.opcode() == OpCode::CMPLT){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(lt(y, x));
    } 
    else if(instr.opcode() == OpCode::CMPLE){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(le(y, x));  
    } 
    else if(instr.opcode() == OpCode::CMPGT){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(gt(y, x));
    } 
    else if(instr.opcode() == OpCode::CMPGE){
      VMValue x = frame->operand_stack.top();
      ensure_not_null(*frame, x);
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      ensure_not_null(*frame, y);
      frame->operand_stack.pop();
      frame->operand_stack.push(ge(y, x));
    }
    else if(instr.opcode() == OpCode::CMPEQ){
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      frame->operand_stack.pop();
      frame->operand_stack.push(eq(y, x));
    }
    else if(instr.opcode() == OpCode::CMPNE){
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      VMValue y = frame->operand_stack.top();
      frame->operand_stack.pop();
      // store the value of eq
      frame->operand_stack.push(eq(y, x));
      VMValue eq = frame->operand_stack.top();
      frame->operand_stack.pop();
      if(get<bool>(eq) == false){
        frame->operand_stack.push(true);
      }else{
        frame->operand_stack.push(false);
      }
    }

    //----------------------------------------------------------------------
    // Branching
    //----------------------------------------------------------------------

    else if(instr.opcode() == OpCode::JMP){
        ensure_not_null(*frame, instr.operand().value());
        if(holds_alternative<int>(instr.operand().value())){
            frame->pc = get<int>(instr.operand().value());
        }else{
            error("Only INTs can be the destination PC for JMP");
        }
    }
    else if(instr.opcode() == OpCode::JMPF){
        ensure_not_null(*frame, instr.operand().value());
        VMValue result = frame->operand_stack.top();
        if(get<bool>(result)==false){
            if(holds_alternative<int>(instr.operand().value())){
                frame->pc = get<int>(instr.operand().value());
            }else{
                error("Only INTs can be the destination PC for JMP");
            }
        }
        frame->operand_stack.pop();
    }
    
    
    //----------------------------------------------------------------------
    // Functions
    //----------------------------------------------------------------------

    // TODO: Finish CALL, RET (done?)
    else if(instr.opcode() == OpCode::CALL){
        if(instr.operand().has_value()){
            string name = get<string>(instr.operand().value());
            shared_ptr<VMFrame> new_frame = make_shared<VMFrame>();
            new_frame->info = frame_info[name];
            call_stack.push(new_frame);
            for(int i = 0; i < new_frame->info.arg_count; i ++){
                VMValue x = frame->operand_stack.top();
                new_frame->operand_stack.push(x);
                frame->operand_stack.pop();
            }
            frame = new_frame;
        }
    }
    else if(instr.opcode() == OpCode::RET){
        VMValue v = frame->operand_stack.top();
        call_stack.pop();
        if(!call_stack.empty()){
            frame = call_stack.top();
            frame->operand_stack.push(v);
        }
    }

    //----------------------------------------------------------------------
    // Built in functions
    //----------------------------------------------------------------------

    else if (instr.opcode() == OpCode::WRITE) {
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      cout << to_string(x);
    }
    else if (instr.opcode() == OpCode::READ) {
      string val = "";
      getline(cin, val);
      frame->operand_stack.push(val);
    }
    else if(instr.opcode() == OpCode::SLEN){
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        int len = get<string>(x).size();
        frame->operand_stack.push(len);
    } 
    else if(instr.opcode() == OpCode::ALEN){
        VMValue val = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame, val);
        int len = array_heap[get<int>(val)].size();
        frame->operand_stack.push(len);
    } 
    else if(instr.opcode() == OpCode::GETC){
        VMValue val = frame->operand_stack.top();
        ensure_not_null(*frame, val);
        frame->operand_stack.pop();
        VMValue index = frame->operand_stack.top();
        ensure_not_null(*frame, index);
        frame->operand_stack.pop();
        char slice = get<string>(val)[get<int>(index)];
        if(slice == '\0'){
            error("out-of-bounds string index",*frame.get());
        }
        string s;
        s.append(1, slice);
        frame->operand_stack.push(s);
    } 
    else if(instr.opcode() == OpCode::TOINT){
        VMValue val = frame->operand_stack.top();
        ensure_not_null(*frame, val);
        frame->operand_stack.pop();
        if(holds_alternative<string>(val)){
            string s = get<string>(val);
            try{
                int i = stoi(s);
                frame->operand_stack.push(i);
            }catch(...){
                error("cannot convert string to int", *frame.get());
            }
        }else if(holds_alternative<double>(val)){
            int d = get<double>(val);
            try{
                int i = (int)d;
                frame->operand_stack.push(i);
            }catch(...){
                error("cannot convert double to int", *frame.get());
            }
        }else if(holds_alternative<int>(val)){
            frame->operand_stack.push(val);
        }
    }
    else if(instr.opcode() == OpCode::TODBL){
        VMValue val = frame->operand_stack.top();
        ensure_not_null(*frame, val);
        frame->operand_stack.pop();
        if(holds_alternative<string>(val)){
            string s = get<string>(val);
            try{
                double d = stod(s);
                frame->operand_stack.push(d);
            }catch(...){
                error("cannot convert string to double", *frame.get());
            }
        }else if(holds_alternative<int>(val)){
            int i = get<int>(val);
            try{
                double d = (double)i;
                frame->operand_stack.push(d);
            }catch(...){
                error("cannot convert int to double", *frame.get());
            }
        }
    } 
    else if(instr.opcode() == OpCode::TOSTR){
        VMValue val = frame->operand_stack.top();
        ensure_not_null(*frame, val);
        frame->operand_stack.pop();
        if(holds_alternative<double>(val)){
            double d = get<double>(val);
            try{
                string s = to_string(d);
                frame->operand_stack.push(s);
            }catch(...){
                error("cannot convert double to string", *frame.get());
            }
        }else if(holds_alternative<int>(val)){
            int i = get<int>(val);
            try{
                string s = to_string(i);
                frame->operand_stack.push(s);
            }catch(...){
                error("cannot convert int to string", *frame.get());
            }
        }
    } 
    else if(instr.opcode() == OpCode::CONCAT){
        VMValue x = frame->operand_stack.top();
        ensure_not_null(*frame, x);
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        ensure_not_null(*frame, y);
        frame->operand_stack.pop();
        string s = get<string>(y) + get<string>(x);
        frame->operand_stack.push(s);
    }
    
    //----------------------------------------------------------------------
    // heap
    //----------------------------------------------------------------------

    // TODO: Finish ALLOCS, ALLOCA, ADDF, SETF, GETF, SETI, GETI
    else if(instr.opcode() == OpCode::ALLOCS){
        struct_heap[next_obj_id] = {};
        frame->operand_stack.push(next_obj_id);
        ++next_obj_id;
    }
    else if(instr.opcode() == OpCode::ALLOCA){
        VMValue val = frame->operand_stack.top();
        frame->operand_stack.pop();
        int size = get<int>(frame->operand_stack.top());
        frame->operand_stack.pop();
        array_heap[next_obj_id] = vector<VMValue>(size, val); // allocate. each element has a value of val
        frame->operand_stack.push(next_obj_id);
        ++next_obj_id;
    }


    else if(instr.opcode() == OpCode::ADDF){
        VMValue x = frame->operand_stack.top(); // oid for x
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        struct_heap[get<int>(x)].insert({get<string>(instr.operand().value()), nullptr});
    }
    else if(instr.opcode() == OpCode::SETF){
        VMValue x = frame->operand_stack.top(); // val to be set to
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top(); // OID
        frame->operand_stack.pop();
        ensure_not_null(*frame, y);

        struct_heap[get<int>(y)][(get<string>(instr.operand().value()))] = x;
    }
    else if(instr.opcode() == OpCode::GETF){
        VMValue x = frame->operand_stack.top(); // OID
        frame->operand_stack.pop();
        ensure_not_null(*frame, x);
        frame->operand_stack.push(struct_heap[get<int>(x)][(get<string>(instr.operand().value()))]);
    }


    else if(instr.opcode() == OpCode::SETI){
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue z = frame->operand_stack.top();
        frame->operand_stack.pop();
        ensure_not_null(*frame,y);
        ensure_not_null(*frame,z);
        if(array_heap[get<int>(z)].size() <= get<int>(y)){
            error("out-of-bounds array index", *frame.get());
        }
        ensure_not_null(*frame, y);
        if(holds_alternative<int>(x)){
            array_heap[get<int>(z)][get<int>(y)] = get<int>(x);
        }else if(holds_alternative<double>(x)){
            array_heap[get<int>(z)][get<int>(y)] = get<double>(x);
        }else if(holds_alternative<string>(x)){
            array_heap[get<int>(z)][get<int>(y)] = get<string>(x);
        }else if(holds_alternative<bool>(x)){
            array_heap[get<int>(z)][get<int>(y)] = get<bool>(x);
        }
    }
    else if(instr.opcode() == OpCode::GETI){
        VMValue x = frame->operand_stack.top();
        frame->operand_stack.pop();
        VMValue y = frame->operand_stack.top();
        frame->operand_stack.pop();

        ensure_not_null(*frame,x);
        ensure_not_null(*frame,y);
        if(array_heap[get<int>(y)].size() <= get<int>(x)){
            error("out-of-bounds array index", *frame.get());
        }
        VMValue r = array_heap[get<int>(y)][get<int>(x)];
        frame->operand_stack.push(r);
    }
    
    //----------------------------------------------------------------------
    // special
    //----------------------------------------------------------------------

    
    else if (instr.opcode() == OpCode::DUP) {
      VMValue x = frame->operand_stack.top();
      frame->operand_stack.pop();
      frame->operand_stack.push(x);
      frame->operand_stack.push(x);      
    }

    else if (instr.opcode() == OpCode::NOP) {
      // do nothing
    }
    
    else {
      error("unsupported operation " + to_string(instr));
    }
  }
}

void VM::ensure_not_null(const VMFrame& f, const VMValue& x) const
{
  if (holds_alternative<nullptr_t>(x))
    error("null reference", f);
}

VMValue VM::add(const VMValue& x, const VMValue& y) const
{
  if (holds_alternative<int>(x)) 
    return get<int>(x) + get<int>(y);
  else
    return get<double>(x) + get<double>(y);
}

VMValue VM::sub(const VMValue& x, const VMValue& y) const
{
if (holds_alternative<int>(x)) 
    return get<int>(x) - get<int>(y);
  else
    return get<double>(x) - get<double>(y);
}

VMValue VM::mul(const VMValue& x, const VMValue& y) const
{
if (holds_alternative<int>(x)) 
    return get<int>(x) * get<int>(y);
  else
    return get<double>(x) * get<double>(y);
}

VMValue VM::div(const VMValue& x, const VMValue& y) const
{
if (holds_alternative<int>(x)) 
    return get<int>(x) / get<int>(y);
  else
    return get<double>(x) / get<double>(y);
}

VMValue VM::eq(const VMValue& x, const VMValue& y) const
{
  if (holds_alternative<nullptr_t>(x) and not holds_alternative<nullptr_t>(y)) 
    return false;
  else if (not holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
    return false;
  else if (holds_alternative<nullptr_t>(x) and holds_alternative<nullptr_t>(y))
    return true;
  else if (holds_alternative<int>(x)) 
    return get<int>(x) == get<int>(y);
  else if (holds_alternative<double>(x))
    return get<double>(x) == get<double>(y);
  else if (holds_alternative<string>(x))
    return get<string>(x) == get<string>(y);
  else
    return get<bool>(x) == get<bool>(y);
}

VMValue VM::lt(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) or holds_alternative<nullptr_t>(y))  // x is nullptr, y is nullptr
        return false;
    else if (holds_alternative<int>(x)) // x is int
        return get<int>(x) < get<int>(y); // return a comparison of two ints (assuming everything is correct?)
    else if (holds_alternative<double>(x))
        return get<double>(x) < get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) < get<string>(y); // one of the tests might fail. "one thing is not less than the others"
    else
        return nullptr;
}

VMValue VM::le(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) or holds_alternative<nullptr_t>(y))  // x is nullptr, y is nullptr
        return false;
    else if (holds_alternative<int>(x)) // x is int
        return get<int>(x) <= get<int>(y); // return a comparison of two ints (assuming everything is correct?)
    else if (holds_alternative<double>(x))
        return get<double>(x) <= get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) <= get<string>(y);
    else
        return nullptr;
}

VMValue VM::gt(const VMValue& x, const VMValue& y) const
{
    if (holds_alternative<nullptr_t>(x) or holds_alternative<nullptr_t>(y))  // x is nullptr, y is nullptr
        return false;
    else if (holds_alternative<int>(x)) // x is int
        return get<int>(x) > get<int>(y); // return a comparison of two ints (assuming everything is correct?)
    else if (holds_alternative<double>(x))
        return get<double>(x) > get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) > get<string>(y);
    else
        return nullptr;
}

VMValue VM::ge(const VMValue& x, const VMValue& y) const
{    if (holds_alternative<nullptr_t>(x) or holds_alternative<nullptr_t>(y))  // x is nullptr, y is nullptr
        return false;
    else if (holds_alternative<int>(x)) // x is int
        return get<int>(x) >= get<int>(y); // return a comparison of two ints (assuming everything is correct?)
    else if (holds_alternative<double>(x))
        return get<double>(x) >= get<double>(y);
    else if (holds_alternative<string>(x))
        return get<string>(x) >= get<string>(y);
    else
        return nullptr;
}

