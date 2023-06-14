//----------------------------------------------------------------------
// FILE: semantic_checker.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Ben Puryear
// DESC: Implements a semantic checker for MyPL
//----------------------------------------------------------------------

#include <unordered_set>
#include "mypl_exception.h"
#include "semantic_checker.h"


using namespace std;

// hash table of names of the base data types and built-in functions
const unordered_set<string> BASE_TYPES {"int", "double", "char", "string", "bool"};
const unordered_set<string> BUILT_INS {"print", "input", "to_string",  "to_int",
  "to_double", "length", "get", "concat"};


// helper functions

optional<VarDef> SemanticChecker::get_field(const StructDef& struct_def,
                                            const string& field_name)
{
  for (const VarDef& var_def : struct_def.fields)
    if (var_def.var_name.lexeme() == field_name)
      return var_def;
  return nullopt;
}


void SemanticChecker::error(const string& msg, const Token& token)
{
  string s = msg;
  s += " near line " + to_string(token.line()) + ", ";
  s += "column " + to_string(token.column());
  throw MyPLException::StaticError(s);
}


void SemanticChecker::error(const string& msg)
{
  throw MyPLException::StaticError(msg);
}


// visitor functions


void SemanticChecker::visit(Program& p)
{
  // record each struct def
  for (StructDef& d : p.struct_defs) {
    string name = d.struct_name.lexeme();
    if (struct_defs.contains(name))
      error("multiple definitions of '" + name + "'", d.struct_name);
    struct_defs[name] = d;
  }
  // record each function def (need a main function)
  bool found_main = false;
  for (FunDef& f : p.fun_defs) {
    string name = f.fun_name.lexeme();
    if (BUILT_INS.contains(name))
      error("redefining built-in function '" + name + "'", f.fun_name);
    if (fun_defs.contains(name))
      error("multiple definitions of '" + name + "'", f.fun_name);
    if (name == "main") {
      if (f.return_type.type_name != "void")
        error("main function must have void type", f.fun_name);
      if (f.params.size() != 0)
        error("main function cannot have parameters", f.params[0].var_name);
      found_main = true;
    }
    fun_defs[name] = f;
  }
  if (!found_main)
    error("program missing main function");
  // check each struct
  for (StructDef& d : p.struct_defs)
    d.accept(*this);
  // check each function
  for (FunDef& d : p.fun_defs)
    d.accept(*this);
}


void SemanticChecker::visit(SimpleRValue& v)
{
  if (v.value.type() == TokenType::INT_VAL)
    curr_type = DataType {false, "int"};
  else if (v.value.type() == TokenType::DOUBLE_VAL)
    curr_type = DataType {false, "double"};
  else if (v.value.type() == TokenType::CHAR_VAL)
    curr_type = DataType {false, "char"};
  else if (v.value.type() == TokenType::STRING_VAL)
    curr_type = DataType {false, "string"};
  else if (v.value.type() == TokenType::BOOL_VAL)
    curr_type = DataType {false, "bool"};
  else if (v.value.type() == TokenType::NULL_VAL)
    curr_type = DataType {false, "void"};
}


// TODO: Implement the rest of the visitor functions (stubbed out below)

void SemanticChecker::visit(FunDef& f)
{
    // check to see it is a valid return type
    if(!(BASE_TYPES.contains(f.return_type.type_name) || struct_defs.contains(f.return_type.type_name) || f.return_type.type_name == "void"))
        error("Invalid return type");
    symbol_table.push_environment();

    // add return type to the symbol_table
    symbol_table.add("return", f.return_type);

    // add the parameters to the symbole_table
    for(auto p: f.params){
        // check to see if it is a duplicate
        if(symbol_table.name_exists_in_curr_env(p.var_name.lexeme()))
            error("Multiple variables with the same name");
        // check to see it is a valid param
        if(BASE_TYPES.contains(p.data_type.type_name) || struct_defs.contains(p.data_type.type_name))
            symbol_table.add(p.var_name.lexeme(), p.data_type);
        else
            error("Invalid param name");
    }

    // handle all the statements
    for (auto s : f.stmts)
        s->accept(*this);
    symbol_table.pop_environment();
}


