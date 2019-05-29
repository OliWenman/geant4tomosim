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
#include "Exceptions.hh"

struct Flag
{
    Flag(int _parameter, std::string _name, std::map<std::string, double> _units) {parameter = _parameter, name = _name; units = _units; hasUnits = true; optional = true;}
    Flag(int _parameter, std::string _name)                                       {parameter = _parameter, name = _name, hasUnits = false; optional = true;}
    Flag(int _parameter)                                                          {parameter = _parameter, hasUnits = false; optional = true;}

    std::string name;
    bool optional;
    bool hasUnits;
    int  parameter;
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
        std::vector<std::string> tokens;
   
        void PrintInput(){std::cout << "\ninput = " << input << std::endl;}
   
   private:
        std::string input;
   
        G4UIcommand* Command;
        std::vector<Flag> FlagContainer;
};

