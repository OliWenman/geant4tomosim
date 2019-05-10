#include "SimulationMessenger.hh"
#include "Simulation.hh"
#include <stdlib.h>
#include <climits>

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"

SimulationMessenger::SimulationMessenger(Simulation* SimObject): sim(SimObject)
{	
	directory = new G4UIdirectory("/data/");
	directory -> SetGuidance("Commands to control Data class");

	setseed = new G4UIcmdWithAnInteger("/simulation/seed", this);
	setseed -> SetDefaultValue(0);
	setseed -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	setseed -> SetParameterName("seed", true);
	setseed -> SetRange("seed > -1");
	
	setgraphics = new G4UIcmdWithAString("/simulation/graphics", this);
	setgraphics -> SetGuidance("Set if you want the simulation to output a file that can viewed to see the geometry");
	
	progressSingleline = new G4UIcmdWithoutParameter("/simulation/progress/singleline", this);
	
	applymacrofile = new G4UIcmdWithAString("/tomosim/execute", this);
	applymacrofile->SetGuidance("Execute a macro file, which contains a list of commands, with error checking and handling");
}

SimulationMessenger::~SimulationMessenger()
{
	delete directory;
	delete setseed;
	delete setgraphics;
	delete progressSingleline;
	delete applymacrofile;
}

void SimulationMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == setseed )
	{
		sim -> SetSeed(setseed -> GetNewIntValue(newValue));	
	}
	else if ( command == setgraphics)
	{
	
	}
	else if (command == progressSingleline)
	{
	    
	}
	else if (command == applymacrofile)
	{
	    sim->applymacrofile_pywrapped(newValue);
	}
}
