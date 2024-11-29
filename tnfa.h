#pragma once

/*
 * Filename: nfa.h 
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/25/2025
 * Purpose: The purpose of this file is to define the TNFA class, meant
 *          to represent specifically a thompson constructed non
 *          deterministic finite state automaton. I wanted to practice
 *          writing custom data structures and proper copying and deleting,
 *          so I opted to not use smart pointers.
 */
 
#include "state.h"
#include <unordered_map>
#include <unordered_set>

using std::unordered_map;
using std::unordered_set;
 
//TODO: Maybe explicitly store the entire state graph rather than indirectly
//      through state object transition lists?

class TNFA {
    public:
        //Default ctor, overloaded ctors, dtor, copy ctor and overloaded 
        //assignment otor.
        TNFA();
        TNFA(char c); //Recognize a literal symbol.
        TNFA(State* the_start_state, State* the_accept_state);
        TNFA(const TNFA& a_tnfa);
        ~TNFA();
        TNFA& operator=(const TNFA& a_tnfa);
        
        //Setters. TODO: Might remove.
        void SetStartState(State* a_start_state); 
        void SetAcceptState(State* an_accept_state);
        
        //Getters.
        State* GetStartState(void) const;
        State* GetAcceptState(void) const;
        
        //Overloaded addition and compound assignment for TNFA
        //concatenation with a literal character.
        TNFA& operator+=(char c);
        TNFA operator+(char c) const;
        
        //Overloaded addition and compound assignment for TNFA
        //concatenation.
        TNFA& operator+=(const TNFA& a_tnfa);
        TNFA operator+(const TNFA& a_tnfa) const;
        
        //Overloaded bitwise OR and compound assignment for TNFA
        //alternation.
        TNFA& operator|=(const TNFA& a_tnfa);
        TNFA operator|(const TNFA& a_tnfa) const;
        
        //TODO: support alternation against literal characters as well.
        
        //Apply a kleene closure to a TNFA.
        TNFA& ApplyKleeneClosure(void);
        friend TNFA KleeneClosure(const TNFA& a_tnfa);
        
        void Clear(void);
        
    private:
        //Deep copy from another TNFA. "Copy" takes in a TNFA object to copy,
        //"CopyHelper" takes in a source state to copy from and a set of
        //"visited" states, and returns the new copied State*. 
        void Copy(const TNFA& a_tnfa);
        State* CopyHelper(State* source_state, State*& accept_state, unordered_map<State*, State*>& visited_state_map) const;
        
        //Recursively delete all states.
        void ClearHelper(State* current_state, unordered_set<State*>& visited);
    
        State* start_state; //The starting state of this TNFA.
        State* accept_state; //The accepting state of this TNFA.
};
