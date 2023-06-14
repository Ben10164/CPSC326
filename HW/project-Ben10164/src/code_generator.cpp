//----------------------------------------------------------------------
// FILE: code_generator.cpp
// DATE: CPSC 326, Spring 2023
// AUTH:
// DESC:
//----------------------------------------------------------------------

#include <iostream> // for debugging
#include "code_generator.h"

using namespace std;

// helper function to replace all occurrences of old string with new
void replace_all(string &s, const string &old_str, const string &new_str)
{
    while (s.find(old_str) != string::npos)
        s.replace(s.find(old_str), old_str.size(), new_str);
}

CodeGenerator::CodeGenerator(VM &vm)
    : vm(vm)
{
}

void CodeGenerator::visit(Program &p)
{
    for (auto &struct_def : p.struct_defs)
        struct_def.accept(*this);
    for (auto &fun_def : p.fun_defs)
        fun_def.accept(*this);
}

void CodeGenerator::visit(FunDef &f)
{
    int old_var_index = next_var_index;
    // create a new frame
    // VMFrameInfo curr_frame; // could be more, also could be curr_frame...
    VMFrameInfo frame;
    curr_frame = frame;
    curr_frame.function_name = f.fun_name.lexeme();
    curr_frame.arg_count = f.params.size();
    // push new environment for var_table
    var_table.push_environment();
    // store each argument passed
    // Add an entry to the var_table for each arg
    // create one STORE() per arg. use a loop
    for (auto &param : f.params)
    {
        if(param.defaultExpr.has_value()){
            curr_frame.default_values.push_back(param.defaultExpr.value());
        }
        curr_frame.parameter_names.push_back(param.var_name.lexeme());
        var_table.add(param.var_name.lexeme());
        VMInstr instr = VMInstr::STORE(next_var_index);
        instr.set_comment("Storing a parameter (" + param.var_name.lexeme() + ") in index " + to_string(next_var_index));
        curr_frame.instructions.push_back(instr);
        next_var_index++;
    }
    // visit each body statement
    for (int i = 0; i < f.stmts.size(); i++)
    {
        f.stmts.at(i)->accept(*this);
    }
    // if last instruction isnt a return:
    if (curr_frame.instructions.size() > 0)
    {
        if (curr_frame.instructions[curr_frame.instructions.size() - 1].opcode() != OpCode::RET)
        {
            curr_frame.instructions.push_back(VMInstr::PUSH("null"));
            curr_frame.instructions.push_back(VMInstr::RET());
        }
    }
    else
    {
        curr_frame.instructions.push_back(VMInstr::PUSH("null"));
        curr_frame.instructions.push_back(VMInstr::RET());
    }

    // pop var_table environment
    var_table.pop_environment();
    // add the frame info to the vm
    vm.add(curr_frame);
    next_var_index = old_var_index;
}

void CodeGenerator::visit(StructDef &s)
{
    struct_defs[s.struct_name.lexeme()] = s;
}

void CodeGenerator::visit(ReturnStmt &s)
{
    s.expr.accept(*this); // accept (evaluate and push) the expression to be returned
    curr_frame.instructions.push_back(VMInstr::RET());
}

void CodeGenerator::visit(WhileStmt &s)
{
    int old_var_index = next_var_index;
    // store the current index
    int loop_start = curr_frame.instructions.size();
    s.condition.accept(*this);                               // evaluate the expression (pushes result to top)
    curr_frame.instructions.push_back(VMInstr::JMPF(10000)); // this will jump outside the loop if the condition is false
    int jmpf_index = curr_frame.instructions.size() - 1;
    var_table.push_environment();
    for (auto &s : s.stmts)
    {
        s->accept(*this);
    }
    var_table.pop_environment();
    VMInstr instr = VMInstr::JMP(loop_start);
    instr.set_comment("This is the JMP instruction to the start of the loop (before the expression)");
    curr_frame.instructions.push_back(instr);
    // this is where the JMPF will go to
    curr_frame.instructions.push_back(VMInstr::NOP());
    instr = VMInstr::JMPF(curr_frame.instructions.size() - 1);
    instr.set_comment("This is the JMP instruction that jumps to the end of the while loop");
    curr_frame.instructions[jmpf_index] = instr;
    next_var_index = old_var_index;
}

