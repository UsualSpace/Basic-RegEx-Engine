/*
 * Filename: tnfa.cpp 
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/25/2025
 * Purpose: The purpose of this file is to implement the TNFA class declared
 *          in "tnfa.h".
 */
 
#include "tnfa.h"
#include <unordered_map>
#include <unordered_set>

using std::unordered_map;
using std::unordered_set;

//TODO: Review design, extend and or come up with better solutions for
//      operators.

//BEGINNING OF TNFA CLASS IMPLEMENTATION

TNFA::TNFA() : start_state(nullptr), accept_state(nullptr) {}

TNFA::TNFA(char c) : start_state(nullptr), accept_state(nullptr) {
    this->start_state = new State(false);
    this->accept_state = new State(true);
    this->start_state->AddSymbolTransition(this->accept_state, c);
}

TNFA::TNFA(State* the_start_state, State* the_accept_state) 
    : start_state(the_start_state), accept_state(the_accept_state) {}

TNFA::TNFA(const TNFA& a_tnfa) {
    Copy(a_tnfa);
}

TNFA::~TNFA() {
    Clear();
}

TNFA& TNFA::operator=(const TNFA& a_tnfa) {
    if(this == &a_tnfa) return *this;
    Clear();
    Copy(a_tnfa);
    return *this;
}

void TNFA::SetStartState(State* a_start_state) {
    this->start_state = a_start_state;
    return;
}

void TNFA::SetAcceptState(State* an_accept_state) {
    this->accept_state = an_accept_state;
    return;
}

State* TNFA::GetStartState(void) const {
    return this->start_state;
}

State* TNFA::GetAcceptState(void) const {
    return this->start_state; 
}

TNFA& TNFA::operator+=(char c) {
    if(!this->start_state) { //Signifies empty automaton.
        this->start_state = new State(false);
        this->accept_state = new State(true);
        this->start_state->AddSymbolTransition(accept_state, c);
        return *this;
    } 
    
    State* new_start_state = new State(false);
    State* new_accept_state = new State(true);
    
    new_start_state->AddSymbolTransition(new_accept_state, c);
    
    this->accept_state->acceptance = false;
    this->accept_state->AddEpsilonTransition(new_start_state);
    
    this->accept_state = new_accept_state;
    
    return *this;
}

TNFA TNFA::operator+(char c) const {
    State* rhs_start = new State(false);
    State* rhs_accept = new State(true);
    rhs_start->AddSymbolTransition(rhs_accept, c);
    
    //Check for empty lhs automaton, if empty, just return rhs automaton.
    if(!this->start_state) return TNFA(rhs_start, rhs_accept);
    
    //Copy lhs automaton.
    unordered_map<State*, State*> visited_state_map;
    State* lhs_accept = nullptr;
    State* lhs_start = CopyHelper(this->start_state, lhs_accept, visited_state_map);
    
    //Flip acceptance status for lhs.
    lhs_accept->acceptance = false;
    
    //Add a transition from lhs accept to rhs start.
    lhs_accept->AddEpsilonTransition(rhs_start);
    
    //Return new automaton.
    return TNFA(lhs_start, rhs_accept);
}

TNFA& TNFA::operator+=(const TNFA& a_tnfa) {
    //If other a_tnfa is empty, just return.
    if(!a_tnfa.start_state) return *this;
    
    //Copy a_tnfa to "rhs_start"
    unordered_map<State*, State*> visited_state_map;
    State* rhs_accept = nullptr;
    State* rhs_start = CopyHelper(a_tnfa.start_state, rhs_accept, visited_state_map);
    
    //Flip the acceptance level of the invoking accept state, add a transition
    //from the invoking accept state to the new start state, and set the
    //invoking accept state to the new accept state.
    this->accept_state->acceptance = false;
    this->accept_state->AddEpsilonTransition(rhs_start);
    this->accept_state = rhs_accept;
    
    //Return modified automaton.
    return *this;
}

