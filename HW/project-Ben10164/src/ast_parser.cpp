//----------------------------------------------------------------------
// FILE: ast_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH:
// DESC:
//----------------------------------------------------------------------

#include "ast_parser.h"

using namespace std;

ASTParser::ASTParser(const Lexer &a_lexer)
    : lexer{a_lexer}
{
}

void ASTParser::advance()
{
    curr_token = lexer.next_token();
}

void ASTParser::eat(TokenType t, const string &msg)
{
    if (!match(t))
        error(msg);
    advance();
}

bool ASTParser::match(TokenType t)
{
    return curr_token.type() == t;
}

bool ASTParser::match(initializer_list<TokenType> types)
{
    for (auto t : types)
        if (match(t))
            return true;
    return false;
}

void ASTParser::error(const string &msg)
{
    string s = msg + " found '" + curr_token.lexeme() + "' ";
    s += "at line " + to_string(curr_token.line()) + ", ";
    s += "column " + to_string(curr_token.column());
    throw MyPLException::ParserError(s);
}

bool ASTParser::bin_op()
{
    return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
                  TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
                  TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
                  TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}

Program ASTParser::parse()
{
    Program p;
    advance();
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::STRUCT))
            struct_def(p);
        else
            fun_def(p);
    }
    eat(TokenType::EOS, "expecting end-of-file");
    return p;
}

void ASTParser::struct_def(Program &p)
{
    // STRUCT ID LBRACE <fields> RBRACE
    StructDef struct_def;
    eat(TokenType::STRUCT, "Expected a Struct");
    struct_def.struct_name = curr_token;
    eat(TokenType::ID, "Expected an ID to be the name of the struct");
    eat(TokenType::LBRACE, "Expected a LBrace");
    fields(struct_def.fields); // calls the fields function. this will set the value of fields
    eat(TokenType::RBRACE, "Expected a RBrace");
    p.struct_defs.push_back(struct_def);
}

void ASTParser::fun_def(Program &p)
{
    // (<data_type> | VOID_TYPE) ID LPAREN <params> RPAREN LBRACE (<stmt>)* RBRACE
    FunDef fun_def;
    fun_def.params = std::vector<VarDef>();
    DataType return_type;

    if (data_type(return_type) || match(TokenType::VOID_TYPE)) // if it is a datatype, or void
    {
        // since data_type() advances, if its void we need to advance manually
        if (match(TokenType::VOID_TYPE))
        {
            return_type.is_array = false;
            return_type.type_name = curr_token.lexeme();
            advance();
        }
        fun_def.return_type = return_type;
        Token fun_name;
        fun_name = curr_token;
        eat(TokenType::ID, "Expected an ID");
        fun_def.fun_name = fun_name;

        eat(TokenType::LPAREN, "Expected a LParen");
        params(fun_def.params); // calls the params
        eat(TokenType::RPAREN, "Expected a RParen");
        eat(TokenType::LBRACE, "Expected a LBrace");
        // handle the (<stmt>)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt(fun_def.stmts);
        }
        eat(TokenType::RBRACE, "Expected a RBrace"); // redundant, could also use advance
    }
    else
    {
        error("Expected a <data_type> or VOID_TYPE");
    }
    p.fun_defs.push_back(fun_def);
}

// TODO: Finish rest of parser based on your simple parser
// implementation

void ASTParser::params(std::vector<VarDef> &params)
{
    // <data_type> ID(COMMA <data_type> ID)*|empty
    // make sure it is a <data_type>
    VarDef var_def;
    DataType datatype;
    bool seen_default = false;
    if (data_type(datatype))
    {
        var_def.data_type = datatype;
        var_def.var_name = curr_token;
        eat(TokenType::ID, "Expected an ID");
        if(match(TokenType::ASSIGN)){
            // this means there is a default value
            eat(TokenType::ASSIGN, "Expected an EQUAL");
            Expr tmpExpr;
            expr(tmpExpr);
            var_def.defaultExpr = tmpExpr;
            seen_default = true;
        }
        params.push_back(var_def);
        // handle the (COMMA <data_type> ID)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::COMMA))
            {
                VarDef tempVD;
                DataType tempDT;
                eat(TokenType::COMMA, "Expected a COMMA"); // redundant, could also use advance
                if (data_type(tempDT))
                {
                    tempVD.data_type = tempDT;
                    tempVD.var_name = curr_token;
                    // here
                    eat(TokenType::ID, "Expected an ID");
                    if(match(TokenType::ASSIGN)){
                        seen_default = true;
                    }
                    if (seen_default){
                        // this means there is a default value
                        eat(TokenType::ASSIGN, "Default values must be at the end of functions parameters.");
                        Expr tmpExpr;
                        expr(tmpExpr);
                        tempVD.defaultExpr = tmpExpr;
                    }
                    params.push_back(tempVD);
                }
                else
                {
                    error("Expected a <data_type>");
                }
            }
            else
            {
                break;
            }
        }
    }
}

