#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4ProcessManager.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* PLObject):Physics(PLObject)
{
  	physicsDirectory = new G4UIdirectory("/physics");
  	physicsDirectory -> SetGuidance("Commands to activate physics models and set cuts");

  	PhysicsPackageCmd = new G4UIcmdWithAString("/physics/addPhysics/EM",this);  
  	PhysicsPackageCmd -> SetGuidance("Choose the type of physics package to use"); 

	PhotoElectricCmd = new G4UIcmdWithABool("/physics/gamma/livermore/photoelectric",this);  
  	PhotoElectricCmd -> SetGuidance("Choose if you want the photoelectric effect true/false");

	ComptonScatteringCmd = new G4UIcmdWithABool("/physics/gamma/livermore/comptomscattering",this);  
  	ComptonScatteringCmd -> SetGuidance("Choose if you want compton scattering true/false");
	
	RayleighScatteringCmd = new G4UIcmdWithABool("/physics/gamma/livermore/raylieghscattering",this);  
  	RayleighScatteringCmd -> SetGuidance("Choose if you want rayliegh scattering true/false");

	FluorescenceCmd = new G4UIcmdWithABool("/physics/gamma/fluorescence",this);  
  	FluorescenceCmd -> SetGuidance("Choose if you want fluorescence true/false");
  	
  	GammaRefractionCmd = new G4UIcmdWithABool("/physics/gamma/refraction", this);
  	GammaRefractionCmd -> SetGuidance("Choose if you want refraction true/false");
  	
  	GammaAbsorptionCmd = new G4UIcmdWithABool("/physics/gamma/opticalabsorption", this);
}

PhysicsListMessenger::~PhysicsListMessenger()
{
    delete physicsDirectory;   
  	delete PhysicsPackageCmd;
  	
	delete PhotoElectricCmd;
	delete ComptonScatteringCmd;
	delete RayleighScatteringCmd;
	delete FluorescenceCmd;
  	delete GammaRefractionCmd;
  	delete GammaAbsorptionCmd;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{       
  	if( command == PhysicsPackageCmd ){ 
		Physics ->SetPhysicsPackage(newValue);
	}
	else if (command == ComptonScatteringCmd){
		Physics -> SetComptonScattering(ComptonScatteringCmd -> GetNewBoolValue(newValue));
	}
	else if (command == RayleighScatteringCmd){
		Physics -> SetRayleighScattering(RayleighScatteringCmd -> GetNewBoolValue(newValue));
	}
	else if (command == PhotoElectricCmd){
		Physics -> SetPhotoElectric(PhotoElectricCmd -> GetNewBoolValue(newValue));
	}
	else if (command == FluorescenceCmd){
		Physics -> SetFluorescence(FluorescenceCmd -> GetNewBoolValue(newValue));
	}
	else if (command == GammaRefractionCmd){
	    Physics -> SetRefraction(GammaRefractionCmd -> GetNewBoolValue(newValue));
	}    
	else if (command == GammaAbsorptionCmd){
	    Physics -> SetGammaAbsorption(GammaAbsorptionCmd -> GetNewBoolValue(newValue));
	}    
}
