#pragma once

/*
 * Filename: regex.h
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/28/2024
 * Purpose: The purpose of this file is to define the Regex class. Currently
 *          only the 3 fundamental regular expression operations are supported:
 *          concatenation, alternation, and kleene closure.
 */
 
//TODO: Merge TNFA and Regex class as they are only really used together?
//      Or inherit Regex from TNFA to not have to basically write wrappers for
//      TNFA specific operations.
 
#include "tnfa.h"
#include <string>

using std::string;

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
        //Recursively traverses the data member "nfa" to see if the input
        //string matches the pattern "nfa" was meant to recognize.
        void MatchHelper(State* current, const string& s, size_t position, bool& matched) const;
    
        //Inserts a concatenation symbol '+' into a regular expression where
        //it is implicitly implied. Returns a copy of the input but with 
        //explicit concatenation symbols.
        string MakeConcatenationExplicit(const string& a_pattern);
    
        //Converts a regular expression string into its post fix form for
        //simple stack evaluation. Returns said post fix form.
        string RegexToPostFix(const string& a_pattern);
        
        //Constructs an NFA representation of a regex, per the rules 
        //defined by thompsons construction algorithm. TODO:
        //Make this a standalone function that returns a TNFA?
        void DoThompsonsConstruction(const string& a_pattern);
    
        string pattern; //The regex pattern "nfa" will be built upon.
        
        TNFA nfa; //Will store the thompson construction based NFA 
                  //representation of the provided regex pattern.
};
