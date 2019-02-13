#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4ProcessManager.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* PLObject):Physics(PLObject)
{
  	physicsDirectory = new G4UIdirectory("/physics/");
  	physicsDirectory -> SetGuidance("Commands to activate physics models and set cuts");

  	PhysicsPackageCmd = new G4UIcmdWithAString("/physics/addPhysics/EM",this);  
  	PhysicsPackageCmd -> SetGuidance("Choose the type of physics package to use"); 

	PhotoElectricCmd = new G4UIcmdWithABool("/physics/EM/PhotoElectricEffect",this);  
  	PhotoElectricCmd -> SetGuidance("Choose if you want the photoelectric effect true/false");

	ComptonScatteringCmd = new G4UIcmdWithABool("/physics/EM/ComptonScattering",this);  
  	ComptonScatteringCmd -> SetGuidance("Choose if you want compton scattering true/false");
	
	RayleighScatteringCmd = new G4UIcmdWithABool("/physics/EM/RayleighScattering",this);  
  	RayleighScatteringCmd -> SetGuidance("Choose if you want rayliegh scattering true/false");

	FluorescenceCmd = new G4UIcmdWithABool("/physics/EM/Fluorescence",this);  
  	FluorescenceCmd -> SetGuidance("Choose if you want fluorescence true/false");
  	
  	RefractionCmd = new G4UIcmdWithABool("/physics/EM/Refraction", this);
  	RefractionCmd -> SetGuidance("Choose if you want refraction true/false");
}

PhysicsListMessenger::~PhysicsListMessenger()
{
    delete physicsDirectory;   
  	delete PhysicsPackageCmd;
  	
	delete PhotoElectricCmd;
	delete ComptonScatteringCmd;
	delete RayleighScatteringCmd;
	delete FluorescenceCmd;
  	delete RefractionCmd;
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
	else if (command == RefractionCmd){
	    Physics -> SetRefraction(RefractionCmd -> GetNewBoolValue(newValue));
	}    
	    
}
