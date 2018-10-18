#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class Input;

class PhysicsList: public G4VModularPhysicsList
{
	public:

  		PhysicsList(Input* input);
  		~PhysicsList();

		void Print(G4String name);
		void ConstructEM(G4String Physics);
  		void ConstructParticle();
  		void AddPhysicsList(G4String& name);
  		void ConstructProcess();
		void SetCuts(G4double aValue);
	
		void SetPhysicsUsed(G4String value){PhysicsUsed = value;}
		G4String GetPhysicsUsed(){return PhysicsUsed;}

		G4String LivermoreGamma = "LivermoreGamma";
		G4String LMPhotoElectricEffect = "LMPhotoElectricEffect";

	private:
		Input* input;

  		G4EmConfigurator em_config;

  		G4double cutForGamma;
  		G4double cutForElectron;
  		G4double cutForPositron;
		G4String PhysicsUsed;

  		G4VPhysicsConstructor* emPhysicsList;

  		PhysicsListMessenger* PhysicsMessenger;
};

#endif