void SemanticChecker::visit(StructDef& s)
{
    unordered_set<string> field_names = unordered_set<string>();
    // go through each field
    for(auto f : s.fields){
        if(field_names.contains(f.var_name.lexeme()))
            error("Redeclaration of a variable in a struct def");
        else
            field_names.insert(f.var_name.lexeme());
        if(f.data_type.type_name == "void")
            error("Structs can not have fields with type void");
        if(!BASE_TYPES.contains(f.data_type.type_name))
            if(!(symbol_table.name_exists(f.data_type.type_name) || struct_defs.contains(f.data_type.type_name)))
                error("Invalid type");
    }
}


void SemanticChecker::visit(ReturnStmt& s)
{
    // get the functions return type
    DataType return_type = symbol_table.get("return").value();

    // check the type against the expression and other stuff, etc...
    s.expr.accept(*this); // this will set the value of curr_type
    if(curr_type.type_name != return_type.type_name && curr_type.type_name != "void")
        error("Return statements return type '" + curr_type.type_name + "' does not match the function return type '" + return_type.type_name + "'.");

    if(return_type.type_name == "void" && curr_type.type_name != "void")
        error("Void functions can not have any return statements in them");

    // check to see it is a valid return type
    if(!(BASE_TYPES.contains(curr_type.type_name) || struct_defs.contains(curr_type.type_name) || curr_type.type_name == "void"))
        error("Invalid return type");
}


void SemanticChecker::visit(WhileStmt& s)
{
    // check the expr in the while(e) is a bool
    s.condition.accept(*this);
    if(curr_type.type_name != "bool")
        error("while statements must be 'while(e)' with e being a boolean expression.");

    // handle all of the statements
    symbol_table.push_environment();
    for (auto s : s.stmts)
        s->accept(*this);
    symbol_table.pop_environment();
}


void SemanticChecker::visit(ForStmt& s)
{
    symbol_table.push_environment();
    s.var_decl.accept(*this); // this SHOULD (maybe) take care of the token mismatch types (it can be either t or void)
    s.condition.accept(*this);
    if(curr_type.type_name != "bool")
        error("Condition in forstmt should be bool");
    s.assign_stmt.accept(*this);
    for (auto s : s.stmts)
        s->accept(*this);
    symbol_table.pop_environment();
}

void SemanticChecker::visit(IfStmt& s)
{
    // if part
    s.if_part.condition.accept(*this);
    if(curr_type.type_name != "bool" || curr_type.is_array == true)
        error("if statements must be 'if(e)' with e being a boolean expression.");

    // handle all of the statements in the if part
    symbol_table.push_environment();
    for (auto s : s.if_part.stmts)
        s->accept(*this);
    symbol_table.pop_environment();

    // else if part
    for(auto b : s.else_ifs){
        b.condition.accept(*this);
        if(curr_type.type_name != "bool")
            error("else if statements must be 'elseif(e)' with e being a boolean expression.");
        // handle all of the statements in the if part
        symbol_table.push_environment();
        for (auto bs : b.stmts)
            bs->accept(*this);
        symbol_table.pop_environment();
    }

    // else part
    if(s.else_stmts.size() != 0){
        symbol_table.push_environment();
        for (auto l : s.else_stmts)
            l->accept(*this);
        symbol_table.pop_environment();
    }
}


void SemanticChecker::visit(VarDeclStmt& s)
{
    DataType dt = s.var_def.data_type;
    string name = s.var_def.var_name.lexeme();
    if(symbol_table.name_exists_in_curr_env(name))
        error("Cannot attempt to shadow in the same environment.");
    s.expr.accept(*this);
    if(curr_type.type_name != dt.type_name && curr_type.type_name != "void" || (dt.is_array != curr_type.is_array && !(curr_type.type_name == "void")))
        error("Type of right side of VarDeclStmt does not equal the type of the left side.");
    symbol_table.add(name, dt);
}


