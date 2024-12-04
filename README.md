This project was written in c++17. A small program utilizing the engine can be ran here: https://onlinegdb.com/BVRgjri0ni

I wanted to make this project as a fun little exercise to accompany my interest in finite state automata and regular expressions after first learning about them in a
discrete structures course I took in college.

I eventually want to incorporate some of the common features I've seen in other regular expression engines such as character classes and functions for finding and replacing pattern matches 
in strings.

What it does:
  - Preprocesses an input regular expression pattern to make concatenations explicit symbols.
  - Converts the resulting pattern to its post fix form using the shunting yard algorithm.
  - Evaluates the post fix pattern. When a character is found, an NFA is constructed according to thompsons
    construction algorithm and pushed onto a stack of NFA's. When an operator is found, the appropriate number
    of NFA's are popped off the stack and combined in the way the operator intended, also according to
    thompsons construction algorithm. The only NFA left in the stack is the final NFA representation of the input pattern.
  - Provides functions for string matching.

FIXED 12/4/2024! -> NOTE: Small bug in my match function not working correctly with NFA's containing epsilon loops. Currently working on a better algorithm.
