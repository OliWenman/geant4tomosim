#include "RunActionMessenger.hh"
#include "RunAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

RunActionMessenger::RunActionMessenger(RunAction *run):G4UImessenger(), RAction(run)
{	
	runDirectory = new G4UIdirectory("/run/");
	runDirectory -> SetGuidance("Commands to control RunAction class");

	seedCmd = new G4UIcmdWithAnInteger("/run/seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Choose random seed (0) or input your own. ");
}

RunActionMessenger::~RunActionMessenger()
{
	delete seedCmd;
}

void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == seedCmd )
	{
		RAction -> SetSeedOption(seedCmd -> GetNewIntValue(newValue));
		G4cout << G4endl << G4endl << "Seed value is " << seedCmd << G4endl << G4endl;
	}
}