void ASTParser::fields(std::vector<VarDef> &fields)
{
    // <data_type> ID (COMMA <data_type> ID)* | empty
    // make sure it is a <data_type>
    VarDef var_def;
    DataType datatype;
    if (data_type(datatype))
    {
        var_def.data_type = datatype;
        var_def.var_name = curr_token;
        eat(TokenType::ID, "Expected an ID");
        fields.push_back(var_def);
        // handle the (COMMA <data_type> ID)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::COMMA))
            {
                VarDef tempVD;
                DataType tempDT;
                eat(TokenType::COMMA, "Expected a COMMA"); // redundant, could also use advance
                if (data_type(tempDT))
                {
                    tempVD.data_type = tempDT;
                    tempVD.var_name = curr_token;
                    eat(TokenType::ID, "Expected an ID");
                    fields.push_back(tempVD);
                }
                else
                {
                    error("Expected a <data_type>");
                }
            }
            else
            {
                break;
            }
        }
    }
}

bool ASTParser::data_type(DataType &datatype)
{
    // <base_type>|ID|ARRAY(<base_type>|ID)
    if (base_type())
    {
        datatype.type_name = curr_token.lexeme();
        datatype.is_array = false;
        advance();
        return true;
    }
    else if (match(TokenType::ID))
    {
        datatype.type_name = curr_token.lexeme();
        datatype.is_array = false;
        advance();
        return true;
    }
    else if (match(TokenType::ARRAY))
    {
        datatype.is_array = true;
        advance();
        // array is a multi-token data_type, so we need to check the next
        if (base_type())
        {
            datatype.type_name = curr_token.lexeme();
            advance();
            return true;
        }
        else if (match(TokenType::ID))
        {
            datatype.type_name = curr_token.lexeme();
            advance();
            return true;
        }
    }
    return false;
}

void ASTParser::stmt(std::vector<std::shared_ptr<Stmt>> &stmts)
{
    // <vdecl_stmt>|<assign_stmt>|<if_stmt>|<while_stmt>|<for_stmt>|<call_expr>|<ret_stmt>

    // VarDeclStmt vardeclstmt;
    std::shared_ptr<VarDeclStmt> vardeclstmt = make_shared<VarDeclStmt>();
    VarDef vardef;
    if (data_type_minus_ID(vardef.data_type)) // could be ID
    {
        vardeclstmt->var_def = vardef;
        // do this
        vdecl_stmt_no_pre_ID(vardeclstmt);
        stmts.push_back(vardeclstmt);
    }
    else if (match(TokenType::IF))
    {
        std::shared_ptr<IfStmt> ifstmt = make_shared<IfStmt>();
        if_stmt(ifstmt);
        stmts.push_back(ifstmt);
    }
    else if (match(TokenType::WHILE))
    {
        std::shared_ptr<WhileStmt> whilestmt = make_shared<WhileStmt>();
        while_stmt(whilestmt);
        stmts.push_back(whilestmt);
    }
    else if (match(TokenType::FOR))
    {
        std::shared_ptr<ForStmt> forstmt = make_shared<ForStmt>();
        for_stmt(forstmt);
        stmts.push_back(forstmt);
    }
    else if (match(TokenType::ID)) // both <assign_stmt> and <call_expr> ALSO DATA_TYPE() could be an 
    {
        vardeclstmt->var_def.data_type.type_name = curr_token.lexeme();
        Token prevtoken = curr_token;
        eat(TokenType::ID, "Expected an ID");
        // need to check for vdecl_stmt
        if (match(TokenType::ID))
        {
            vdecl_stmt(vardeclstmt);
            stmts.push_back(vardeclstmt);
        }
        else if (match(TokenType::LPAREN))
        {
            std::shared_ptr<CallExpr> callexpr = make_shared<CallExpr>();
            call_expr(callexpr, prevtoken);
            stmts.push_back(callexpr);
        }
        else
        {
            // else because match() could be empty due to lvalue including ()*
            std::shared_ptr<AssignStmt> assignstmt = make_shared<AssignStmt>();
            assign_stmt(assignstmt, prevtoken);
            stmts.push_back(assignstmt);
        }
    }
    else if (match(TokenType::RETURN))
    {
        std::shared_ptr<ReturnStmt> returnstmt = make_shared<ReturnStmt>();
        ret_stmt(returnstmt);
        stmts.push_back(returnstmt);
    }
    else
    {
        error("Expected <vdecl_stmt>|<assign_stmt>|<if_stmt>|<while_stmt>|<for_stmt>|<call_expr>|<ret_stmt>");
    }
}

