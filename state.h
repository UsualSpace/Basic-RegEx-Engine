#pragma once

/*
 * Filename: state.h 
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/24/2025
 * Purpose: The purpose of this file is to define the State struct.
 */

#include <vector>
#include <unordered_map>

struct State {
    //Ctors.
    State() : acceptance(false) {}
    State(bool an_acceptance) : acceptance(an_acceptance) {}
    
    //Insertion wrappers for clarity.
    void AddSymbolTransition(State* destination, char symbol) {
        this->symbol_transitions[symbol].push_back(destination);
    }
    
    void AddEpsilonTransition(State* destination) {
        this->epsilon_transitions.push_back(destination);
    }
    
    //Tracks whether the state is accepting state or not.
    bool acceptance; 
    
    //Tracks symbol transitions.
    std::unordered_map<char, std::vector<State*>> symbol_transitions; 
    
    //Tracks states accessible via epsilon transitions.
    std::vector<State*> epsilon_transitions; 
};
