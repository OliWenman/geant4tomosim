#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"

/// Modular physics list
/// - G4EmStandardPhysics

class PhysicsListMessenger;

class PhysicsList: public G4VModularPhysicsList
{
	public:
  		PhysicsList();
  		virtual ~PhysicsList();

  		virtual void SetCuts();
		
		//Set methods
		void SetPhysicsOption(G4String value){PhysicsProcessCmd = value;}
	
		//Get methods
		G4String GetPhysicsOption(){return PhysicsProcessCmd;}

	private:
		PhysicsListMessenger* physicsMessenger;
		
		G4String PhysicsProcessCmd;
};

#endif

