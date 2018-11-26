#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class G4StepLimiter;

class PhysicsList: public G4VModularPhysicsList
{
	public:
  		PhysicsList();
  		~PhysicsList();

		void SetPhotoElectric(G4bool value){PhotoElectricCmd = value;}
		void SetComptonScattering(G4bool value){ComptonScatteringCmd = value;}
		void SetRayleighScattering(G4bool value){RayleighScatteringCmd = value;}
		void SetFluorescence(G4bool value){FluorescenceCmd = value;}
		void SetPhysicsPackage(G4String value){PhysicsPackageCmd = value;}

		void ConstructEM();
  		void ConstructParticle();
  		void ConstructProcess();
		void SetCuts(G4double aValue);

		void ReadOutInfo(G4String SaveFilePath);

	private:
		//G4StepLimiter* fStepLimit;

  		G4double cutForGamma;
  		G4double cutForElectron;
  		G4double cutForPositron;
		G4String PhysicsPackageCmd;

		G4bool PhotoElectricCmd;
		G4bool ComptonScatteringCmd;
		G4bool RayleighScatteringCmd;
		G4bool FluorescenceCmd;

		std::vector<G4String> PhysicProcesses;

		//G4EmConfigurator em_config;
  		//G4VPhysicsConstructor* emPhysicsList;

  		PhysicsListMessenger* PhysicsMessenger;
};

#endif