void CodeGenerator::visit(ForStmt &s)
{
    int old_var_index = next_var_index;
    // push an environment first for cvar decl
    var_table.push_environment();
    // generate var decl code
    s.var_decl.accept(*this);
    // store the current index
    int loop_start = curr_frame.instructions.size();
    s.condition.accept(*this);                               // evaluate the expression (pushes result to top)
    curr_frame.instructions.push_back(VMInstr::JMPF(10000)); // this will jump outside the loop if the condition is false
    int jmpf_index = curr_frame.instructions.size() - 1;
    var_table.push_environment();
    for (auto &s : s.stmts)
    {
        s->accept(*this);
    }
    var_table.pop_environment();
    // add assignment statement code before final jmp
    s.assign_stmt.accept(*this);
    VMInstr instr = VMInstr::JMP(loop_start);
    instr.set_comment("This is the JMP instruction to the start of the loop (before the expression)");
    curr_frame.instructions.push_back(instr);
    // this is where the JMPF will go to
    curr_frame.instructions.push_back(VMInstr::NOP());
    instr = VMInstr::JMPF(curr_frame.instructions.size() - 1);
    instr.set_comment("This is the JMP instruction that jumps to the end of the for loop loop");
    curr_frame.instructions[jmpf_index] = instr;
    next_var_index = old_var_index;
    // pop env for vardecl
    var_table.pop_environment();
}

void CodeGenerator::visit(IfStmt &s)
{
    s.if_part.condition.accept(*this);
    curr_frame.instructions.push_back(VMInstr::JMPF(10)); // this will be changed. Its going to go to the first else-if (before the expression is accepted)
    int jmpf_index = curr_frame.instructions.size() - 1;
    for (auto &stmt : s.if_part.stmts)
    {
        stmt->accept(*this);
    }
    curr_frame.instructions.push_back(VMInstr::NOP());
    curr_frame.instructions.push_back(VMInstr::JMP(10)); // this will also be changed to be the final nop
    int jmp_to_end_index = curr_frame.instructions.size() - 1;
    curr_frame.instructions.push_back(VMInstr::NOP()); // this is the nop arrived at before the elseif
    VMInstr instr = VMInstr::JMPF(curr_frame.instructions.size() - 1);
    instr.set_comment("This is the jmp to the start of the elseif");
    curr_frame.instructions[jmpf_index] = instr;
    for (auto &elseif : s.else_ifs)
    {
        elseif.condition.accept(*this);
        curr_frame.instructions.push_back(VMInstr::JMPF(10)); // this will be changed. Its going to go to the NOP() before teh forloop continues onto the next else_if
        int loop_idx = curr_frame.instructions.size() - 1;

        for (auto &stmt : elseif.stmts)
        {
            stmt->accept(*this);
        }
        instr = VMInstr::JMP(jmp_to_end_index);
        instr.set_comment("This is the jmp instruction to jump to the jmp statement that jumps to the end");
        curr_frame.instructions.push_back(instr);
        curr_frame.instructions.push_back(VMInstr::NOP());
        instr = VMInstr::JMPF(curr_frame.instructions.size() - 1);
        instr.set_comment("This si the jmpf that will pass over an else-if if false");
        curr_frame.instructions[loop_idx] = instr;
    }
    for (auto &stmt : s.else_stmts)
    {
        stmt->accept(*this);
    }
    curr_frame.instructions.push_back(VMInstr::NOP());
    instr = VMInstr::JMP(curr_frame.instructions.size() - 1);
    instr.set_comment("This is the jmp to the end of the stmt");
    curr_frame.instructions[jmp_to_end_index] = instr;
}

void CodeGenerator::visit(VarDeclStmt &s)
{
    s.expr.accept(*this);
    var_table.add(s.var_def.var_name.lexeme());
    VMInstr instr = VMInstr::STORE(next_var_index);
    instr.set_comment("store in variable " + s.var_def.var_name.lexeme() + " with oid " + to_string(next_var_index));
    curr_frame.instructions.push_back(instr);
    next_var_index++;
}

