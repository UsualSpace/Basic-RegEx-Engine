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
#include <unordered_set>

using std::string;
using std::stack;
using std::unordered_map;
using std::unordered_set;

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
    //Will hold the set of states the nfa is currently in at any moment.
    unordered_set<State*> current_states; 
                                        
    //Get all states the nfa will be in simultaneously at the start.
    GetEpsilonClosure(this->nfa.GetStartState(), current_states);
    
    unordered_set<State*> closure;
    
    for(char c : input) {
        for(State* state : current_states)
            if(state->symbol_transitions.count(c) > 0)
                for(State* next_state : state->symbol_transitions.at(c))
                    GetEpsilonClosure(next_state, closure);
                    
        current_states = closure;  
        closure.clear();  
    }
    
    for(State* state : current_states) if(state->acceptance) return true;
        
    return false;
}

//TODO: make this less ugly.
string Regex::MakeConcatenationExplicit(const string& a_pattern) const {
    std::string result = "";
    for(int i{}; i < a_pattern.size(); ++i) {
        if (i > 0 && 
            !(a_pattern[i - 1] == '|' || a_pattern[i - 1] == '(' || a_pattern[i - 1] == '+') &&
            !(a_pattern[i] == '|' || a_pattern[i] == '*' || a_pattern[i] == ')' || a_pattern[i] == '+')) {
            result += '+';
        }
        result += a_pattern[i];
    }
    std::cout << result << std::endl;
    return result;
}

string Regex::RegexToPostFix(const string& a_pattern) const {
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
    std::cout << output << std::endl;
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

void Regex::GetEpsilonClosure(State* current, unordered_set<State*>& visited) const {
    //Mark current state as visited.
    visited.insert(current);
    
    //Visit all other neighboring states of the current state via 
    //epsilon transitions.
    for(State* next_state : current->epsilon_transitions)
        if(visited.count(next_state) == 0)
            GetEpsilonClosure(next_state, visited);
            
    return;
}

//END OF REGEX CLASS IMPLEMENTATION
