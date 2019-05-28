/*
A class to control and manage physics used for a simulation.
Note: Using Geant4 version 10.4 doesn't allow for changing
fluorescence on or off once set due to a lock method done
internally inside G4VAtomDeexcitation.

Author: Oliver Jack Wenman
*/

#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "SettingsLog.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class G4StepLimiter;

class G4PhotoElectricEffect;
class G4LivermorePhotoElectricModel;
class G4LivermorePolarizedPhotoElectricModel;

class G4ComptonScattering;
class G4LivermoreComptonModel;

class G4RayleighScattering;
class G4LivermoreRayleighModel;

class G4VAtomDeexcitation;

class GammaOpticalRefraction;
class GammaOpticalAbsorption;

class StepMax;

class PhysicsList: public G4VModularPhysicsList
{
	public:
  		PhysicsList();
  		~PhysicsList();
/*
        ConstructEM(), ConstructParticle() and ConstructProcess() are inherited methods
        from G4VModularPhysicsList.
*/
        //Construct the particles and processes
		void ConstructEM();
  		void ConstructParticle();
  		void ConstructProcess();
		void SetCuts();
		void AddStepMax();

		//Used to set the type of phyiscs package you want
		void SetPhysicsPackage(G4String value){PhysicsPackageCmd = value;}

		//Used to set the physics processes you want
		void SetPhotoElectric(G4bool value)     {photoelectricOn = value;}
		void SetComptonScattering(G4bool value) {comptonscatteringOn = value;}
		void SetRayleighScattering(G4bool value){rayleighscatteringOn = value;}
		void SetFluorescence(G4bool value)      {fluorescenceOn = value;}
		void SetRefraction(G4bool value)        {refractionOn = value;}
		void SetGammaAbsorption(G4bool value)   {gamma_absorptionOn = value;}

		G4bool GetFluorescence(){return fluorescenceOn;}

        void ActivateUserPhysics();     

		//Read and save info user has inputted
		void ReadOutInfo(SettingsLog& log);

	private:
		//Pointer to its messenger class
		PhysicsListMessenger* physicsMessenger;
		
		//Pointers to the physics processes and models
		G4PhotoElectricEffect* photoelectriceffect;
		//G4LivermorePolarizedPhotoElectricModel* livpol_photoeletriceffect;
		G4LivermorePhotoElectricModel* liv_photoelectric; 
		
		G4ComptonScattering*     comptonscattering;
		G4LivermoreComptonModel* liv_comptonscattering;
		
		G4RayleighScattering*     rayleighscattering;
		G4LivermoreRayleighModel* liv_rayleighscattering;
		
		GammaOpticalRefraction* gamma_refraction;
		GammaOpticalAbsorption* gamma_absorption;	
		
		StepMax* fStepMaxProcess;	

		//Physics options
		G4String PhysicsPackageCmd;
		G4bool photoelectricOn;
		G4bool comptonscatteringOn;
		G4bool rayleighscatteringOn;
		G4bool fluorescenceOn;
		G4bool refractionOn;
		G4bool gamma_absorptionOn;
		
		//Cuts for particles
		G4double cutForGamma;
  		G4double cutForElectron;
  		G4double cutForPositron;
};

#endif