void CodeGenerator::visit(AssignStmt &s)
{
    // struct stuff
    if (s.lvalue.size() > 1)
    {
        int main_oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
        VMInstr instr = VMInstr::LOAD(main_oid);
        instr.set_comment("This is loading the oid of " + to_string(main_oid) + " which is " + s.lvalue.at(0).var_name.lexeme());
        curr_frame.instructions.push_back(instr);
        if (s.lvalue.at(0).array_expr.has_value())
        {
            s.lvalue.at(0).array_expr.value().accept(*this);
            curr_frame.instructions.push_back(VMInstr::GETI());
        }
        for (int i = 1; i < s.lvalue.size() - 1; i++)
        {
            string path = s.lvalue.at(i).var_name.lexeme();
            instr = VMInstr::GETF(path);
            instr.set_comment("Getting the path: " + path);
            curr_frame.instructions.push_back(instr);
            if (s.lvalue.at(i).array_expr.has_value())
            {
                s.lvalue.at(i).array_expr.value().accept(*this);
                curr_frame.instructions.push_back(VMInstr::GETI());
            }
        }
        string path = s.lvalue.at(s.lvalue.size() - 1).var_name.lexeme();
        if (s.lvalue.at(s.lvalue.size() - 1).array_expr.has_value())
        {
            instr = VMInstr::GETF(path);
            instr.set_comment("Getting the path: " + path);
            curr_frame.instructions.push_back(instr);
            s.lvalue.at(s.lvalue.size() - 1).array_expr.value().accept(*this);
            s.expr.accept(*this); // pushes the val that the path will be set to
            curr_frame.instructions.push_back(VMInstr::SETI());
        }
        else
        {
            s.expr.accept(*this); // pushes the val that the path will be set to
            instr = VMInstr::SETF(path);
            instr.set_comment("Setting the path: " + path);
            curr_frame.instructions.push_back(instr);
        }
    }
    else if (s.lvalue.at(0).array_expr.has_value())
    {
        // get the array
        int oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
        VMInstr instr = VMInstr::LOAD(oid);
        instr.set_comment("Loading the oid " + to_string(oid) + " which is " + s.lvalue.at(0).var_name.lexeme());
        curr_frame.instructions.push_back(instr);
        // push the index that will be modified
        s.lvalue.at(0).array_expr.value().accept(*this);
        // push the rhs
        s.expr.accept(*this);
        curr_frame.instructions.push_back(VMInstr::SETI());
    }
    else
    {
        // other stuff
        // do the rhs first
        int oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
        s.expr.accept(*this);
        VMInstr instr = VMInstr::STORE(oid);
        instr.set_comment("Storing the oid " + to_string(oid) + " which is " + s.lvalue.at(0).var_name.lexeme());
        curr_frame.instructions.push_back(VMInstr::STORE(oid));
    }
}

void CodeGenerator::visit(CallExpr &e)
{
    if(e.keyword_values){
        // this is only valid for custom made functions, so we will handle that in that section
    }else{
        for (auto &arg : e.args)
        {
            arg.accept(*this);
        }
    }
    if (e.fun_name.lexeme() == "print")
    {
        curr_frame.instructions.push_back(VMInstr::WRITE());
    }
    else if (e.fun_name.lexeme() == "concat")
    {
        curr_frame.instructions.push_back(VMInstr::CONCAT());
    }
    else if (e.fun_name.lexeme() == "to_string")
    {
        curr_frame.instructions.push_back(VMInstr::TOSTR());
    }
    else if (e.fun_name.lexeme() == "to_int")
    {
        curr_frame.instructions.push_back(VMInstr::TOINT());
    }
    else if (e.fun_name.lexeme() == "to_double")
    {
        curr_frame.instructions.push_back(VMInstr::TODBL());
    }
    else if (e.fun_name.lexeme() == "get")
    {
        curr_frame.instructions.push_back(VMInstr::GETC());
    }
    else if (e.fun_name.lexeme() == "length")
    {
        curr_frame.instructions.push_back(VMInstr::SLEN());
    }
    else if (e.fun_name.lexeme() == "length@array")
    {
        curr_frame.instructions.push_back(VMInstr::ALEN());
    }
    else if (e.fun_name.lexeme() == "input")
    {
        curr_frame.instructions.push_back(VMInstr::READ());
    }
    else
    {
        VMFrameInfo hm = vm.frame_info[e.fun_name.lexeme()];
        if (e.args.size() + hm.default_values.size() != hm.arg_count){
            // error!
        }
        if(e.keyword_values){
            // for(std::string name : hm.parameter_names){
            for(int j = 0; j < hm.parameter_names.size(); j ++){
                // we are now going through each of the parameter names
                // we are going to match them with their respective default 
                
                // get the index 
                int index = -1;
                for(int i =0; i < e.keyword_args.size(); i ++){
                    std::string uh = e.args.at(i).first_token().lexeme();
                    if (e.args.at(i).first_token().lexeme() == hm.parameter_names.at(j)){
                        index = i;
                        break;
                    }
                }
                if (index == -1){
                    // error! didnt find the keyword arg
                    int index = hm.parameter_names.size() - hm.default_values.size() - j;
                    hm.default_values.at(abs(index)).accept(*this);

                    // int first_default_index_needed = hm.arg_count - e.args.size() - hm.default_values.size();
                    // for(int i = first_default_index_needed; i < hm.default_values.size(); i ++){
                    //     hm.default_values.at(i).accept(*this);
                    // }
                    // break;
                }else{
                    e.keyword_args.at(index).accept(*this);
                }
            }
        }else{
            int first_default_index_needed = hm.arg_count - e.args.size() - hm.default_values.size();
            for(int i = first_default_index_needed; i < hm.default_values.size(); i ++){
                hm.default_values.at(i).accept(*this);
            }
        }
        curr_frame.instructions.push_back(VMInstr::CALL(e.fun_name.lexeme()));
    }
}

