#include "MultiParameters.hh"
#include "MyG4UImessenger.hh"

#include "CommandStatus.hh"

MultiParameters::MultiParameters(const char *theCommandPath, MyG4UImessenger *theMessenger) : G4UIcmdWithAString(theCommandPath, theMessenger), ownmessenger(theMessenger)
{
    numberofparameters = 1;
    moreparameters = false;
}

G4int MultiParameters::CheckCorrectNumberOfParameters(G4String parameterList)
{
    char buffer;
    int parametercounter = 1;

    //Loop through the parameters
    for (int n = 0; n < parameterList.size() ; n++)
    {
        //Check for a space that seperate the parameters
        if (parameterList[n] != ' ')
        {
            buffer = parameterList[n];
            continue;
        }    
        else
        {
            //If the last character was a space, continue
            if (buffer == ' ')
            {
                continue;
            }
            else
            {
                //If character has changed to a space, then add one to the parametercounter
                buffer = parameterList[n];
                parametercounter++;
            }
        }
    }
    
    if (parametercounter != numberofparameters)
    {
        if (moreparameters == true && parametercounter > numberofparameters)
        {
            return 0;
        }
        else
        {
            return fNotEnoughParameters;
        }
    }    
    
    return 0;
}

G4int MultiParameters::DoIt(G4String parameterList) 
{
    int success = 0;

    if (CheckCorrectNumberOfParameters(parameterList) == fNotEnoughParameters) {return fNotEnoughParameters;}
        
    int checkifokay = G4UIcmdWithAString::DoIt(parameterList);    
    if (checkifokay != success) {return checkifokay;}

    int commandstatus = ownmessenger->ApplyCommand(this, parameterList);
    
    return commandstatus; 
}

