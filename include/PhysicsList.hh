#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class Data;

class PhysicsList: public G4VModularPhysicsList
{
	public:

  		PhysicsList(Data* data);
  		virtual ~PhysicsList();

  		void ConstructParticle();

  		void AddPhysicsList(G4String& name);
  		void ConstructProcess();
		void SetCuts(G4double aValue);
		//void SetCuts();

	private:
		Data* data;

  		G4EmConfigurator em_config;

  		G4double cutForGamma;
  		G4double cutForElectron;
  		G4double cutForPositron;

  		G4String emName;
  		G4VPhysicsConstructor* emPhysicsList;
  		G4VPhysicsConstructor* decPhysicsList;

  		PhysicsListMessenger* PhysicsMessenger;
};

#endif
