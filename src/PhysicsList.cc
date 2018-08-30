#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
#include "Data.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

//Physic lists (contained inside the Geant4 distribution)
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4DecayPhysics.hh"

PhysicsList::PhysicsList(Data* DataObject) : G4VModularPhysicsList(), data(DataObject)
{
  	PhysicsMessenger = new PhysicsListMessenger(this);

  	//EM physics
  	emPhysicsList = new G4EmStandardPhysics();

  	//Deacy physics and all particles
 	decPhysicsList = new G4DecayPhysics();

	G4double Cutvalue = 0;
}

PhysicsList::~PhysicsList()
{
  	delete PhysicsMessenger;
  	delete emPhysicsList;
  	delete decPhysicsList;
	delete data;
}

void PhysicsList::ConstructParticle()
{
  	decPhysicsList->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
  	//Transportation
  	AddTransportation();

  	//Electromagnetic physics list
  	emPhysicsList->ConstructProcess();
  	em_config.AddModels();

  	//Decay physics list
  	decPhysicsList->ConstructProcess();
}

void PhysicsList::AddPhysicsList(G4String& name)
{
  	if (name == "EMStandardPhysics")
	{
		G4cout << G4endl << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics" << G4endl;
		data -> SetPhysicsUsed(name);
	}
  	else if (name == "EMStandardPhysics_option3") 
	{
    		emName = name;
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option3();
    		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option3" << G4endl;
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmLivermorePhysics") 
	{
    		emName = name;
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLivermorePhysics();
    		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmLivermorePhysics" << G4endl;
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmPenelopePhysics") 
	{
    		emName = name;
    		delete emPhysicsList;
    		emPhysicsList = new G4EmPenelopePhysics();
    		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmLivermorePhysics" << G4endl;
		data -> SetPhysicsUsed(name);
	}
}

void PhysicsList::SetCuts(G4double Cutvalue)
{
	SetCutValue(Cutvalue, "gamma");
  	SetCutValue(Cutvalue, "e-");
  	SetCutValue(Cutvalue, "e+");
}