TNFA TNFA::operator+(const TNFA& a_tnfa) const {
    unordered_map<State*, State*> visited_state_map;
    
    State* lhs_accept = nullptr;
    State* lhs_start = CopyHelper(this->start_state, lhs_accept, visited_state_map);
    
    visited_state_map.clear();
    
    State* rhs_accept = nullptr;
    State* rhs_start = CopyHelper(a_tnfa.start_state, rhs_accept, visited_state_map);
    
    lhs_accept->acceptance = false;
    lhs_accept->AddEpsilonTransition(rhs_start);
    
    return TNFA(lhs_start, rhs_accept);
}

TNFA& TNFA::operator|=(const TNFA& a_tnfa) {
    //Can't alternate with an empty automaton so return as is.
    if(!a_tnfa.start_state) return *this;
    
    //Setup alternation's start and accept state.
    State* new_start_state = new State(false);
    State* new_accept_state = new State(true);
    
    //Copy a_tnfa.
    unordered_map<State*, State*> visited_state_map;
    State* rhs_accept = nullptr;
    State* rhs_start = CopyHelper(a_tnfa.start_state, rhs_accept, visited_state_map);
    
    //Transition from new start state to both invoking start state and second start state.
    new_start_state->AddEpsilonTransition(this->start_state);
    new_start_state->AddEpsilonTransition(rhs_start);
    
    //Flip acceptance status of invoking accept state and rhs accept state.
    this->accept_state->acceptance = false;
    rhs_accept->acceptance = false;
    
    //Transition from invoking accept state and second accept state to 
    //the new accept_state.
    this->accept_state->AddEpsilonTransition(new_accept_state);
    rhs_accept->AddEpsilonTransition(new_accept_state);
    
    //Change start and accept states to the new start and accept
    //states.
    this->start_state = new_start_state;
    this->accept_state = new_accept_state;
    
    return *this; //Return modified automaton.
}

TNFA TNFA::operator|(const TNFA& a_tnfa) const {
    //Prep states for new alternated automaton.
    State* new_start_state = new State(false);
    State* new_accept_state = new State(true);
    
    unordered_map<State*, State*> visited_state_map; //Track states during copy.
    
    //Copy first automaton (the invoking one, left hand side).
    State* lhs_accept = nullptr;
    State* lhs_start = CopyHelper(this->start_state, lhs_accept, visited_state_map);
    
    //Reset state map for copying the second automaton (Technically unnecessary).
    visited_state_map.clear();
    
    //Copy second automaton (right hand side).
    State* rhs_accept = nullptr;
    State* rhs_start = CopyHelper(a_tnfa.start_state, rhs_accept, visited_state_map);
    
    //Flip acceptance status' of each accept state in preparation for the new 
    //alternated automaton.
    lhs_accept->acceptance = false;
    rhs_accept->acceptance = false;
    
    //Transition from new start state to lhs and rhs start states
    new_start_state->AddEpsilonTransition(lhs_start);
    new_start_state->AddEpsilonTransition(rhs_start);
    
    //Transition from lhs and rhs accept states to new accept state.
    lhs_accept->AddEpsilonTransition(new_accept_state);
    rhs_accept->AddEpsilonTransition(new_accept_state);
    
    //Return the final automaton.
    return TNFA(new_start_state, new_accept_state);
}

TNFA& TNFA::ApplyKleeneClosure(void) {
    //Can't apply kleene closure on an empty automaton so return as is.
    if(!this->start_state) return *this;
    
    //Create kleene closure's start and accept states.
    State* new_start_state = new State(false);
    State* new_accept_state = new State(true);
    
    //Add transitions from new start to the invoking start and new accept.
    new_start_state->AddEpsilonTransition(this->start_state);
    new_start_state->AddEpsilonTransition(new_accept_state);
    
    //Add transitions from invoking accept state to invoking start and
    //new accept state.
    this->accept_state->AddEpsilonTransition(this->start_state);
    this->accept_state->AddEpsilonTransition(new_accept_state);
    
    //Flip acceptance level
    this->accept_state->acceptance = false;
    
    //Set invoking start and accept states to the new start and accept states.
    this->start_state = new_start_state;
    this->accept_state = new_accept_state;
    
    //Return modified automaton.
    return *this;
}

