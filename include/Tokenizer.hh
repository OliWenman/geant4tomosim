/*
Class used to tokenize strings. Can have optinal Flags, used for
error checking a string command.

Author: Oliver Jack Wenman
*/

#ifndef Tokenizer_h
#define Tokenizer_h 1

#include <bits/stdc++.h>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <sstream> 
#include "G4UIcommand.hh"

struct Flag
{
    Flag(std::string Name, std::map<std::string, double> Units) {name = Name; units = Units; hasUnits = true;}
    Flag(std::map<std::string, double> Units, std::string Name) {name = Name; units = Units; hasUnits = true;}
    Flag(std::string Name)                                      {name = Name, hasUnits = false;}
    Flag()                                                      {hasUnits = false;}

    std::string name;
    bool hasUnits;
    std::map<std::string, double> units;
};

struct doubleArray
{
    double* values;
    size_t size;
};

#endif

class Tokenizer
{
    public:
        Tokenizer(std::string input, G4UIcommand* command);
        ~Tokenizer();
        
        void SetFlags(std::vector<Flag> flags){FlagContainer = flags;}
        
        //Would use template <typename T> T but encounter bug "ImportError: ./../src/sim.so: undefined symbol" when importing from python
        double FindDouble(Flag flag);
        std::string FindString(Flag flag);
        doubleArray FindArray(Flag flag);
   
        void PrintInput(){std::cout << "\ninput = " << input << std::endl;}
   
   private:
        std::string input;
   
        G4UIcommand* Command;
        std::vector<Flag> FlagContainer;
        std::vector<std::string> tokens;
};

