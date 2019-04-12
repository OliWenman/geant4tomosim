#ifndef CommandStatus_h
#define CommandStatus_h 1

#include "G4UIcommandStatus.hh"

//Whenever a command is used, an int is returned to tell if it was successful or not.
//If not, an error message can appear depending on the number

enum CommandStatus
{
    //Geant4 defined ones
    //fCommandSucceeded         =   0,
    //fCommandNotFound          = 100,
    //fIllegalApplicationState  = 200,
    //fParameterOutOfRange      = 300,
    //fParameterUnreadable      = 400,
    //fParameterOutOfCandidates = 500,
    //fAliasNotFound            = 600,
  
    //My own ones
    fNotEnoughParameters    = 1,
    fParameterNotADouble    = 2,
    fParameterNotAInteger   = 3,
    fIncorrectUnit          = 4,
    fFlagNotFound           = 5,
    fParameterNotFound      = 6,
    fParameterAlreadyExists = 7 
};

#endif
