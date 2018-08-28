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
	G4cout << G4endl << "RunActionMessenger has been created " << G4endl;

	runDirectory = new G4UIdirectory("/run/");
	runDirectory -> SetGuidance("Commands to control RunAction class");

	seedCmd = new G4UIcmdWithAnInteger("/run/seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Choose random seed (0) or input your own. ");

	TextFileCmd = new G4UIcmdWithAString("/run/TextFile", this);
	TextFileCmd -> SetDefaultValue("true");
	TextFileCmd -> SetGuidance("Choose if you want to save your simulation data to a text file ");
}

RunActionMessenger::~RunActionMessenger()
{
	delete seedCmd;
	delete TextFileCmd;

	G4cout << G4endl << "RunActionMessenger has been deleted " << G4endl;
}

void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == seedCmd )
	{
		RAction -> SetSeedOption(seedCmd -> GetNewIntValue(newValue));
		G4cout << G4endl << "RunAction -> SetSeedOption command detected" << G4endl;
	}
	else if (command == TextFileCmd)
	{
		RAction -> SetTextFileOption(newValue);
		G4cout << G4endl << "RunAction -> SetTextFileOption command detected" << G4endl;
	}
}
