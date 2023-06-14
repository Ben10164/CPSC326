//----------------------------------------------------------------------
// FILE: ast_parser.h
// DATE: CPSC 326, Spring 2023
// AUTH:
// DESC:
//----------------------------------------------------------------------

#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "mypl_exception.h"
#include "lexer.h"
#include "ast.h"

class ASTParser
{
public:
    // crate a new recursive descent parer
    ASTParser(const Lexer &lexer);

    // run the parser
    Program parse();

private:
    Lexer lexer;
    Token curr_token;

    // helper functions
    void advance();
    void eat(TokenType t, const std::string &msg);
    bool match(TokenType t);
    bool match(std::initializer_list<TokenType> types);
    void error(const std::string &msg);
    bool bin_op();

    // recursive descent functions
    void struct_def(Program &p);
    void fun_def(Program &s);

    void fields(std::vector<VarDef> &fields);
    bool data_type(DataType &datatype);
    void params(std::vector<VarDef> &params);
    bool base_type();
    void stmt(std::vector<std::shared_ptr<Stmt>> &stmts);
    void l_value(std::vector<VarRef> &lvalue, Token prevtoken);
    void assign_stmt(std::shared_ptr<AssignStmt> &assingstmt, Token prevtoken);
    void if_stmt(std::shared_ptr<IfStmt> &ifstmt);
    void if_stmt_t(std::shared_ptr<IfStmt> &ifstmt);
    void while_stmt(std::shared_ptr<WhileStmt> &whilestmt);
    void for_stmt(std::shared_ptr<ForStmt> &forstmt);
    void vdecl_stmt(std::shared_ptr<VarDeclStmt> &vardeclstmt);
    void vdecl_stmt_no_pre_ID(std::shared_ptr<VarDeclStmt> &vardeclstmt);
    void ret_stmt(std::shared_ptr<ReturnStmt> &returnstmt);
    bool data_type_minus_ID(DataType &datatype);
    void new_rvalue(std::shared_ptr<NewRValue> &newrvalue);
    void var_rvalue(std::shared_ptr<VarRValue> &varrvalue, Token prevtoken);
    bool base_rvalue();
    void r_value(std::shared_ptr<RValue>& rvalue);
    void call_expr(std::shared_ptr<CallExpr>& callexpr, Token  prevtoken);

    void expr(Expr& expression);
};

#endif
