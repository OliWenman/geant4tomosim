#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4ProcessManager.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* PLObject):physics(PLObject)
{
  	physicsDirectory = new G4UIdirectory("/physics/");
  	physicsDirectory -> SetGuidance("Commands to activate physics models and set cuts");

	gamma_liv_photoelectricCmd = new G4UIcmdWithABool("/physics/gamma/livermore/photoelectric",this);  
  	gamma_liv_photoelectricCmd -> SetGuidance("Choose if you want gamma particles to have the photoelectric effect \n" 
  	                                "true/false");

	gamma_liv_comptonscatteringCmd = new G4UIcmdWithABool("/physics/gamma/livermore/comptomscattering",this);  
  	gamma_liv_comptonscatteringCmd -> SetGuidance("Choose if you want gamma particles to have compton scattering \n" 
  	                                    "<true/false>");
	
	gamma_liv_rayleighscatteringCmd = new G4UIcmdWithABool("/physics/gamma/livermore/rayleighscattering",this);  
  	gamma_liv_rayleighscatteringCmd -> SetGuidance("Choose if you want gamma particles to have rayleigh scattering physics \n"
  	                                     "<true/false>");

	gamma_fluorescenceCmd = new G4UIcmdWithABool("/physics/gamma/fluorescence",this);  
  	gamma_fluorescenceCmd -> SetGuidance("Choose if you want gamma particles to have fluorescence physics \n"
  	                               "<true/false>");
  	
  	gamma_refractionCmd = new G4UIcmdWithABool("/physics/gamma/refraction", this);
  	gamma_refractionCmd -> SetGuidance("Choose if you want gamma particles to have refraction physics \n"
  	                                  "<true/false>");
  	
  	optical_absorptionCmd = new G4UIcmdWithABool("/physics/opticalphoton/absorption", this);
  	optical_absorptionCmd -> SetGuidance("Choose if you want opticalphoton particles to have absorption physics \n"
  	                                     "<true/false>");
  	
  	optical_refractionCmd = new G4UIcmdWithABool("/physics/opticalphoton/refraction", this);
  	optical_refractionCmd -> SetGuidance("Choose if you want opticalphoton's particles to have refraction physics \n"
  	                                     "<true/false>");
  	
    gammacuts = new G4UIcmdWithADoubleAndUnit("/physics/gamma/cuts", this);
    gammacuts -> SetUnitCategory("Length");
    gammacuts -> SetParameterName("length",true,true);
    gammacuts -> SetRange("length > 0");
    gammacuts -> SetGuidance("Set the minimum cut length for the gamma particle \n"
                             "<length> <unit>");

    opticalcuts = new G4UIcmdWithADoubleAndUnit("/physics/opticalphoton/cuts", this);
    opticalcuts -> SetUnitCategory("Length");
    opticalcuts -> SetParameterName("length",true,true);
    opticalcuts -> SetRange("length > 0");
    opticalcuts -> SetGuidance("Set the minimum cut length for the opticalphoton particle \n"
                               "<length> <unit>");

    electroncuts = new G4UIcmdWithADoubleAndUnit("/physics/electron/cuts", this);
    electroncuts -> SetUnitCategory("Length");
    electroncuts -> SetParameterName("length",true,true);
    electroncuts -> SetRange("length > 0");
    electroncuts -> SetGuidance("Set the minimum cut length for the electron particle \n"
                                "<length> <unit>");
}

PhysicsListMessenger::~PhysicsListMessenger()
{
    delete physicsDirectory;   
  	
	delete gamma_liv_photoelectricCmd;
	delete gamma_liv_comptonscatteringCmd;
	delete gamma_liv_rayleighscatteringCmd;
	delete gamma_fluorescenceCmd;
  	delete gamma_refractionCmd;
  	
  	delete optical_absorptionCmd;
  	delete optical_refractionCmd;
  	
  	delete gammacuts;
  	delete opticalcuts;
  	delete electroncuts;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{       
	if (command == gamma_liv_comptonscatteringCmd){
		physics -> SetComptonScattering(gamma_liv_comptonscatteringCmd -> GetNewBoolValue(newValue));
	}
	else if (command == gamma_liv_rayleighscatteringCmd){
		physics -> SetRayleighScattering(gamma_liv_rayleighscatteringCmd -> GetNewBoolValue(newValue));
	}
	else if (command == gamma_liv_photoelectricCmd){
		physics -> SetPhotoElectric(gamma_liv_photoelectricCmd -> GetNewBoolValue(newValue));
	}
	else if (command == gamma_fluorescenceCmd){
		physics -> SetFluorescence(gamma_fluorescenceCmd -> GetNewBoolValue(newValue));
	}
	else if (command == gamma_refractionCmd){
	    physics -> SetRefraction(gamma_refractionCmd -> GetNewBoolValue(newValue));
	}    
	else if (command == optical_absorptionCmd){
	    physics -> SetOpticalAbsorption(optical_absorptionCmd->GetNewBoolValue(newValue));
	}
	else if (command == optical_refractionCmd){
	    physics -> SetOpticalRefraction(optical_refractionCmd->GetNewBoolValue(newValue));
	}
	else if (command == gammacuts)
	{
	    gammacuts->GetNewUnitValue(newValue);
	    physics -> SetGammaCuts(gammacuts->GetNewDoubleValue(newValue));
	}
	else if (command == opticalcuts)
	{
	    opticalcuts->GetNewUnitValue(newValue);
	    physics -> SetOpticalCuts(opticalcuts->GetNewDoubleValue(newValue));
	}
	else if (command == electroncuts)
	{
	    electroncuts->GetNewUnitValue(newValue);
	    physics -> SetElectronCuts(electroncuts->GetNewDoubleValue(newValue));
	}
}