void SemanticChecker::visit(AssignStmt& s)
{
    DataType lval_type;
    bool isStruct = false;
    StructDef struct_def;

    for(auto l : s.lvalue){
        if(isStruct){
            if(get_field(struct_def, l.var_name.lexeme()) != nullopt){
                lval_type = get_field(struct_def, l.var_name.lexeme()).value().data_type;
                if(!struct_defs.contains(lval_type.type_name))
                    isStruct = false;
                else
                    struct_def = struct_defs[lval_type.type_name];
            }else
                error("The field is not present in the struct.");
            if(l.array_expr.has_value()){
                l.array_expr.value().accept(*this);
                if(curr_type.type_name != "int")
                    error("Only integers can be used to index into an array.");
                lval_type.is_array = false; // no multi-dim arrays
            }
        }else{
            if(!symbol_table.name_exists(l.var_name.lexeme()))
                error("Var does not exist");
            lval_type = symbol_table.get(l.var_name.lexeme()).value();
            if(struct_defs.contains(lval_type.type_name)){
                // it is a struct, so we continue with the path
                isStruct = true;
                struct_def = struct_defs[lval_type.type_name];
            }
            if(l.array_expr.has_value()){
                l.array_expr.value().accept(*this);
                if(curr_type.type_name != "int")
                    error("Only integers can be used to index into an array.");
                lval_type.is_array = false; // no multi-dim arrays
            }
        }
    }
    s.expr.accept(*this);
    if((curr_type.type_name != lval_type.type_name) || (curr_type.is_array != lval_type.is_array))
        error("Type mismatch");

}


void SemanticChecker::visit(CallExpr& e)
{
    string fun_name = e.fun_name.lexeme();
    // check built-in types {"print", "input", "to_string",  "to_int", "to_double", "length", "get", "concat"};
    if (fun_name == "print"){
        if (e.args.size() != 1)
            error("print() takes 1 arguments, but more or less than 1 were provided.");
        e.args[0].accept(*this);
        if(!BASE_TYPES.contains(curr_type.type_name) || curr_type.is_array == true)
            error("print() only takes in base types. A non-base type value was passed in. '" + curr_type.type_name+"'.");
        curr_type = DataType {false, "void"};
    }
    else if (fun_name == "input"){
        // check for no parameters
        if (e.args.size() != 0)
            error("input() takes 0 arguments, but more than 0 were provided.");
        curr_type = DataType {false, "string"};
    }
    else if (fun_name == "to_string"){
        if(e.args.size() != 1)
            error("to_double() takes 1 arguments, but a more or less than 1 were provided.");
        e.args[0].accept(*this);
        // parameter can be int, double, char
        if(!((curr_type.type_name == "int") || (curr_type.type_name == "double") || (curr_type.type_name == "char") || (curr_type.type_name == "string")))
            error("to_string() only takes in either an int, double, or a char.");
        curr_type = DataType {false, "string"};
    }
    else if (fun_name == "to_int"){
        // parameter can be double or string
        if(e.args.size() != 1)
            error("to_int() takes 1 arguments, but a more or less than 1 were provided.");
        e.args[0].accept(*this);
        // parameter can be double or string
        if(!((curr_type.type_name == "double") || (curr_type.type_name == "string")))
            error("to_int() only takes in either an double or a string.");
        curr_type = DataType {false, "int"};
    }
    else if (fun_name == "to_double"){
        if(e.args.size() != 1)
            error("to_double() takes 1 arguments, but a more or less than 1 were provided.");
        e.args[0].accept(*this);
        // parameter can be int or string
        if(!((curr_type.type_name == "int") || (curr_type.type_name == "string")))
            error("to_double() only takes in either an int or a string.");
        curr_type = DataType {false, "double"};
    }
    else if (fun_name == "length"){
        if (e.args.size() != 1)
            error("length() takes 1 arguments, but more or less than 1 were provided.");
        e.args[0].accept(*this);
        if(curr_type.type_name != "string" && curr_type.is_array != true)
            error("length() only takes in string. A non-string value was passed in.");
        curr_type = DataType {false, "int"};
    }
    else if (fun_name == "get"){
        if (e.args.size() != 2)
            error("get() takes 2 arguments, but more or less than 2 were provided.");
        e.args[0].accept(*this);
        if(curr_type.type_name != "int")
            error("Parameter 1 in get() can only be an int. A non-int value was passed in.");
        e.args[1].accept(*this);
        if(curr_type.type_name != "string")
            error("Parameter 2 in get() can only be an string. A non-string value was passed in.");
        curr_type = DataType {false, "char"};
    }
    else if (fun_name == "concat"){
        if (e.args.size() != 2)
            error("concat() takes 2 arguments, but more or less than 2 were provided.");
        e.args[0].accept(*this);
        if(curr_type.type_name != "string")
            error("Parameter 1 in concat() can only be an string. A non-string value was passed in.");
        e.args[1].accept(*this);
        if(curr_type.type_name != "string")
            error("Parameter 2 in concat() can only be an string. A non-string value was passed in.");
        curr_type = DataType {false, "string"};
    }
    else {
        // must be a custom function
        if (!fun_defs.contains(fun_name))
            error("Function '" + fun_name + "' is not defined.");
        FunDef& f = fun_defs[fun_name];

        // make sure same number of args as params
        if (e.args.size() != f.params.size())
            error("Different number of arguments for unction '" + f.fun_name.lexeme() + "'.");
        // check each arg type:
        for (int i = 0; i < e.args.size(); ++i) {
            DataType param_type = f.params[i].data_type;
            e.args[i].accept(*this);
            // now that the value of curr_type is set, we can compare them
            if(param_type.type_name != curr_type.type_name && (curr_type.type_name != "void")){
                printf("%d",i);
                error("Invalid type passed in as a parameter. Passed in '" + curr_type.type_name +"' but expected '" + param_type.type_name + "'.");
            }
        }

        // at this point, there hasnt been any errors, so we can just set the val of curr_type!
        curr_type = f.return_type; // might be here
    }
}