void CodeGenerator::visit(Expr &e)
{
    e.first->accept(*this);
    if (e.negated)
        curr_frame.instructions.push_back(VMInstr::NOT());
    if (e.op.has_value())
    {
        e.rest.get()->accept(*this);
        if (e.op.value().type() == TokenType::PLUS)
            curr_frame.instructions.push_back(VMInstr::ADD());
        else if (e.op.value().type() == TokenType::MINUS)
            curr_frame.instructions.push_back(VMInstr::SUB());
        else if (e.op.value().type() == TokenType::TIMES)
            curr_frame.instructions.push_back(VMInstr::MUL());
        else if (e.op.value().type() == TokenType::DIVIDE)
            curr_frame.instructions.push_back(VMInstr::DIV());
        else if (e.op.value().type() == TokenType::LESS)
            curr_frame.instructions.push_back(VMInstr::CMPLT());
        else if (e.op.value().type() == TokenType::LESS_EQ)
            curr_frame.instructions.push_back(VMInstr::CMPLE());
        else if (e.op.value().type() == TokenType::GREATER)
            curr_frame.instructions.push_back(VMInstr::CMPGT());
        else if (e.op.value().type() == TokenType::GREATER_EQ)
            curr_frame.instructions.push_back(VMInstr::CMPGE());
        else if (e.op.value().type() == TokenType::NOT_EQUAL)
            curr_frame.instructions.push_back(VMInstr::CMPNE());
        else if (e.op.value().type() == TokenType::EQUAL)
            curr_frame.instructions.push_back(VMInstr::CMPEQ());
        else if (e.op.value().type() == TokenType::AND)
            curr_frame.instructions.push_back(VMInstr::AND());
        else if (e.op.value().type() == TokenType::OR)
            curr_frame.instructions.push_back(VMInstr::OR());
    }
}

void CodeGenerator::visit(SimpleTerm &t)
{
    t.rvalue->accept(*this);
}

void CodeGenerator::visit(ComplexTerm &t)
{
    t.expr.accept(*this);
}

void CodeGenerator::visit(SimpleRValue &v)
{
    if (v.value.type() == TokenType::INT_VAL)
    {
        int val = stoi(v.value.lexeme());
        VMInstr instr = VMInstr::PUSH(val);
        instr.set_comment("Pushed value " + v.value.lexeme());
        curr_frame.instructions.push_back(instr);
    }
    else if (v.value.type() == TokenType::DOUBLE_VAL)
    {
        double val = stod(v.value.lexeme());
        VMInstr instr = VMInstr::PUSH(val);
        instr.set_comment("Pushed value " + v.value.lexeme());
        curr_frame.instructions.push_back(instr);
    }
    else if (v.value.type() == TokenType::NULL_VAL)
    {
        VMInstr instr = VMInstr::PUSH(nullptr);
        instr.set_comment("Pushing nullptr");
        curr_frame.instructions.push_back(instr);
    }
    else if (v.value.type() == TokenType::BOOL_VAL)
    {
        if (v.value.lexeme() == "true")
        {
            VMInstr instr = VMInstr::PUSH(true);
            instr.set_comment("Pushed true");
            curr_frame.instructions.push_back(instr);
        }
        else
        {
            VMInstr instr = VMInstr::PUSH(false);
            instr.set_comment("Pushed false");
            curr_frame.instructions.push_back(instr);
        }
    }
    else if (v.value.type() == TokenType::STRING_VAL)
    {
        string s = v.value.lexeme();
        replace_all(s, "\\n", "\n");
        replace_all(s, "\\t", "\t");
        // could do more here
        VMInstr instr = VMInstr::PUSH(s);
        instr.set_comment("Pushed " + s);
        curr_frame.instructions.push_back(instr);
    }
    else if (v.value.type() == TokenType::CHAR_VAL)
    {
        string s = v.value.lexeme();
        replace_all(s, "\\n", "\n");
        replace_all(s, "\\t", "\t");
        // could do more here
        VMInstr instr = VMInstr::PUSH(s);
        instr.set_comment("Pushed " + s);
        curr_frame.instructions.push_back(instr);
    }
}

