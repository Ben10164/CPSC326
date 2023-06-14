//----------------------------------------------------------------------
// FILE: mypl.cpp
// DATE: Spring 2023
// AUTH: Ben Puryear
// DESC: Implements the usage of MyPL stated in HW1
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>

using namespace std;

void help();
void lex(string path);
void parse(string path);
void print(string path);
void check(string path);
void ir(string path);
void normal(string path);

int main(int argc, char *argv[])
{
    // read in the char* c-strings to be std:string's in a new array
    string args[argc];
    for (int i = 0; i < argc; ++i)
        args[i] = (string)argv[i];

    string command = ""; // command will be blank by default in case there is no command (Normal mode)
    string path = "";    // path will also be blank by default in case there is no path (CLI mode)

    switch (argc)
    {
    case 3:
        path = args[2]; // if there are 2 extra arguments, it is `./mypl [command] [path]`
        // we will store the path from args[2]
    case 2:
        command = args[1]; // if there is only 1 extra argument, it is `./mypl [command]` (for CLI mode) or `./mypl [path]` (for default path mode)
        // this will also be reached if there are 2 extra arguments, setting the value of command to args[1]
        break;
    case 1:
        command = ""; // if there are no extra arguments, it is `./mypl` and will run in default CLI mode
        break;
    default:
        command = "--help"; // if there are more than three arguments, the command will be set as if it is "--run"
    }

    if (command.compare("") == 0) // case 1
        // `./mypl` will run normal mode with path = "" (no path will run interactive mode)
        normal(path);
    else if (command.compare("--help") == 0) // could be case 2, 3, or default
        // `./mypl --help`
        help();
    else if (command.compare("--lex") == 0) // could be case 2 or 3
        // `./mypl --lex examples/example.txt` (or without the path) (no path will run interactive mode)
        lex(path);
    else if (command.compare("--parse") == 0) // could be case 2 or 3
        // `./mypl --parse examples/example.txt` (or without the path) (no path will run interactive mode)
        parse(path);
    else if (command.compare("--print") == 0) // could be case 2 or 3
        // `./mypl --print examples/example.txt` (or without the path) (no path will run interactive mode)
        print(path);
    else if (command.compare("--check") == 0) // could be case 2 or 3
        // `./mypl --check examples/example.txt` (or without the path) (no path will run interactive mode)
        check(path);
    else if (command.compare("--ir") == 0) // could be case 2 or 3
        // `./mypl --ir examples/example.txt` (or without the path) (no path will run interactive mode)
        ir(path);
    else
        // `./mypl examples/example.txt` in which normal mode will be run on the path
        normal(command); // although in this case, 'command' will be assumed to be the path
}

void help()
{
    cout << "Usage: ./mypl [option] [script-file]\n";
    cout << "Options:\n";
    cout << "\t--help\tprints this message\n";
    cout << "\t--lex\tdisplays token information\n";
    cout << "\t--parse\tchecks for syntax errors\n";
    cout << "\t--print\tpretty prints program\n";
    cout << "\t--check\tstatically checks program\n";
    cout << "\t--ir\tprint intermediate (code) representation\n";
}

istream *getInputFileContents(string path)
{
    istream *input = new ifstream(path);
    return input;
}

void lex(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        input = getInputFileContents(path);
        // check if the file was able to be opened
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[Lex Mode]" << endl;

    string lexResult = "";
    int letters = 0;
    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;
            delete input;
        }
        char temp;
        input->get(temp);

        // while there are less than 1 letter, lexResult will be adding the char to its value
        if (letters < 1)
            lexResult.push_back(temp);
        letters += 1;
    }
    cout << lexResult << endl;
}

void parse(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        input = getInputFileContents(path);
        // check if the file was able to be opened
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[Parse Mode]" << endl;

    string parseResult = "";
    int letters = 0;
    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;
            delete input;
        }
        char temp;
        input->get(temp);

        // while there are less than 2 letters, parseResult will be adding the char to its value
        if (letters < 2)
            parseResult.push_back(temp);
        letters += 1;
    }
    cout << parseResult << endl;
}

void print(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        input = getInputFileContents(path);
        // check if the file was able to be opened
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[Print Mode]" << endl;

    string printResult = "";
    bool beforeSpace = true;
    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;
            delete input;
        }
        char temp;
        input->get(temp);

        // if there is a space, the value of beforeSpace is set to false
        if (temp == ' ')
            beforeSpace = false;

        // if this character is before the first space, it will be added to printResult
        if (beforeSpace)
            printResult.push_back(temp);
    }
    cout << printResult << endl;
}

void check(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        // check if the file was able to be opened
        input = getInputFileContents(path);
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[Check Mode]" << endl;

    string checkResult = "";
    int totalLines = 0;
    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;
            delete input;
        }
        char temp;
        input->get(temp);

        // if the total lines are less than 1, the char is added to checkResult
        if (totalLines < 1)
            checkResult.push_back(temp);

        // if the char is a new line, totalLines is incremented by 1
        if (temp == '\n')
            totalLines += 1;
    }
    cout << checkResult;
}

void ir(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        input = getInputFileContents(path);
        // check if the file was able to be opened
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[IR Mode]" << endl;

    string irResult = "";
    int totalLines = 0;
    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;
            delete input;
        }
        char temp;
        input->get(temp);

        // if the total lines are less than 1, the char is added to irResult
        if (totalLines < 2)
            irResult.push_back(temp);

        // if the char is a new line, totalLines is incremented by 1
        if (temp == '\n')
            totalLines += 1;
    }
    cout << irResult;
}

void normal(string path)
{
    istream *input;
    input = &cin;

    // check if the path is not empty (from file mode)
    if (path.compare(""))
    {
        input = getInputFileContents(path);
        // check if the file was able to be opened
        if (input->fail())
        {
            cout << "ERROR: Unable to open file \'" + (string)path + "\'" << endl;
            return;
        }
    }

    cout << "[Normal Mode]" << endl;

    string result = "";

    while (true)
    {
        // if the "file" is done (or ctrl-d), break this loop
        if (input->eof())
        {
            break;

            delete input;
        }
        char temp;
        input->get(temp);

        // since this is normal mode, the char will always be added to the result
        result.push_back(temp);
    }
    cout << result;
}