void ASTParser::assign_stmt(std::shared_ptr<AssignStmt> &assign_stmt, Token prevtoken)
{
    // <lvalue> ASSIGN <expr>
    // <lvalue> assumes the ID at the start is eaten
    l_value(assign_stmt->lvalue, prevtoken);
    eat(TokenType::ASSIGN, "Expected a ASSIGN");
    expr(assign_stmt->expr);
}

void ASTParser::l_value(vector<VarRef> &lvalue, Token prevtoken)
{
    // ID(DOT ID|LBRACKET <expr> RBRACKET)*
    VarRef varref;
    // varref.var_name = curr_token;
    varref.var_name = prevtoken;
    // eat(TokenType::ID, "Expected an ID");
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::DOT))
        {
            lvalue.push_back(varref);
            advance();
            if (varref.array_expr.has_value())
            {
                varref.array_expr.reset();
            }
            varref.var_name = curr_token;
            eat(TokenType::ID, "Expected an ID");
        }
        else if (match(TokenType::LBRACKET))
        {
            advance();
            Expr arrayexpr;
            expr(arrayexpr);
            varref.array_expr = arrayexpr;
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
        else
        {
            lvalue.push_back(varref);
            break;
        }
    }
}

void ASTParser::if_stmt(std::shared_ptr<IfStmt> &ifstmt)
{
    // IF LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE <if_stmt_t>
    BasicIf basicif;
    eat(TokenType::IF, "Expected a IF");
    eat(TokenType::LPAREN, "Expected a LPAREN");
    expr(basicif.condition);
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt(basicif.stmts);
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
    ifstmt->if_part = basicif;
    if_stmt_t(ifstmt);
}

void ASTParser::if_stmt_t(std::shared_ptr<IfStmt> &ifstmt)
{
    // ELSEIF LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE <if_stmt_t>|ELSE LBRACE(<stmt>)*RBRACE|empty
    if (match(TokenType::ELSEIF))
    {
        BasicIf basicif;
        eat(TokenType::ELSEIF, "Expected a ELSEIF");
        eat(TokenType::LPAREN, "Expected a LPAREN");
        Expr conditions;
        expr(conditions);
        basicif.condition = conditions;
        eat(TokenType::RPAREN, "Expected a RPAREN");
        eat(TokenType::LBRACE, "Expected a LBRACE");
        // handle the (<stmt>)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt(basicif.stmts);
            // ifstmt->else_ifs.push_back(basicif);
        }
        eat(TokenType::RBRACE, "Expected a RBRACE");
        ifstmt->else_ifs.push_back(basicif);
        if_stmt_t(ifstmt);
    }
    else if (match(TokenType::ELSE))
    {
        std::vector<std::shared_ptr<Stmt>> elsestmts;
        eat(TokenType::ELSE, "Expected a ELSE");
        eat(TokenType::LBRACE, "Expected a LBRACE");
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt(elsestmts);
        }
        ifstmt->else_stmts = elsestmts;
        eat(TokenType::RBRACE, "Expected a RBRACE");
    }
    // empty case; do nothing
}

void ASTParser::while_stmt(std::shared_ptr<WhileStmt> &whilestmt)
{
    // WHILE LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE
    eat(TokenType::WHILE, "Expected a WHILE");

    eat(TokenType::LPAREN, "Expected a LPAREN");

    expr(whilestmt->condition);
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt(whilestmt->stmts);
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
}

void ASTParser::for_stmt(std::shared_ptr<ForStmt> &forstmt)
{
    // FOR LPAREN <vdecl_stmt> SEMICOLON <expr> SEMICOLON<assign_stmt> <RPAREN> LBRACE(<stmt>)*RBRACE
    eat(TokenType::FOR, "Expected a FOR");
    eat(TokenType::LPAREN, "Expected a LPAREN");
    std::shared_ptr<VarDeclStmt> vardecstmt = make_shared<VarDeclStmt>();
    if (data_type(vardecstmt->var_def.data_type))
    {
        vdecl_stmt(vardecstmt);
        forstmt->var_decl = *vardecstmt.get();
    }
    else
    {
        error("Expected a <data_type>");
    }
    eat(TokenType::SEMICOLON, "Expected a SEMICOLON");
    Expr expression;
    expr(expression);
    forstmt->condition = expression;
    eat(TokenType::SEMICOLON, "Expected a SEMICOLON");
    std::shared_ptr<AssignStmt> assignstmt = make_shared<AssignStmt>();
    Token prevtoken = curr_token;
    // assign assumes the ID was eaten
    eat(TokenType::ID, "Expected an ID");
    assign_stmt(assignstmt, prevtoken);
    forstmt->assign_stmt = *assignstmt.get();
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt(forstmt->stmts);
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
}

