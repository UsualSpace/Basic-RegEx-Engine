#pragma once

/*
 * Filename: regex.h
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/28/2024
 * Purpose: The purpose of this file is to define the Regex class. Currently
 *          only the 3 fundamental regular expression operations are supported:
 *          concatenation, alternation, and kleene closure.
 */
 
#include "tnfa.h"
#include <string>
#include <unordered_set>

using std::string;
using std::unordered_set;

class Regex {
    public:
        //Ctor and overloaded dtor.
        Regex();
        Regex(const string& a_pattern);
    
        //Will reconstruct the internal nfa representation of a regex
        //based on the input pattern. Returns a reference to the
        //invoking Regex object.
        Regex& operator=(const string& a_pattern);
        
        //Returns the "pattern" data member.
        string GetPattern(void) const;
    
        //Checks if the input string matches the pattern recognized by "nfa".
        //Only looks for exact matches from the beginning of a string to the
        //end. Returns if it matched or not (true/false);
        bool Match(const string& input) const;
        
        //TODO: support more regex operations.
        
    private:
        //Inserts a concatenation symbol '+' into a regular expression where
        //it is implicitly implied. Returns a copy of the input but with 
        //explicit concatenation symbols.
        string MakeConcatenationExplicit(const string& a_pattern) const;
    
        //Converts a regular expression string into its post fix form for
        //simple stack evaluation. Returns said post fix form.
        string RegexToPostFix(const string& a_pattern) const;
        
        //Constructs an NFA representation of a regex, per the rules 
        //defined by thompsons construction algorithm. TODO:
        //Make this a standalone function that returns a TNFA?
        void DoThompsonsConstruction(const string& a_pattern);
        
        //Computes the set of all reachable states from the current state
        //via epsilon transitions.
        void GetEpsilonClosure(State* current, unordered_set<State*>& visited) const;
    
        string pattern; //The regex pattern "nfa" will be built upon.
        
        TNFA nfa; //Will store the thompson construction based NFA 
                  //representation of the provided regex pattern.
};
