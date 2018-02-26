#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "functions.h"

bool isanop(char c);

/**
 * Checks to see if the bracket symbols are balanced.
 * @param input a std::string to examine
 * @return whether or not brackets are balanced
 **/
bool balancedSymbols(std::string input) {
    std::stack<char, std::deque<char> > stk;

    for (int i = 0; input[i] != '\0'; i++) {
        switch (input[i]) {
            case '(':
            case '[':
            case '{':
                stk.push(input[i]);
                break;

            default:
                /* If the immediately previous */
                /* It's necessary to put this inside an if block to
                 * prevent a segfault caused by calling top() on an
                 * empty stack. */
                if (!stk.empty()) {
                    switch (input[i]) {
                        case ')':
                            if (stk.top() == '(')
                                stk.pop();
                            else
                                return false;
                            break;

                        case ']':
                            if (stk.top() == '[')
                                stk.pop();
                            else
                                return false;
                            break;

                        case '}':
                            if (stk.top() == '[')
                                stk.pop();
                            else
                                return false;
                            break;

                        default:
                            break;
                    }
                } else if (input[i] == ')' ||
                           input[i] == ']' ||
                           input[i] == '}') {
                    return false;
                }
        }
    }


    return stk.empty();
}


/**
 * Checks to see if the given charcter is an arithmetic operator
 * @param c a character to check
 * @return whether or not c is an arithmetic operator
 **/
bool isanop(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

/**
 * Checks to see if a string is of proper infix format
 * @param s the string to check
 * @return whether or not that string is of proper infix format
 **/
bool strok(std::string s) {
    int len = s.length();
    if (!balancedSymbols(s) || isanop(s[len-1]) ||
            isanop(s[0])) {
        return false;
    }

    bool justhadop = false;
    for (int i = 0; s[i]; i++) {
        if (isanop(s[i])) {
            if (justhadop) {
                return false;
            }
            justhadop = true;
        } else {
            justhadop = false;
        }
    }

    return true;
}

/**
 * Lex the given infix string into a vector of strings.
 * @param input the string to lex
 * @return a vector of strings containing the lexed atoms
 **/
std::vector<std::string> lex(std::string input) {
    std::vector<std::string> vec;
    std::string read;
    int parencount = 0;

    int len = input.length();

    /* We'll be needing this later, to preserve iteration counters
       past the end of for loops. */
    int j;

    /* If the string is all contained within one pair of parentheses,
       strip them. */
    if (input[0] == '(' && input[len-1] == ')') {
        parencount++;
        for (j = 1; input[j] != '\0'; j++) {
            if (input[j] == '(') {
                parencount++;
            } else if (input[j] == ')') {
                parencount--;
            }

            if (parencount == 0)
                break;
        }
    }

    if (j == len-1) {
        input = input.substr(1, len-2);
    }

    if (!strok(input)) {
        std::cerr << "Error: malformed infix string \"" << input << 
            "\"" << std::endl;
        return std::vector<std::string>();
    }

    for (int i = 0; input[i] != '\0'; i++) {
        read = "";
        parencount = 0;

        switch(input[i]) {
            /* If we find a number, lex it and push it to the vector. */
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                for (j = 0;
                     input[i+j] >= '0' && input[i+j] <= '9';
                     j++) {
                    read += input[i+j];
                }
                vec.push_back(read);
                i += j - 1;
                break;

            /* If we come across an opening parenthesis, recurse on it,
               and push the return value to the vector. */
            case '(':
                parencount++;
                /* We know this loop will halt because we already
                   called balancedSymbols() */
                for (j = 0; parencount > 0; j++) {
                    if (input[i+j] == ')')
                        parencount--;
                    else if (input[i+j] == '(' && j != 0)
                        parencount++;

                    read += input[i+j];
                }

                vec.push_back(iToP(read));
                i += j - 1;
                break;
 
            case ')':
                break;

            case '*':
            case '/':
            case '+':
            case '-':
                vec.push_back(input.substr(i, 1));
                break;

            default:
                std::cerr << "Error: input contains unrecognized character "
                    << "\'" << input[i] << "\'"<< "; ignoring" <<
                    std::endl;
                break;
        }
    }

    return vec;
}


/**
 * Returns the input arithmetic expression in reverse Polish notation
 * @param input The string to convert
 * @return the converted string
 **/
std::string iToP(std::string input) {
    /* Declaring and defining a string to store substrings of input. */
    std::string read = "";

    int i;
    bool isnum = true;

    /* First, strip all spaces from input. */
    /* Meanwhile, check to see if the whole of input is a valid number!
       If so, we can just return it. */
    for (i = 0; input[i] != '\0'; i++) {
        if (isnum && (input[i] < '0' || input[i] > '9'))
            isnum = false;

        if (input[i] == ' ') {
            input.erase(input.begin()+i);
            i--;
        }
    }

    if (isnum)
        return input;

    /* Lex input into a vector */
    std::vector<std::string> vec = lex(input);

    std::string retstr = "";

    int vecsize = vec.size();

    /* Begin iteration through the lex vector. */
    for (int i = 0; i < vecsize; i++) {
        /* Check to see if the first character in the string is a '*'
           or '/' operator, and if so, parse like so:
           A B C -> A C B 
           and assign the result to the immediately prior string in the
           vector. Then erase the operator and the second operand. */

        if (vec[i][0] == '*' || vec[i][0] == '/') {
            vec[i-1] = vec[i-1] + " " + vec[i+1] + " " + vec[i];
            vec.erase(vec.begin()+i, vec.begin()+(i+2));
            vecsize -= 2;
            i--;
        }
    }

    /* The same, but with addition and subtraction. */
    for (int i = 0; i < vecsize; i++) {
        if (vec[i][0] == '+' || vec[i][0] == '-') {
            vec[i-1] = vec[i-1] + " " + vec[i+1] + " " + vec[i];
            vec.erase(vec.begin()+i, vec.begin()+(i+2));
            vecsize -= 2;
            i--;
        }
    }

    return *(vec.begin());

}


int main(int argc, const char **argv) {
    std::string foo = argv[1];

    std::cout << "\"" << foo << "\" is " <<
        (balancedSymbols(foo) ? "" : "not ") << "balanced" << std::endl;

    std::cout << "\"" << foo << "\" in reverse-Polish notation is \"" <<
        iToP(foo) << "\"" << std::endl;

    return 0;
}
