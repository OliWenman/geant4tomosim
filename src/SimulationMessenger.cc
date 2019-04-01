#include "SimulationMessenger.hh"
#include "Simulation.hh"
#include "Data.hh"
#include <stdlib.h>
#include <climits>

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

SimulationMessenger::SimulationMessenger(Simulation* SimObject): sim(SimObject)
{	
	SimulationDirectory = new G4UIdirectory("/data/");
	SimulationDirectory -> SetGuidance("Commands to control Data class");

	seedCmd = new G4UIcmdWithAnInteger("/data/Seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	seedCmd -> SetParameterName("seed", true);
	seedCmd -> SetRange("seed > -1");
	
	verboseCmd = new G4UIcmdWithAnInteger("/data/verbose", this);
	verboseCmd -> SetParameterName("verbose", true);
	verboseCmd -> SetRange("verbose > -1");
	
	globalVerboseCmd = new G4UIcmdWithAnInteger("/simulation/verbose", this);
	globalVerboseCmd -> SetParameterName("verbose", true);
	globalVerboseCmd -> SetRange("verbose > -1");
}

SimulationMessenger::~SimulationMessenger()
{
	delete SimulationDirectory;
	delete seedCmd;
	delete verboseCmd;
	delete globalVerboseCmd;
}

void SimulationMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == seedCmd ){
		sim -> SetSeed(seedCmd -> GetNewIntValue(newValue));	
	}
	else if( command == verboseCmd){
	    sim -> SetVerboseLevel(seedCmd -> GetNewIntValue(newValue));	
	}
	else if( command == globalVerboseCmd)
	{
	    sim -> SetVerboseLevel(globalVerboseCmd -> GetNewIntValue(newValue));
	}
}
