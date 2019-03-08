#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "SettingsLog.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class G4StepLimiter;

class G4Cerenkov;
class G4Scintillation;
class G4OpAbsorption;
class G4OpRayleigh;
class G4OpMieHG;
class G4OpBoundaryProcess;

class PhysicsList: public G4VModularPhysicsList
{
	public:
  		PhysicsList();
  		~PhysicsList();

		//Used to set the type of phyiscs package you want
		void SetPhysicsPackage(G4String value){PhysicsPackageCmd = value;}

		//Used to set the physics processes you want
		void SetPhotoElectric(G4bool value)     {PhotoElectricCmd = value; if(value){PhysicProcesses.push_back("  - Photoelectric effect");}}
		void SetComptonScattering(G4bool value) {ComptonScatteringCmd = value; if(value){PhysicProcesses.push_back("  - Compton scattering");}}
		void SetRayleighScattering(G4bool value){RayleighScatteringCmd = value; if(value){PhysicProcesses.push_back("  - Rayleigh scattering");}}
		void SetFluorescence(G4bool value)      {FluorescenceCmd = value; if(value){PhysicProcesses.push_back("- Fluorescence");}}
		void SetRefraction(G4bool value)        {RefractionCmd = value; if(value){PhysicProcesses.push_back("- Refraction");}}
		void SetGammaAbsorption(G4bool value)   {GammaAbsorption = value; if(value){PhysicProcesses.push_back("- Optical absorption");}}

		G4bool GetFluorescence(){return FluorescenceCmd;}

		//Construct the particles and processes
		void ConstructEM();
  		void ConstructParticle();
  		void ConstructProcess();
		void SetCuts();

		//Read and save info user has inputted
		void ReadOutInfo(SettingsLog& log);

	private:
		//Pointer to its messenger class
		PhysicsListMessenger* PhysicsMessenger;

  		G4double cutForGamma;
  		G4double cutForElectron;
  		G4double cutForPositron;

		//Physics options
		G4String PhysicsPackageCmd;
		G4bool PhotoElectricCmd;
		G4bool ComptonScatteringCmd;
		G4bool RayleighScatteringCmd;
		G4bool FluorescenceCmd;
		G4bool RefractionCmd;
		G4bool GammaAbsorption;

		//Vector used to read out the physics processes used
		std::vector<G4String> PhysicProcesses;
};

#endif