void ASTParser::vdecl_stmt(std::shared_ptr<VarDeclStmt> &vardeclstmt)
{
    // <data_type> ID ASSIGN <expr>
    // this function is called when there is a <data_type> that is an ID
    // e.g. my_struct i = 0
    // assumes the <data_type> was dealt with
    if (match(TokenType::ASSIGN))
    {
        eat(TokenType::ASSIGN, "Expected a ASSIGN");
    }
    else if (match(TokenType::ID))
    {
        vardeclstmt->var_def.var_name = curr_token;
        eat(TokenType::ID, "Expected an ID");
        eat(TokenType::ASSIGN, "Expected a ASSIGN");
    }
    Expr expression;
    expr(expression);
    vardeclstmt->expr = expression;
}

void ASTParser::vdecl_stmt_no_pre_ID(std::shared_ptr<VarDeclStmt> &vardeclstmt)
{
    // <data_type> ID ASSIGN <expr>
    // this function is called when there is a <data_type> that is not an ID
    // e.g. int i = 0
    // assumes the <data_type> was dealt with
    vardeclstmt->var_def.var_name = curr_token;
    eat(TokenType::ID, "Expected an ID");
    eat(TokenType::ASSIGN, "Expected a ASSIGN");

    Expr expression;
    expr(expression);
    vardeclstmt->expr = expression;
}

void ASTParser::ret_stmt(std::shared_ptr<ReturnStmt> &returnstmt)
{
    // RETURN <expr>
    eat(TokenType::RETURN, "Expected a RETURN");
    Expr expression;
    expr(expression);
    returnstmt->expr = expression;
}

bool ASTParser::data_type_minus_ID(DataType &datatype)
{
    // <base_type>|ID|ARRAY(<base_type>|ID)
    if (base_type())
    {
        datatype.is_array = false;
        datatype.type_name = curr_token.lexeme();
        advance();
        return true;
    }
    else if (match(TokenType::ARRAY))
    {
        datatype.is_array = true;
        advance();
        // array is a multi-token data_type, so we need to check the next
        if (base_type())
        {
            datatype.type_name = curr_token.lexeme();
            advance();
            return true;
        }
        else if (match(TokenType::ID))
        {
            datatype.type_name = curr_token.lexeme();
            advance();
            return true;
        }
    }
    return false;
}

bool ASTParser::base_rvalue()
{
    // INT_VAL|DOUBLE_VAL|BOOL_VAL|CHAR_VAL|STRING_VAL
    return match({TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL});
}

void ASTParser::new_rvalue(std::shared_ptr<NewRValue> &newrvalue)
{
    // NEW ID(LBRACKET <expr> RBRACKET|empty)|NEW <base_type> LBRACKET <expr> RBRACKET
    // both start off with NEW
    eat(TokenType::NEW, "Expected a NEW");
    if (match(TokenType::ID))
    {
        // ID(LBRACKET <expr> RBRACKET|empty)
        newrvalue->type = curr_token;
        eat(TokenType::ID, "Expected an ID");
        if (match(TokenType::LBRACKET))
        {
            eat(TokenType::LBRACKET, "Expected a LBRACKET");
            Expr expression;
            expr(expression);
            newrvalue->array_expr = expression;
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
    }
    else if (base_type())
    {
        // <base_type> LBRACKET <expr> RBRACKET
        newrvalue->type = curr_token;
        advance();
        eat(TokenType::LBRACKET, "Expected a LBRACKET");
        Expr expression;
        expr(expression);
        newrvalue->array_expr = expression;
        eat(TokenType::RBRACKET, "Expected a RBRACKET");
    }
}

void ASTParser::var_rvalue(std::shared_ptr<VarRValue> &varrvalue, Token prevtoken)
{
    VarRef varref;
    varrvalue->path = std::vector<VarRef>();
    varref.var_name = prevtoken;
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::DOT))
        {
            varrvalue->path.push_back(varref);
            advance();
            if (varref.array_expr.has_value())
            {
                varref.array_expr.reset();
            }
            varref.var_name = curr_token;
            eat(TokenType::ID, "Expected an ID");
        }
        else if (match(TokenType::LBRACKET))
        {
            advance();
            Expr expression;
            expr(expression);
            varref.array_expr = expression;
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
        else
        {
            varrvalue->path.push_back(varref);
            break;
        }
    }
}

