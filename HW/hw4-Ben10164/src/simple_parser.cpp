//----------------------------------------------------------------------
// FILE: simple_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Ben Puryear
// DESC:
//----------------------------------------------------------------------

#include "simple_parser.h"

SimpleParser::SimpleParser(const Lexer &a_lexer)
    : lexer{a_lexer}
{
}

void SimpleParser::advance()
{
    curr_token = lexer.next_token();
}

void SimpleParser::eat(TokenType t, const std::string &msg)
{
    if (!match(t))
        error(msg);
    advance();
}

bool SimpleParser::match(TokenType t) 
{
    return curr_token.type() == t;
}

bool SimpleParser::match(std::initializer_list<TokenType> types)
{
    for (auto t : types)
        if (match(t))
            return true;
    return false;
}

void SimpleParser::error(const std::string &msg)
{
    std::string s = msg + " found '" + curr_token.lexeme() + "' ";
    s += "at line " + std::to_string(curr_token.line()) + ", ";
    s += "column " + std::to_string(curr_token.column());
    throw MyPLException::ParserError(s);
}

bool SimpleParser::bin_op()
{
    // PLUS|MINUS|TIMES|DIVIDE|AND|OR|EQUAL|LESS|GREATER|LESS_EQ|GREATER_EQ|NOT_EQUAL
    return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
                  TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
                  TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
                  TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}

void SimpleParser::parse()
{
    advance();
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::STRUCT))
            struct_def();
        else
            fun_def();
    }
    eat(TokenType::EOS, "expecting end-of-file");
}

void SimpleParser::struct_def()
{
    // STRUCT ID LBRACE <fields> RBRACE
    eat(TokenType::STRUCT, "Expected a struct");
    eat(TokenType::ID, "Expected an ID");
    eat(TokenType::LBRACE, "Expected a LBrace");
    fields(); // calls the fields function
    eat(TokenType::RBRACE, "Expected a RBrace");
}

void SimpleParser::fun_def()
{
    // (<data_type> | VOID_TYPE) ID LPAREN <params> RPAREN LBRACE (<stmt>)* RBRACE
    if (data_type() || match(TokenType::VOID_TYPE)) // if it is a datatype, or void
    {
        // since data_type() advances, if its void we need to advance manually
        if (match(TokenType::VOID_TYPE)) 
        {
            advance();
        }
        eat(TokenType::ID, "Expected an ID");
        eat(TokenType::LPAREN, "Expected a LParen");
        params(); // calls the params
        eat(TokenType::RPAREN, "Expected a RParen");
        eat(TokenType::LBRACE, "Expected a LBrace");
        // handle the (<stmt>)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt();
        }
        eat(TokenType::RBRACE, "Expected a RBrace"); // redundant, could also use advance
    }
    else
    {
        error("Expected a <data_type> or VOID_TYPE");
    }
}