void SemanticChecker::visit(Expr& e)
{
    // check the first term
    e.first->accept(*this);
    // record the lhs type
    DataType lhs_type = curr_type;
    // check if more to expression
    if (e.op.has_value()) {
        // check rest of expression
        e.rest->accept(*this);
        // record the rhs type
        DataType rhs_type = curr_type;
        // we have to make sure the types are compatable
        if(rhs_type.type_name != lhs_type.type_name && !(rhs_type.type_name != "void" || lhs_type.type_name != "void"))
            error("Invalid operation between two types '" + lhs_type.type_name + "' and '" + rhs_type.type_name + "' with op '" + e.op.value().lexeme());

        // THESE ARE ALL THE SAME TYPE AS THE LHS_TYPE

        // d+d,d-d,d/d,d*d
        // i+i,i-i,i/i,i*i
        if(e.op.value().type() == TokenType::PLUS){
            if (!((rhs_type.type_name == lhs_type.type_name) && ((rhs_type.type_name == "double") || (rhs_type.type_name == "int"))))
                error("The types must be the same as well as both either be of type double or type int when using PLUS.");
            curr_type = lhs_type;
        }else if(e.op.value().type() == TokenType::MINUS){
            if (!((rhs_type.type_name == lhs_type.type_name) && ((rhs_type.type_name == "double") || (rhs_type.type_name == "int"))))
                error("The types must be the same as well as both either be of type double or type int when using MINUS.");
            curr_type = lhs_type;
        }else if(e.op.value().type() == TokenType::DIVIDE){
            if (!((rhs_type.type_name == lhs_type.type_name) && ((rhs_type.type_name == "double") || (rhs_type.type_name == "int"))))
                error("The types must be the same as well as both either be of type double or type int when using DIVIDE.");
            curr_type = lhs_type;
        }else if(e.op.value().type() == TokenType::TIMES){
            if (!((rhs_type.type_name == lhs_type.type_name) && ((rhs_type.type_name == "double") || (rhs_type.type_name == "int"))))
                error("The types must be the same as well as both either be of type double or type int when using TIMES.");
            curr_type = lhs_type;
        }

        // b&&b,b||b
        if(e.op.value().type() == TokenType::AND){
            if(!(rhs_type.type_name == "bool" && lhs_type.type_name == "bool"))
                error("The types must both be bool when using AND.");
            curr_type = lhs_type;
        }else if(e.op.value().type() == TokenType::OR){
            if(!(rhs_type.type_name == "bool" && lhs_type.type_name == "bool"))
                error("The types must both be bool when using OR.");
            curr_type = lhs_type;
        }

        // THESE ARE BOOL!

        // type1==(type1||void),type1!=(type1||void),
        if(e.op.value().type() == TokenType::EQUAL){
            if(!(rhs_type.type_name == lhs_type.type_name || ((rhs_type.type_name == "void" && lhs_type.type_name != "void") || (rhs_type.type_name != "void" && lhs_type.type_name == "void"))) || (rhs_type.is_array != lhs_type.is_array && rhs_type.type_name != "void" && lhs_type.type_name != "void"))
                error("The types must both be the same, or ONE of them is void and the other is not when using EQUAL.");
            curr_type = DataType {false, "bool"};

        }else if(e.op.value().type() == TokenType::NOT_EQUAL){
            if(!(rhs_type.type_name == lhs_type.type_name || ((rhs_type.type_name == "void" && lhs_type.type_name != "void") || (rhs_type.type_name != "void" && lhs_type.type_name == "void"))))
                error("The types must both be the same, or ONE of them is void and the other is not when using NOT_EQUAL.");
            curr_type = DataType {false, "bool"};
        }

        // i<i,i<=i,i>i,i>=i
        // d<d,d<=d,d>d,d>=d
        // c<c,c<=c,c>c,c>=c
        // s<s,s<=s,s>s,s>=s
        if(e.op.value().type() == TokenType::GREATER){
            if (!((rhs_type.type_name == lhs_type.type_name) && (
                (rhs_type.type_name == "double") ||
                (rhs_type.type_name == "int") ||
                (rhs_type.type_name == "char") ||
                (rhs_type.type_name == "string")
            )))
                error("The types must be the same as well as both either be of type double or type int when using GREATER.");
            curr_type = DataType {false, "bool"};
        }else if(e.op.value().type() == TokenType::GREATER_EQ){
            if (!((rhs_type.type_name == lhs_type.type_name) && (
                (rhs_type.type_name == "double") ||
                (rhs_type.type_name == "int") ||
                (rhs_type.type_name == "char") ||
                (rhs_type.type_name == "string")
            )))
                error("The types must be the same as well as both either be of type double or type int when using GREATER_EQ.");
            curr_type = DataType {false, "bool"};
        }else if(e.op.value().type() == TokenType::LESS){
            if (!((rhs_type.type_name == lhs_type.type_name) && (
                (rhs_type.type_name == "double") ||
                (rhs_type.type_name == "int") ||
                (rhs_type.type_name == "char") ||
                (rhs_type.type_name == "string")
            )))
                error("The types must be the same as well as both either be of type double or type int when using LESS.");
            curr_type = DataType {false, "bool"};
        }else if(e.op.value().type() == TokenType::LESS_EQ){
            if (!((rhs_type.type_name == lhs_type.type_name) && (
                (rhs_type.type_name == "double") ||
                (rhs_type.type_name == "int") ||
                (rhs_type.type_name == "char") ||
                (rhs_type.type_name == "string")
            )))
                error("The types must be the same as well as both either be of type double or type int when using LESS_EQ.");
            curr_type = DataType {false, "bool"};
        }
    }else
        curr_type = lhs_type;

    // check to make sure that only bools can be negated
    if(e.negated){
        if(curr_type.type_name != "bool"){
            error("Only bool values can be negated");
        }
    }
}


