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
	directory = new G4UIdirectory("/data/");
	directory -> SetGuidance("Commands to control Data class");

	setseed = new G4UIcmdWithAnInteger("/simulation/seed", this);
	setseed -> SetDefaultValue(0);
	setseed -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	setseed -> SetParameterName("seed", true);
	setseed -> SetRange("seed > -1");
	
	setgraphics = new G4UIcmdWithAString("/simulation/graphics", this);
	setgraphics -> SetGuidance("Set if you want the simulation to output a file that can viewed to see the geometry");
}

SimulationMessenger::~SimulationMessenger()
{
	delete directory;
	delete setseed;
	delete setgraphics;
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
}