void SimpleParser::fields()
{
    // <data_type> ID (COMMA <data_type> ID)* | empty
    // make sure it is a <data_type>
    if (data_type())
    {
        eat(TokenType::ID, "Expected an ID");
        // handle the (COMMA <data_type> ID)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::COMMA))
            {
                eat(TokenType::COMMA, "Expected a COMMA"); // redundant, could also use advance
                if (data_type())
                {
                    eat(TokenType::ID, "Expected an ID");
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

bool SimpleParser::data_type()
{
    // <base_type>|ID|ARRAY(<base_type>|ID)
    if (base_type())
    {
        advance();
        return true;
    }
    else if (match(TokenType::ID))
    {
        advance();
        return true;
    }
    else if (match(TokenType::ARRAY))
    {
        advance();
        // array is a multi-token data_type, so we need to check the next
        if (base_type())
        {
            advance();
            return true;
        }
        else if (match(TokenType::ID))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool SimpleParser::data_type_minus_ID()
{
    // <base_type>|ID|ARRAY(<base_type>|ID)
    if (base_type())
    {
        advance();
        return true;
    }
    else if (match(TokenType::ARRAY))
    {
        advance();
        if (base_type())
        {
            advance();
            return true;
        }
        else if (match(TokenType::ID))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool SimpleParser::base_type()
{
    // INT_TYPE|DOUBLE_TYPE|BOOL_TYPE|CHAR_TYPE|STRING_TYPE
    return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::CHAR_TYPE, TokenType::STRING_TYPE});
}

void SimpleParser::params()
{
    // <data_type> ID(COMMA <data_type> ID)*|empty
    fields(); // the same function
}

void SimpleParser::stmt()
{
    // <vdecl_stmt>|<assign_stmt>|<if_stmt>|<while_stmt>|<for_stmt>|<call_expr>|<ret_stmt>
    if (data_type_minus_ID()) // could be ID
    {
        vdecl_stmt_no_pre_ID();
    }
    else if (match(TokenType::IF))
    {
        if_stmt();
    }
    else if (match(TokenType::WHILE))
    {
        while_stmt();
    }
    else if (match(TokenType::FOR))
    {
        for_stmt();
    }
    else if (match(TokenType::ID)) // both <assign_stmt> and <call_expr> ALSO DATA_TYPE() could be an ID
    {
        eat(TokenType::ID, "Expected an ID");
        // need to check for vdecl_stmt
        if (match(TokenType::ID) || match(TokenType::ASSIGN))
        {
            vdecl_stmt();
        }
        else if (match(TokenType::LPAREN))
        {
            call_expr();
        }
        else
        {
            // else because match() could be empty due to lvalue including ()*
            assign_stmt();
        }
    }
    else if (match(TokenType::RETURN))
    {
        ret_stmt();
    }
    else
    {
        error("Expected <vdecl_stmt>|<assign_stmt>|<if_stmt>|<while_stmt>|<for_stmt>|<call_expr>|<ret_stmt>");
    }
}

void SimpleParser::lvalue()
{
    // ID(DOT ID|LBRACKET <expr> RBRACKET)*
    // assuming ID was already eaten
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::DOT))
        {
            advance();
            eat(TokenType::ID, "Expected an ID");
        }
        else if (match(TokenType::LBRACKET))
        {
            advance();
            expr();
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
        else
        {
            break;
        }
    }
}

void SimpleParser::assign_stmt()
{
    // <lvalue> ASSIGN <expr>
    // <lvalue> assumes the ID at the start is eaten
    if (match(TokenType::ID))
    {
        eat(TokenType::ID, "Expected an ID");
    }
    lvalue();
    eat(TokenType::ASSIGN, "Expected a ASSIGN");
    expr();
}

void SimpleParser::call_expr()
{
    // ID LPAREN(<expr>(COMMA <expr>)*|empty)RPAREN
    // IS ASSUMING ID WAS ALREADY EATEN
    eat(TokenType::LPAREN, "Expected a LPAREN");
    if (!match(TokenType::RPAREN))
    {
        expr();
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::COMMA))
            {
                eat(TokenType::COMMA, "Expected a COMMA");
                expr();
            }
            else
            {
                break;
            }
        }
    }
    eat(TokenType::RPAREN, "Expected a RPAREN");
}
void SimpleParser::vdecl_stmt()
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
        eat(TokenType::ID, "Expected an ID");
        eat(TokenType::ASSIGN, "Expected a ASSIGN");
    }
    expr();
}

void SimpleParser::vdecl_stmt_no_pre_ID()
{
    // <data_type> ID ASSIGN <expr>
    // this function is called when there is a <data_type> that is not an ID
    // e.g. int i = 0
    // assumes the <data_type> was dealt with
    eat(TokenType::ID, "Expected an ID");
    eat(TokenType::ASSIGN, "Expected a ASSIGN");

    expr();
}
void SimpleParser::if_stmt()
{
    // IF LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE <if_stmt_t>
    eat(TokenType::IF, "Expected a IF");
    eat(TokenType::LPAREN, "Expected a LPAREN");
    expr();
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt();
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
    if_stmt_t();
}

void SimpleParser::if_stmt_t()
{
    // ELSEIF LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE <if_stmt_t>|ELSE LBRACE(<stmt>)*RBRACE|empty
    if (match(TokenType::ELSEIF))
    {
        eat(TokenType::ELSEIF, "Expected a ELSEIF");
        eat(TokenType::LPAREN, "Expected a LPAREN");
        expr();
        eat(TokenType::RPAREN, "Expected a RPAREN");
        eat(TokenType::LBRACE, "Expected a LBRACE");
        // handle the (<stmt>)*
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt();
        }
        eat(TokenType::RBRACE, "Expected a RBRACE");
        if_stmt_t();
    }
    else if (match(TokenType::ELSE))
    {
        eat(TokenType::ELSE, "Expected a ELSE");
        eat(TokenType::LBRACE, "Expected a LBRACE");
        while (!match(TokenType::EOS))
        {
            if (match(TokenType::RBRACE))
            {
                break;
            }
            stmt();
        }
        eat(TokenType::RBRACE, "Expected a RBRACE");
    }
    // empty case; do nothing
}

