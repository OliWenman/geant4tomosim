//A intermediate class that inherits from G4UImessenger. Needed as its SetNewValue
//method has no return type. Return type useful to make custom error checks for complex
//commands that have multiple inputs of different types

#ifndef MyG4UImessenger_h
#define MyG4UImessenger_h 1

#include "G4UImessenger.hh"
class G4UIcommand;

class MyG4UImessenger : public G4UImessenger
{
    public:
        MyG4UImessenger();
        virtual ~MyG4UImessenger();
    
        virtual int  ApplyCommand(G4UIcommand* command, G4String newValue);
        //virtual void SetNewValue (G4UIcommand* command, G4String newValue);
};

#endif
