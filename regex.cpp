/*
 * Filename: regex.cpp
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/28/2024
 * Purpose: The purpose of this file is to implement the Regex class 
 *          defined in regex.h
 */

#include "regex.h"
#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>

using std::string;
using std::stack;
using std::unordered_map;

//BEGINNING OF REGEX CLASS IMPLEMENTATION

Regex::Regex() {}

Regex::Regex(const string& a_pattern) {
    this->pattern = a_pattern;
    DoThompsonsConstruction(a_pattern);
}

Regex& Regex::operator=(const string& a_pattern) {
    this->pattern = a_pattern;
    this->nfa.Clear();
    DoThompsonsConstruction(a_pattern);
    return *this;
}

string Regex::GetPattern(void) const {
    return this->pattern;
}

bool Regex::Match(const string& input) const {
    bool matched = false;
    MatchHelper(this->nfa.GetStartState(), input, 0, matched);
    return matched;
}

void Regex::MatchHelper(State* current, const string& s, size_t position, bool& matched) const {
    if(matched) return;
    
    for(State* next : current->epsilon_transitions) {
        MatchHelper(next, s, position, matched);
    }

    if(position >= s.size()) {
        matched = matched || current->acceptance;
        return;
    }
    
    auto it = current->symbol_transitions.find(s.at(position));
    if(it != current->symbol_transitions.end()) {
        for(State* next : current->symbol_transitions.at(s.at(position))) {
            MatchHelper(next, s, position + 1, matched);
        }
    } 
}

//TODO: make this less ugly.
string Regex::MakeConcatenationExplicit(const string& a_pattern) {
    std::string result = "";
    for(int i{}; i < a_pattern.size(); ++i) {
        if (i > 0 && 
            !(a_pattern[i - 1] == '|' || a_pattern[i - 1] == '(' || a_pattern[i - 1] == '*' || a_pattern[i - 1] == '+') &&
            !(a_pattern[i] == '|' || a_pattern[i] == '*' || a_pattern[i] == ')' || a_pattern[i] == '+')) {
            result += '+';
        }
        result += a_pattern[i];
    }
    return result;
}

string Regex::RegexToPostFix(const string& a_pattern) {
    //Setup for shunting yard algorithm.
    unordered_map<char, int> precedence = {{'*', 3}, {'+', 2}, {'|', 1}};
    stack<char> operators;
    string output;
    
    //Do shunting yard algorithm.
    for(const char& c : MakeConcatenationExplicit(a_pattern)) {
        if(c == '*' || c == '+' || c == '|') {
            while(!operators.empty() && precedence[c] <= precedence[operators.top()]) {
                output += operators.top();
                operators.pop();
            }
            operators.push(c);
        } else if(c == '(') {
            operators.push(c);
        } else if(c == ')') {
            while(!operators.empty() && operators.top() != '(') {
                output += operators.top();
                operators.pop();
            }
            operators.pop();
        } else {
            output += c;
        }
    }
    while(!operators.empty()) {
        output += operators.top();
        operators.pop();
    }
    return output;
}

void Regex::DoThompsonsConstruction(const string& a_pattern) {
    stack<TNFA> nfa_stack;
    for(const char& c : RegexToPostFix(a_pattern)) {
        switch(c) {
            case '*': {
                    auto x = nfa_stack.top();
                    nfa_stack.pop();
                    nfa_stack.push(x.ApplyKleeneClosure());
                }
                break;
            case '+': {
                    auto rhs = nfa_stack.top();
                    nfa_stack.pop();
                    auto lhs = nfa_stack.top();
                    nfa_stack.pop();
                    nfa_stack.push(lhs + rhs);
                }
                break;
            case '|': {
                    auto rhs = nfa_stack.top();
                    nfa_stack.pop();
                    auto lhs = nfa_stack.top();
                    nfa_stack.pop();
                    nfa_stack.push(lhs | rhs);
                }
                break;
            default:
                nfa_stack.push(TNFA(c));
                break;
        }
    }
    this->nfa = nfa_stack.top();
    return;
}

//END OF REGEX CLASS IMPLEMENTATION
