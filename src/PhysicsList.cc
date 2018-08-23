#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
	G4cout << G4endl << "PhysicsList has been created " << G4endl;
	SetVerboseLevel(1);

  	// EM physics
  	RegisterPhysics(new G4EmStandardPhysics());

}

PhysicsList::~PhysicsList(){ }

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}  


