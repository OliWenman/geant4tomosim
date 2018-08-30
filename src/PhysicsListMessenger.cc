#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* PLObject):Physics(PLObject)
{
  	physicsDirectory = new G4UIdirectory("/physics/");
  	physicsDirectory->SetGuidance("Commands to activate physics models and set cuts");

  	PhysicsListCmd = new G4UIcmdWithAString("/physics/addPhysics",this);  
  	PhysicsListCmd->SetGuidance("Add physics list.");
  	PhysicsListCmd->SetParameterName("PList",false);
  	PhysicsListCmd->AvailableForStates(G4State_PreInit);  
}

PhysicsListMessenger::~PhysicsListMessenger()
{
  	delete PhysicsListCmd;
  	delete physicsDirectory;    
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{       
  		if( command == PhysicsListCmd )
   		{ 
			G4cout << G4endl << "PhysicsList -> AddPhysicsList command detected " << G4endl;
			Physics -> AddPhysicsList(newValue);
		}
}