void CodeGenerator::visit(NewRValue &v)
{
    if (v.array_expr.has_value())
    {
        // push the length of the array
        v.array_expr.value().accept(*this);
        // push the default value of the array (nullptr)
        VMInstr instr = VMInstr::PUSH(nullptr);
        instr.set_comment("Pushed nullptr (the default value of the array)");
        curr_frame.instructions.push_back(instr);
        // push alloca
        curr_frame.instructions.push_back(VMInstr::ALLOCA());
    }
    else if (struct_defs.contains(v.type.lexeme()))
    {
        // allocs
        curr_frame.instructions.push_back(VMInstr::ALLOCS());
        // go through each of the structs values
        // get struct info
        StructDef s = struct_defs[v.first_token().lexeme()];
        for (auto &field : s.fields)
        {
            // dup (because we will be adding the value)
            VMInstr instr = VMInstr::DUP();
            instr.set_comment("Duplicating before modifying the struct (add a field)");
            curr_frame.instructions.push_back(instr);
            // addf the value
            instr = VMInstr::ADDF(field.var_name.lexeme());
            instr.set_comment("Setting the name of the field to " + field.var_name.lexeme());
            curr_frame.instructions.push_back(instr);
            // DUP
            instr = VMInstr::DUP();
            instr.set_comment("Duplicating before modifying the struct (set a field's value)");
            curr_frame.instructions.push_back(instr);
            // setf the value to null
            instr = VMInstr::PUSH(nullptr);
            instr.set_comment("The default value for the struct value");
            curr_frame.instructions.push_back(instr);
            instr = VMInstr::SETF(field.var_name.lexeme());
            instr.set_comment("Setting the field " + field.var_name.lexeme() + " to the value above");
            curr_frame.instructions.push_back(instr);
        }
    }
}

void CodeGenerator::visit(VarRValue &v)
{
    // lookup the value
    if (v.path.size() > 1)
    {
        // struct paths
        int main_oid = var_table.get(v.first_token().lexeme());
        VMInstr instr = VMInstr::LOAD(main_oid);
        instr.set_comment("Loading the varaible at " + to_string(main_oid) + " which is " + v.first_token().lexeme());
        curr_frame.instructions.push_back(instr);
        if (v.path.at(0).array_expr.has_value())
        {
            v.path.at(0).array_expr.value().accept(*this);
            curr_frame.instructions.push_back(VMInstr::GETI());
        }
        for (int i = 1; i < v.path.size(); i++)
        {
            instr = VMInstr::GETF(v.path.at(i).var_name.lexeme());
            instr.set_comment("Getting the field: " + v.path.at(i).var_name.lexeme());
            curr_frame.instructions.push_back(instr);
            if (v.path.at(i).array_expr.has_value())
            {
                v.path.at(i).array_expr.value().accept(*this);
                curr_frame.instructions.push_back(VMInstr::GETI());
            }
        }
    }
    else
    {
        int oid = var_table.get(v.first_token().lexeme());
        VMInstr instr = VMInstr::LOAD(oid);
        instr.set_comment("Loading the varaible at " + to_string(oid) + " which is " + v.first_token().lexeme());
        curr_frame.instructions.push_back(instr);
        if (v.path.at(0).array_expr.has_value())
        {
            v.path.at(0).array_expr.value().accept(*this);
            curr_frame.instructions.push_back(VMInstr::GETI());
        }
    }
}
