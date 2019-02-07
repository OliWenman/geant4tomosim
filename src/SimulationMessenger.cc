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

	WriteToTextCmd = new G4UIcmdWithABool("/data/WriteTextFile", this);
	WriteToTextCmd -> SetGuidance("Choose if you would like the data to be saved to a text file ");
	WriteToTextCmd -> SetDefaultValue(true);

	NoImages_Cmd = new G4UIcmdWithAnInteger("/data/NumberOfImages", this);
	NoImages_Cmd -> SetDefaultValue(2);
	NoImages_Cmd -> SetGuidance("Pick the number of images you would to have taken ");
	NoImages_Cmd -> SetParameterName("nImages", true);
	NoImages_Cmd -> SetRange("nImages > 0");

	NoPhotons_Cmd = new G4UIcmdWithAString("/data/NumberOfPhotons", this);
	NoPhotons_Cmd -> SetGuidance("Pick the number of photons you would to have per image ");

	seedCmd = new G4UIcmdWithAnInteger("/data/Seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	seedCmd -> SetParameterName("seed", true);
	seedCmd -> SetRange("seed > -1");
	
	verboseCmd = new G4UIcmdWithAnInteger("/data/verbose", this);
	verboseCmd -> SetParameterName("verbose", true);
	verboseCmd -> SetRange("verbose > -1");
}

SimulationMessenger::~SimulationMessenger()
{
	delete SimulationDirectory;
	delete WriteToTextCmd;
	delete NoImages_Cmd;
	delete NoPhotons_Cmd;
	delete seedCmd;
	delete verboseCmd;
}

void SimulationMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WriteToTextCmd ){ 
		sim -> SetWriteToText(WriteToTextCmd -> GetNewBoolValue(newValue));
	}	
	else if( command == NoImages_Cmd ){
		//input -> SetNoImages(NoImages_Cmd -> GetNewIntValue(newValue));	
	}
	else if( command == NoPhotons_Cmd ){
		//input -> SetNoPhotons(newValue);
	}
	else if( command == seedCmd ){
		sim -> SetSeed(seedCmd -> GetNewIntValue(newValue));	
	}
	else if( command == verboseCmd){
	    sim -> SetVerboseLevel(seedCmd -> GetNewIntValue(newValue));	
	}
}