void SemanticChecker::visit(SimpleTerm& t)
{
    t.rvalue->accept(*this);
}


void SemanticChecker::visit(ComplexTerm& t)
{
    t.expr.accept(*this);
}


void SemanticChecker::visit(NewRValue& v)
{
    // form the dt the new r value should be
    string type_lexeme = v.type.lexeme();
    bool isArray = v.array_expr.has_value();
    if(isArray && type_lexeme == "void")
        error("Arrays can not be of type void");
    if(v.array_expr.has_value()){
        v.array_expr.value().accept(*this);
        if(curr_type.type_name != "int")
            error("Type mismatch");
    }
    DataType dt = DataType {isArray, type_lexeme};

    curr_type = dt;
}


void SemanticChecker::visit(VarRValue& v)
{
     DataType lval_type;
    bool isStruct = false;
    StructDef struct_def;

    for(auto l : v.path){
        if(isStruct){
            if(get_field(struct_def, l.var_name.lexeme()) != nullopt){
                lval_type = get_field(struct_def, l.var_name.lexeme()).value().data_type;
                if(!struct_defs.contains(lval_type.type_name))
                    isStruct = false;
                else
                    struct_def = struct_defs[lval_type.type_name];
            }else
                error("The field is not present in the struct.");
        }else{
            if(!symbol_table.name_exists(l.var_name.lexeme()))
                error("Var does not exist");
            lval_type = symbol_table.get(l.var_name.lexeme()).value();
            if(struct_defs.contains(lval_type.type_name)){
                // it is a struct, so we continue with the path
                isStruct = true;
                struct_def = struct_defs[lval_type.type_name];
            }
            if(l.array_expr.has_value()){
                l.array_expr.value().accept(*this);
                if(curr_type.type_name != "int")
                    error("Only integers can be used to index into an array.");
                lval_type.is_array = false; // no multi-dim arrays
            }
        }
    }
    curr_type = lval_type;
}