void ASTParser::r_value(std::shared_ptr<RValue> &rvalue)
{
    // <base_rvalue>|NULL_VAL|<new_rvalue>|<var_rvalue>|<call_expr>
    if (base_rvalue())
    {
        shared_ptr<SimpleRValue> r = make_shared<SimpleRValue>();
        r->value = curr_token;
        rvalue = r;
        advance();
    }
    else if (match(TokenType::NULL_VAL))
    {
        shared_ptr<SimpleRValue> r = make_shared<SimpleRValue>();
        r->value = curr_token;
        rvalue = r;
        eat(TokenType::NULL_VAL, "Expected a NULL_VAL");
    }
    else if (match(TokenType::NEW))
    {
        // new_rvalue
        shared_ptr<NewRValue> r = make_shared<NewRValue>();
        new_rvalue(r);
        rvalue = r;
    }
    else if (match(TokenType::ID))
    {
        // either a function call(call expr) or a VarRValue
        Token prevtoken = curr_token;
        std::shared_ptr<CallExpr> callexpr = make_shared<CallExpr>();
        std::shared_ptr<VarRValue> varrvalue = make_shared<VarRValue>();
        // var_rvalue AND call_expr
        eat(TokenType::ID, "Expected an ID");
        if (match(TokenType::LPAREN))
        {
            call_expr(callexpr, prevtoken);
            rvalue = callexpr;
        }
        else
        {
            var_rvalue(varrvalue, prevtoken);
            rvalue = varrvalue;
        }
    }
    else
    {
        error("Expected something");
    }
}

void ASTParser::call_expr(std::shared_ptr<CallExpr> &callexpr, Token prevtoken)
{
    // ID LPAREN(<expr>(COMMA <expr>)*|empty)RPAREN
    // IS ASSUMING ID WAS ALREADY EATEN
    callexpr->fun_name = prevtoken;
    eat(TokenType::LPAREN, "Expected a LPAREN");
    if (!match(TokenType::RPAREN))
    {
        Expr temp;
        expr(temp);
        if (match(TokenType::ASSIGN)){
            callexpr->keyword_values = true;
        }
        callexpr->args.push_back(temp);
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::ASSIGN)){
                callexpr->keyword_values = true;
            }
            if(callexpr->keyword_values){
                eat(TokenType::ASSIGN, "");
                Expr default_val;
                expr(default_val);
                callexpr->keyword_args.push_back(default_val);
            }
            if (match(TokenType::COMMA))
            {
                Expr expression;
                eat(TokenType::COMMA, "Expected a COMMA");
                expr(expression);
                callexpr->args.push_back(expression);
            }else 
            {
                break;
            }
        }
    }
    eat(TokenType::RPAREN, "Expected a RPAREN");
}

void ASTParser::expr(Expr &expression)
{
    // (<rvalue>|NOT <expr>|LPAREN <expr> RPAREN)(<bin_op> <expr>|empty)
    // either rval, not, or lparen
    
    // set the not flag and recall
    while(match(TokenType::NOT))
    {
        if(expression.negated == true){
            expression.negated = false;
        }else{
            expression.negated = true;
        }
        eat(TokenType::NOT, "Expected a NOT");
    }

    // this is the complexexpr
    if (match(TokenType::LPAREN))
    {
        eat(TokenType::LPAREN, "Expected a LPAREN");
        shared_ptr<ComplexTerm> t = make_shared<ComplexTerm>();
        expr(t->expr);
        expression.first = t;
        eat(TokenType::RPAREN, "Expected a RPAREN");
    }
    else //this is the simple expr (just an rvalue)
    {
        shared_ptr<SimpleTerm> simple = make_shared<SimpleTerm>();
        r_value(simple->rvalue);
        expression.first = simple;
        // do somethng with this?
    }
    // if there is a bin_op, that means that there will be a rest
    if (bin_op())
    {
        expression.op = curr_token;
        advance();
        expression.rest = make_shared<Expr>();
        expr(*expression.rest);
    }
    // empty
}

bool ASTParser::base_type()
{
    // INT_TYPE|DOUBLE_TYPE|BOOL_TYPE|CHAR_TYPE|STRING_TYPE
    return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::CHAR_TYPE, TokenType::STRING_TYPE});
}