/*
Used to determine the return type of a command. Can output to
the user the error of the message. Expands upon Geant4's version
with my own custom return types.

Author: Oliver Jack Wenman
*/

#ifndef CommandStatus_h
#define CommandStatus_h 1

#include "G4UIcommandStatus.hh"

enum CommandStatus
{
/*
    Geant4 defined ones included in G4UIcommandStatus.hh
    fCommandSucceeded         =   0,
    fCommandNotFound          = 100,
    fIllegalApplicationState  = 200,
    fParameterOutOfRange      = 300,
    fParameterUnreadable      = 400,
    fParameterOutOfCandidates = 500,
    fAliasNotFound            = 600,
*/
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