void SimpleParser::while_stmt()
{
    // WHILE LPAREN <expr> RPAREN LBRACE(<stmt>)*RBRACE
    eat(TokenType::WHILE, "Expected a WHILE");
    eat(TokenType::LPAREN, "Expected a LPAREN");
    expr();
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt();
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
}

void SimpleParser::for_stmt()
{
    // FOR LPAREN <vdecl_stmt> SEMICOLON <expr> SEMICOLON<assign_stmt> <RPAREN> LBRACE(<stmt>)*RBRACE
    eat(TokenType::FOR, "Expected a FOR");
    eat(TokenType::LPAREN, "Expected a LPAREN");
    if (data_type())
    {
        vdecl_stmt();
    }
    else
    {
        error("Expected a <data_type>");
    }
    eat(TokenType::SEMICOLON, "Expected a SEMICOLON");
    expr();
    eat(TokenType::SEMICOLON, "Expected a SEMICOLON");
    assign_stmt();
    eat(TokenType::RPAREN, "Expected a RPAREN");
    eat(TokenType::LBRACE, "Expected a LBRACE");
    // handle the (<stmt>)*
    while (!match(TokenType::EOS))
    {
        if (match(TokenType::RBRACE))
        {
            break;
        }
        stmt();
    }
    eat(TokenType::RBRACE, "Expected a RBRACE");
}

void SimpleParser::ret_stmt()
{
    // RETURN <expr>
    eat(TokenType::RETURN, "Expected a RETURN");
    expr();
}

void SimpleParser::expr()
{
    // (<rvalue>|NOT <expr>|LPAREN <expr> RPAREN)(<bin_op> <expr>|empty)
    // either rval, not, or lparen
    if (match(TokenType::NOT))
    {
        eat(TokenType::NOT, "Expected a NOT");
        expr();
    }
    else if (match(TokenType::LPAREN))
    {
        eat(TokenType::LPAREN, "Expected a LPAREN");
        expr();
        eat(TokenType::RPAREN, "Expected a RPAREN");
    }
    else
    {
        // must be rvalue();
        rvalue();
    }
    if (bin_op())
    {
        advance();
        expr();
    }
    // empty
}

void SimpleParser::rvalue()
{
    // <base_rvalue>|NULL_VAL|<new_rvalue>|<var_rvalue>|<call_expr>
    if (base_rvalue())
    {
        advance();
    }
    else if (match(TokenType::NULL_VAL))
    {
        eat(TokenType::NULL_VAL, "Expected a NULL_VAL");
    }
    else if (match(TokenType::NEW))
    {
        // new_rvalue
        new_rvalue();
    }
    else if (match(TokenType::ID))
    {
        // var_rvalue AND call_expr
        eat(TokenType::ID, "Expected an ID");
        if (match(TokenType::LPAREN))
        {
            call_expr();
        }
        else
        {
            var_rvalue();
        }
    }
    else
    {
        error("Expected something");
    }
}

bool SimpleParser::base_rvalue()
{
    // INT_VAL|DOUBLE_VAL|BOOL_VAL|CHAR_VAL|STRING_VAL
    return match({TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL});
}

void SimpleParser::new_rvalue()
{
    // NEW ID(LBRACKET <expr> RBRACKET|empty)|NEW <base_type> LBRACKET <expr> RBRACKET
    // both start off with NEW
    eat(TokenType::NEW, "Expected a NEW");

    if (match(TokenType::ID))
    {
        // ID(LBRACKET <expr> RBRACKET|empty)
        eat(TokenType::ID, "Expected an ID");
        if (match(TokenType::LBRACKET))
        {
            eat(TokenType::LBRACKET, "Expected a LBRACKET");
            expr();
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
    }
    else if (base_type())
    {
        // <base_type> LBRACKET <expr> RBRACKET
        advance();
        eat(TokenType::LBRACKET, "Expected a LBRACKET");
        expr();
        eat(TokenType::RBRACKET, "Expected a RBRACKET");
    }
}

void SimpleParser::var_rvalue()
{
    // ID(DOT ID|LBRACKET <expr> RBRACKET)*
    // assumes the ID was already eaten
    while (!match(TokenType::EOS))
    {
        if (!match({TokenType::DOT, TokenType::LBRACKET}))
        {
            break;
        }
        if (match(TokenType::DOT))
        {
            eat(TokenType::DOT, "Expected a DOT");
            eat(TokenType::ID, "Expected an ID");
        }
        else if (match(TokenType::LBRACKET))
        {
            eat(TokenType::LBRACKET, "Expected a LBRACKET");
            expr();
            eat(TokenType::RBRACKET, "Expected a RBRACKET");
        }
    }
}