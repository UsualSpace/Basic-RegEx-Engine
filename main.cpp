/*
 * Filename: main.cpp
 * Programmer: Abdurrahman Alyajouri
 * Date: 11/25/2024
 * Purpose: The purpose of this file is to test out basic regex operations
 *          implemented in other files.
 */

#include "regex.h"
#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;

int main() {
    
    //Pattern that matches the set of strings that begin with either "Hello"
    //or "Hi", then a space and then "World" followed by 0 or more instances
    //of the character 's'. Ex: "Hello Worldssssss".
    Regex re("((Hello)|(Hi)) Worlds*");
    
    string s;
    
    do {
        cout << "Enter a string that matches \"" + re.GetPattern() + "\" (Enter \"quit\" to quit): ";
        getline(cin, s);
        if(re.Match(s)) cout << '"' + s + "\" matches!\n";
        else cout << '"' + s + "\" DOES NOT MATCH!!!11!\n";
    } while(cin.good() && s != "quit");
    
    return 0;
}

