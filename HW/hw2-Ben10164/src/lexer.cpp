//----------------------------------------------------------------------
// FILE: lexer.cpp
// DATE: CPSC 326, Spring 2023
// NAME: Ben Puryear
// DESC: Implements the Lexer of MyPL
//----------------------------------------------------------------------

#include "lexer.h"
#include <iostream>

using namespace std;

Lexer::Lexer(istream &input_stream)
    : input{input_stream}, column{0}, line{1}
{
}

char Lexer::read()
{
    ++column;
    return input.get();
}

char Lexer::peek()
{
    return input.peek();
}

void Lexer::error(const string &msg, int line, int column) const
{
    throw MyPLException::LexerError(msg + " at line " + to_string(line) +
                                    ", column " + to_string(column));
}

Token Lexer::next_token()
{
    // Read in the currect char
    char curr_char = read();
    // First we are going to skip over all comments, new lines, and spaces before logging the tokens
    while (curr_char == '#' || curr_char == ' ' || curr_char == '\n')
    {
        // Skipping over comments
        if (curr_char == '#')
        {
            while (curr_char != EOF)
            {
                curr_char = read();
                // for comments, you skip until a new line
                if (curr_char == '\n')
                {
                    break;
                }
            }
        }
        // Skipping over spaces
        if (curr_char == ' ')
        {
            while (curr_char != EOF)
            {
                curr_char = read();
                // if the char is not a space, break (this could be a '\n', '#', or EOF)
                if (curr_char != ' ')
                {
                    break;
                }
            }
            // while still in the space skipping section, if the char is a new line, reset the column and increment line
            if (curr_char == '\n')
            {
                column = 1;
            }
        }
        // Skipping new lines
        if (curr_char == '\n')
        {
            while (curr_char != EOF)
            {
                if (curr_char == '\n')
                {
                    line++;
                    column = 1;
                }
                else
                {
                    // this can only happen after a first new line
                    column = 1; // reset the column to 1
                    break;
                }
                curr_char = read(); // after checking, read once more
            }
        }
    }

    // EOF implementation
    if (curr_char == EOF)
    {
        // fix for the bug for empty files
        return Token(TokenType::EOS, "", line, column);
    }

    // operators (and equal)
    if (curr_char == '+')
    {
        return Token(TokenType::PLUS, "+", line, column);
    }
    else if (curr_char == '-')
    {
        return Token(TokenType::MINUS, "-", line, column);
    }
    else if (curr_char == '*')
    {
        return Token(TokenType::TIMES, "*", line, column);
    }
    else if (curr_char == '/')
    {
        return Token(TokenType::DIVIDE, "/", line, column);
    }
    else if (curr_char == '=')
    {
        if (peek() == '=') // check for '=='
        {
            curr_char = read();
            return Token(TokenType::EQUAL, "==", line, column - 1);
            // NOTE: column = col-1 in the Token because its the start of the two char lexeme
        }
        else
        {
            return Token(TokenType::ASSIGN, "=", line, column);
        }
    }

    // comparators (no equal)
    if (curr_char == '<')
    {
        int start_pos = column;
        if (peek() == '=') // check for '<='
        {
            curr_char = read();
            return Token(TokenType::LESS_EQ, "<=", line, start_pos);
        }
        return Token(TokenType::LESS, "<", line, column);
    }
    else if (curr_char == '>')
    {
        int start_pos = column;
        if (peek() == '=') // check for '>='
        {
            curr_char = read();
            return Token(TokenType::GREATER_EQ, ">=", line, start_pos);
        }
        return Token(TokenType::GREATER, ">", line, column);
    }
    else if (curr_char == '!')
    {
        int start_pos = column;
        if (peek() == '=') // MUST be '!='
        {
            curr_char = read();
            return Token(TokenType::NOT_EQUAL, "!=", line, start_pos);
        }
        else
        {
            error("expecting '!=' found '!" + string(1, peek()) + "'", line, column);
        }
    }

    // punctuation
    if (curr_char == '.')
    {
        return Token(TokenType::DOT, ".", line, column);
    }
    else if (curr_char == ',')
    {
        return Token(TokenType::COMMA, ",", line, column);
    }
    else if (curr_char == '(')
    {
        return Token(TokenType::LPAREN, "(", line, column);
    }
    else if (curr_char == ')')
    {
        return Token(TokenType::RPAREN, ")", line, column);
    }
    else if (curr_char == '[')
    {
        return Token(TokenType::LBRACKET, "[", line, column);
    }
    else if (curr_char == ']')
    {
        return Token(TokenType::RBRACKET, "]", line, column);
    }
    else if (curr_char == ';')
    {
        return Token(TokenType::SEMICOLON, ";", line, column);
    }
    else if (curr_char == '{')
    {
        return Token(TokenType::LBRACE, "{", line, column);
    }
    else if (curr_char == '}')
    {
        return Token(TokenType::RBRACE, "}", line, column);
    }

    // Character detection
    string lexeme = "";
    if (curr_char == '\'')
    {
        int start_pos = column;
        // checking if the char following the ' is alphabetic, or a punctuation (but not \)
        if (isalpha(peek()) || ispunct(peek()) && peek() != '\\')
        {
            curr_char = read(); // read in the next char
            lexeme.push_back(curr_char);

            if (peek() == '\'') // check to make sure if the char ends
            {
                curr_char = read();
                // it MUST end because we already know it is not '\'
                return Token(TokenType::CHAR_VAL, lexeme, line, start_pos);
            }
        }
        else
        {
            curr_char = read();
            lexeme.push_back(curr_char);
            // it is either an error, or there is a \ in the lexeme
            if (peek() == 'n' || peek() == 't' || peek() == '0') // check for escape characters
            {
                // lexeme.push_back(curr_char);
                curr_char = read();
                lexeme.push_back(curr_char);
                if (peek() == '\'')
                {
                    curr_char = read();
                    return Token(TokenType::CHAR_VAL, lexeme, line, start_pos);
                }
                else
                {
                    error("expecting ' found " + string(1, curr_char), line, column);
                }
            }
        }
        // at this point, it has started out as a character, but had an error somewhere in it
        if (curr_char == EOF) // eof error?
        {
            error("found end-of-file in character", line, column);
        }
        else if (curr_char == '\n') // new line error?
        {
            error("found end-of-line in character", line, column);
        }
        else if (curr_char == '\'') // empty error?
        {
            error("empty character", line, column);
        }
        else // no closing '
        {
            error("expecting ' found " + string(1, peek()), line, column + 1);
        }
    }

    // String detection
    if (curr_char == '\"')
    {
        int start_pos = column;
        curr_char = read();
        while (peek() != EOF && peek() != '\n') // reading while its not a new line or EOF
        {
            // repeats until the closing "
            if (curr_char == '\"')
            {
                break;
            }
            lexeme.push_back(curr_char);
            curr_char = read();
        }
        if (curr_char == '\"') // if it broke/ended due to a "
        {
            return Token(TokenType::STRING_VAL, lexeme, line, start_pos);
        }
        else
        {
            if (peek() == EOF) // EOF error
            {
                error("found end-of-file in string", line, column);
            }
            else if (peek() == '\n') // new line error
            {
                error("found end-of-line in string", line, column + 1);
            }
            else // other error
            {
                error("expecting \" found " + string(1, peek()), line, column);
            }
        }
    }

    // Int AND DOUBLE detection
    if (isdigit(curr_char))
    {
        if (curr_char == '0' && peek() != '.' && peek() != ' ' && peek() != ';') // leading zero detection
        {
            error("leading zero in number", line, column);
        }
        bool period_present = false; // bool to be used during double detection
        int start_pos = column;
        lexeme.push_back(curr_char);
        while (peek() != EOF)
        {
            if (peek() == '.') // check for period (double)
            {
                if (period_present) // there is already a period
                {
                    return Token(TokenType::DOUBLE_VAL, lexeme, line, start_pos); // returns as is
                }
                period_present = true; // sets period_present to true
            }
            if (curr_char == ' ' || curr_char == '\n' || isalpha(peek())) // check for alphabetic char
            {
                if (period_present)
                {
                    return Token(TokenType::DOUBLE_VAL, lexeme, line, start_pos); // return double
                }
                else
                {
                    return Token(TokenType::INT_VAL, lexeme, line, start_pos); // return int
                }
            }
            curr_char = read();
            if (isdigit(curr_char) || curr_char == '.')
            {
                lexeme.push_back(curr_char);
            }
            if (!(isdigit(peek())) && curr_char == '.')
            {
                error("missing digit in \'" + lexeme + "\'", line, column + 1);
            }
            if (isalpha(peek()))
            {
                if (period_present)
                {
                    return Token(TokenType::DOUBLE_VAL, lexeme, line, start_pos); // return double
                }
                else
                {
                    return Token(TokenType::INT_VAL, lexeme, line, start_pos); // return int
                }
            }
        }
        if (curr_char == ' ' || curr_char == '\n' || peek() == EOF)
        {
            if (period_present)
            {
                return Token(TokenType::DOUBLE_VAL, lexeme, line, start_pos); // return double
            }
            else
            {
                return Token(TokenType::INT_VAL, lexeme, line, start_pos); // return int
            }
        }
    }

    // keywords & ID detection
    int start_pos = column;
    lexeme.push_back(curr_char);
    // go until curr_char is eof or ' ' or \n or any other token type
    while (peek() != EOF || peek() != ' ' || peek() != '\n')
    {
        curr_char = read();
        if (isalnum(curr_char) || curr_char == '_')
        {
            lexeme.push_back(curr_char);
        }
        if (curr_char == EOF || curr_char == ' ' || peek() == '\n')
        {
            break;
        }
        // check if it is a symbol
        if (peek() == '(' || peek() == ')' || peek() == '=' || peek() == '{' || peek() == '}' || peek() == '[' || peek() == ']' || peek() == '+' || peek() == '-' || peek() == '*' || peek() == '/' || peek() == '\'' || peek() == '"' || peek() == '+' || peek() == '.' || peek() == ',' || peek() == ';')
        {
            break;
        }
    }
    // special
    if (lexeme.compare("null") == 0)
    {
        return Token(TokenType::NULL_VAL, lexeme, line, start_pos);
    }
    else if (lexeme.compare("true") == 0 || lexeme.compare("false") == 0)
    {
        return Token(TokenType::BOOL_VAL, lexeme, line, start_pos);
    }

    // primitive
    if (lexeme.compare("int") == 0)
    {
        return Token(TokenType::INT_TYPE, lexeme, line, start_pos);
    }
    else if (lexeme.compare("double") == 0)
    {
        return Token(TokenType::DOUBLE_TYPE, lexeme, line, start_pos);
    }
    else if (lexeme.compare("char") == 0)
    {
        return Token(TokenType::CHAR_TYPE, lexeme, line, start_pos);
    }
    else if (lexeme.compare("string") == 0)
    {
        return Token(TokenType::STRING_TYPE, lexeme, line, start_pos);
    }
    else if (lexeme.compare("bool") == 0)
    {
        return Token(TokenType::BOOL_TYPE, lexeme, line, start_pos);
    }
    else if (lexeme.compare("void") == 0)
    {
        return Token(TokenType::VOID_TYPE, lexeme, line, start_pos);
    }

    // logical
    if (lexeme.compare("and") == 0)
    {
        return Token(TokenType::AND, lexeme, line, start_pos);
    }
    else if (lexeme.compare("or") == 0)
    {
        return Token(TokenType::OR, lexeme, line, start_pos);
    }
    else if (lexeme.compare("not") == 0)
    {
        return Token(TokenType::NOT, lexeme, line, start_pos);
    }

    // conditional
    if (lexeme.compare("if") == 0)
    {
        return Token(TokenType::IF, lexeme, line, start_pos);
    }
    else if (lexeme.compare("elseif") == 0)
    {
        return Token(TokenType::ELSEIF, lexeme, line, start_pos);
    }
    else if (lexeme.compare("else") == 0)
    {
        return Token(TokenType::ELSE, lexeme, line, start_pos);
    }

    // looping
    if (lexeme.compare("for") == 0)
    {
        return Token(TokenType::FOR, lexeme, line, start_pos);
    }
    else if (lexeme.compare("while") == 0)
    {
        return Token(TokenType::WHILE, lexeme, line, start_pos);
    }

    // function
    if (lexeme.compare("return") == 0)
    {
        return Token(TokenType::RETURN, lexeme, line, start_pos);
    }

    // complex
    if (lexeme.compare("struct") == 0)
    {
        return Token(TokenType::STRUCT, lexeme, line, start_pos);
    }
    else if (lexeme.compare("array") == 0)
    {
        return Token(TokenType::ARRAY, lexeme, line, start_pos);
    }

    // additional
    if (lexeme.compare("new") == 0)
    {
        return Token(TokenType::NEW, lexeme, line, start_pos);
    }

    // ID and invalid characters
    if (lexeme.find('?') != std::string::npos)
    {
        error("unexpected character '?'", line, column + lexeme.find('?') - 1);
    }
    return Token(TokenType::ID, lexeme, line, start_pos);
}