void TNFA::Copy(const TNFA& a_tnfa) {
    //If other "a_tnfa" is empty, clear the invoking tnfa.
    if(!a_tnfa.start_state) {
        Clear();
        return;
    }
    
    //Copy "a_tnfa" to "second_start"
    unordered_map<State*, State*> visited_state_map;
    State* new_accept = nullptr;
    State* new_start = CopyHelper(a_tnfa.start_state, new_accept, visited_state_map);
    this->start_state = new_start;
    this->accept_state = new_accept;
    return;
}

State* TNFA::CopyHelper(State* source_state, State*& accept_state, unordered_map<State*, State*>& visited_state_map) const {
    //Will store the copied state from source_state
    State* copy_state = new State(source_state->acceptance); 
    
    //Make sure to properly set the new accept state.
    if(copy_state->acceptance) accept_state = copy_state;
    
    //Pair up the source state to the corresponding copied state, for easy
    //retrieval in the future.
    visited_state_map.insert({source_state, copy_state});
    
    //Now visit neighboring states
    for(const auto& [symbol, next_states] : source_state->symbol_transitions) {
        for(State* next_state : next_states) {
            if(visited_state_map.count(next_state) == 0) {
                copy_state->symbol_transitions[symbol].push_back(CopyHelper(next_state, accept_state, visited_state_map));    
            } else {
                copy_state->symbol_transitions[symbol].push_back(visited_state_map.at(next_state));
            }
        }
    }
    
    for(State* next_state : source_state->epsilon_transitions) {
        if(visited_state_map.count(next_state) == 0) {
            copy_state->epsilon_transitions.push_back(CopyHelper(next_state, accept_state, visited_state_map));
        } else {
            copy_state->epsilon_transitions.push_back(visited_state_map.at(next_state));
        }
    }
    
    //Connect up the TNFA graph as the recursion stack closes.
    return copy_state;
}

void TNFA::Clear(void) {
    if(!this->start_state) return;
    unordered_set<State*> visited;
    ClearHelper(this->start_state, visited);
}

void TNFA::ClearHelper(State* current_state, unordered_set<State*>& visited) {
    visited.insert(current_state); //Mark visited.
    
    //Traverse transitions.
    for(const auto& [symbol, next_states] : current_state->symbol_transitions) {
        for(State* next_state : next_states) {
            if(visited.count(next_state) == 0) {
                ClearHelper(next_state, visited);
            }
        }
    }
    
    for(State* next_state : current_state->epsilon_transitions) {
        if(visited.count(next_state) == 0) {
            ClearHelper(next_state, visited);
        }
    }
    
    if(current_state) delete current_state;
    return;
}

//END OF TNFA CLASS IMPLEMENTATION

TNFA KleeneClosure(const TNFA& a_tnfa) {
    //Setup start and accept states for the new kleen automaton.
    State* new_start_state = new State(false);
    State* new_accept_state = new State(true);
    
    //Track copied states.
    unordered_map<State*, State*> visited_state_map;
    State* accept_copy = nullptr;
    State* start_copy = a_tnfa.CopyHelper(a_tnfa.start_state, accept_copy, visited_state_map);
    
    //Add transition to start copy and new accept state.
    new_start_state->AddEpsilonTransition(start_copy);
    new_start_state->AddEpsilonTransition(new_accept_state);
    
    //Transition from accept copy to start copy and new accept.
    accept_copy->AddEpsilonTransition(start_copy);
    accept_copy->AddEpsilonTransition(new_accept_state);
    
    //Flip acceptance status of accept copy.
    accept_copy->acceptance = false;
    
    //Return the new automaton.
    return TNFA(new_start_state, new_accept_state);
}

    
