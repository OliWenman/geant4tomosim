#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* PLObject):G4UImessenger(), Physics(PLObject)
{	
	G4cout << G4endl << "PhysicsListMessenger has been created " << G4endl;

	physicsDirectory = new G4UIdirectory("/physics/");
	physicsDirectory -> SetGuidance("Commands to control what physics processes are involved");

	PhysicsProcessCmd = new G4UIcmdWithAString("/physics/process", this);
	PhysicsProcessCmd -> SetDefaultValue("EmStandard");
	PhysicsProcessCmd -> SetGuidance("Choose what physics processes you would like to have ");
}

PhysicsListMessenger::~PhysicsListMessenger()
{
	delete physicsDirectory;
	delete PhysicsProcessCmd;

	G4cout << G4endl << "PhysicsListMessenger has been deleted " << G4endl;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == PhysicsProcessCmd)
	{
		Physics -> SetPhysicsOption(newValue);
		G4cout << G4endl << "PhysicsList -> SetPhysicsOption command detected" << G4endl;
	}
}
