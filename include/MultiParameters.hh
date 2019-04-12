//Class inherits from G4UIcmdWithAString, gives more flexibility for commands. Complex inputs
//now possible with several different types inputted in a single line. Very useful when
//constructing sample geometry or defining new materials via commands. DoIt method is modified
//that allows to get the return type from the ApplyCommand method. G4's SetNewValue method has
//no return type, therefore can't do error checking when applying the command. Overriding the 
//the DoIt method allows for using the MyG4UImessenger function ApplyCommand that has a return type
//The return type therefore can be modified within the ApplyCommand method

#ifndef MultiParameters_h
#define MultiParameters_h 1

#include "G4UIcmdWithAString.hh"
#include <string>
#include "MyVectors.hh"

class MyG4UImessenger;

class MultiParameters : public G4UIcmdWithAString
{
    public:
        MultiParameters(const char *theCommandPath, MyG4UImessenger *theMessenger);
        void SetNumberOfParameters(int value) {numberofparameters = value;}

        G4int DoIt(G4String parameterList);
        
    private:
        G4int CheckCorrectNumberOfParameters(G4String parameterList);
        G4int CheckCorrectType(G4String parameterList);
        
        G4int numberofparameters;               
        MyG4UImessenger *ownmessenger;
};

#endif


