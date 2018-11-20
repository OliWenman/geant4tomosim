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
  	physicsDirectory->SetGuidance("Commands to activate physics models and set cuts");

  	PhysicsPackageCmd = new G4UIcmdWithAString("/physics/addPhysics/EM",this);  
  	PhysicsPackageCmd->SetGuidance("Add physics list.");
  	//PhysicsPackageCmd->SetParameterName("PList",false);
  	PhysicsPackageCmd->AvailableForStates(G4State_PreInit);  

	PhotoElectricCmd = new G4UIcmdWithABool("/physics/EM/PhotoElectricEffect",this);  
  	PhotoElectricCmd->SetGuidance("Add physics list.");
  	//PhotoElectricCmd->SetParameterName("PList",false);
  	PhotoElectricCmd->AvailableForStates(G4State_PreInit);  

	ComptonScatteringCmd = new G4UIcmdWithABool("/physics/EM/ComptonScattering",this);  
  	ComptonScatteringCmd->SetGuidance("Add physics list.");
  	//ComptonScatteringCmd->SetParameterName("PList",false);
  	ComptonScatteringCmd->AvailableForStates(G4State_PreInit);  
	
	RayleighScatteringCmd = new G4UIcmdWithABool("/physics/EM/RayleighScattering",this);  
  	RayleighScatteringCmd->SetGuidance("Add physics list.");
  	//RayleighScatteringCmd->SetParameterName("PList",false);
  	RayleighScatteringCmd->AvailableForStates(G4State_PreInit);  

	FluorescenceCmd = new G4UIcmdWithABool("/physics/EM/Fluorescence",this);  
  	FluorescenceCmd->SetGuidance("Add physics list.");
  	//RayleighScatteringCmd->SetParameterName("PList",false);
  	FluorescenceCmd->AvailableForStates(G4State_PreInit);  

	
}

PhysicsListMessenger::~PhysicsListMessenger()
{
  	delete PhysicsPackageCmd;
	delete PhotoElectricCmd;
	delete ComptonScatteringCmd;
	delete RayleighScatteringCmd;
	delete FluorescenceCmd;
  	delete physicsDirectory;   
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{       
  		if( command == PhysicsPackageCmd )
   		{ 
			Physics ->SetPhysicsPackage(newValue);
		}
		else if (command == ComptonScatteringCmd)
		{
			Physics -> SetComptonScattering(ComptonScatteringCmd -> GetNewBoolValue(newValue));
		}
		else if (command == RayleighScatteringCmd)
		{
			Physics -> SetRayleighScattering(RayleighScatteringCmd -> GetNewBoolValue(newValue));
		}
		else if (command == PhotoElectricCmd)
		{
			Physics -> SetPhotoElectric(PhotoElectricCmd -> GetNewBoolValue(newValue));
		}
		else if (command == FluorescenceCmd)
		{
			Physics -> SetFluorescence(FluorescenceCmd -> GetNewBoolValue(newValue));
		}
}
