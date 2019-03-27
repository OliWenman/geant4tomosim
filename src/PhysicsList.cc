#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Register physics processes
#include "G4ProcessManager.hh"

//Standard Gamma processes
#include "G4ComptonScattering.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

//Livermore Gamma processes
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermoreRayleighModel.hh"

//Fluorescence
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"

//Gamma optical physics
#include "GammaOpticalRefraction.hh"
#include "GammaOpticalAbsorption.hh"

//Particle definations
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4GenericIon.hh"
#include "G4Proton.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4OpticalPhoton.hh"

//Other
#include "G4SystemOfUnits.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4RegionStore.hh"

#include "PrintLines.hh"

#include "G4LivermorePolarizedPhotoElectricModel.hh"

#include "G4RunManager.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList(), photoelectriceffect(0), livpol_photoeletriceffect(0),
                                                      comptonscattering(0), liv_comptonscattering(0),
                                                      rayleighscattering(0), liv_rayleighscattering(0),
                                                      de(0), 
                                                      gamma_refraction(0), 
                                                      gamma_absorption(0)
{
	//Creates the messenger class
  	physicsMessenger = new PhysicsListMessenger(this);

	//Sets the cutvalues
	//cutForGamma = 10*mm;
	//cutForElectron = 1*um;
	
	photoelectricOn = true;
	comptonscatteringOn = true;
	rayleighscatteringOn = true;
	fluorescenceOn = true;
	refractionOn = true;
	gamma_absorptionOn = false;
}

PhysicsList::~PhysicsList()
{
  	delete physicsMessenger; physicsMessenger = 0;
  	
  	delete photoelectriceffect; photoelectriceffect = 0;
  	delete livpol_photoeletriceffect; livpol_photoeletriceffect = 0;
  	
  	delete comptonscattering; comptonscattering = 0;
  	delete liv_comptonscattering; liv_comptonscattering = 0;
  	
  	delete rayleighscattering; rayleighscattering = 0;
  	delete liv_rayleighscattering; liv_rayleighscattering = 0;
  	
  	delete gamma_refraction; gamma_refraction = 0;
  	delete gamma_absorption; gamma_absorption = 0;
  	
  	//delete de;
}
void PhysicsList::ConstructParticle()
{
	//Define the particles you are going to be using
	G4Gamma::GammaDefinition();
	//G4Gamma gamma; gamma.ConstructParticle();
	
	G4Electron::ElectronDefinition();
	//G4Electron electron; electron.ContructParticle();
	
	G4Positron::PositronDefinition();
	//G4Positron positron; positron.ConstructParticle();
	G4GenericIon::GenericIonDefinition();
    //G4GenericIon ion; ion.ConstructParticle();

	G4Proton::ProtonDefinition();
	//G4Proton proton; proton.ConstructParticle();
	G4MuonPlus::MuonPlusDefinition();
  	//G4MuonPlus muonplus; muonplus.ConstructParticle();
  	G4MuonMinus::MuonMinusDefinition();
  	//G4MuonMinus muonminus.ConstructParticle();
  	
  	G4OpticalPhoton::OpticalPhotonDefinition();
    //G4OpticalPhoton photon; photon.ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
	//Add the transportation of the particles
	AddTransportation();
	//Register EM physics
	ConstructEM();
}
void PhysicsList::ConstructEM()
{
  	auto particleIterator = GetParticleIterator();
  	particleIterator->reset();

	//Cycles through the different particles	
  	while( (*particleIterator)() )
	{
		//Finds the physics processes for each particle 
  	  	G4ParticleDefinition* particle = particleIterator->value();
    	G4ProcessManager* pmanager = particle->GetProcessManager();
		pmanager -> SetVerboseLevel(0);
    	G4String particleName = particle->GetParticleName();
		
		//Adds the physics that were turned on via the commands for the gamma particle
		if(particleName == "gamma")
		{   
		    //Setup the photoelectric effect
		    if (!photoelectriceffect)
		    {
		        photoelectriceffect = new G4PhotoElectricEffect();
			    livpol_photoeletriceffect = new G4LivermorePolarizedPhotoElectricModel();
			    photoelectriceffect->SetEmModel(livpol_photoeletriceffect);
			    pmanager->AddDiscreteProcess(photoelectriceffect);
		    }
		
			/*G4LivermorePhotoElectricModel* livPhotoElectricEffect = new G4LivermorePhotoElectricModel();
			//livPhotoElectricEffect->SetVerboseLevel(0);
			photoelectriceffect->SetVerboseLevel(0);
			photoelectriceffect->SetEmModel(livPhotoElectricEffect);
			pmanager->AddDiscreteProcess(photoelectriceffect); */

            //Setup compton scattering
            if (!comptonscattering)
            {
			    comptonscattering = new G4ComptonScattering();
			    liv_comptonscattering = new G4LivermoreComptonModel();
			    comptonscattering->SetEmModel(liv_comptonscattering);
			    pmanager->AddDiscreteProcess(comptonscattering); 
			}
			
			//Setup the rayleigh scattering
			if (!rayleighscattering)
			{
			    rayleighscattering = new G4RayleighScattering();
			    liv_rayleighscattering = new G4LivermoreRayleighModel();
			    rayleighscattering->SetEmModel(liv_rayleighscattering);
			    pmanager->AddDiscreteProcess(rayleighscattering); 
		    }
		    
			//Setup fluorescence
			if (!de)
			{
			    de = new G4UAtomicDeexcitation();
			    de -> SetVerboseLevel(0);
  			    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
            }
            
            //Setup gamma refraction
            if (!gamma_refraction)
            {
                gamma_refraction = new GammaOpticalRefraction();
		        pmanager -> AddDiscreteProcess(gamma_refraction);
            }
		    if(!gamma_absorption)
		    {
		        gamma_absorption = new GammaOpticalAbsorption();
		        pmanager -> AddDiscreteProcess(gamma_absorption);
		    }
		    /*fRayleighScatteringProcess = new G4OpRayleigh();
            fMieHGScatteringProcess = new G4OpMieHG();
            pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
            pmanager->AddDiscreteProcess(fMieHGScatteringProcess);*/
		 }
	    //G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);
	}

	G4cout << G4endl;
}

void PhysicsList::ActivateUserPhysics()
{   
    G4ProcessManager* gammaProcessManager = G4Gamma::GammaDefinition()->GetProcessManager();
    
    G4ProcessVector* processlist = gammaProcessManager->GetProcessList();
    
    //Add photoelectric effect to processes if it doesn't already contain it and user wants it on
    if (photoelectricOn && !processlist->contains(photoelectriceffect)) {
        gammaProcessManager->AddProcess(photoelectriceffect);
    }
    else if (!photoelectricOn) {   
        gammaProcessManager->RemoveProcess(photoelectriceffect);    
    }
   
    //Add compton scattering to processes if it doesn't already contain it and user wants it on
    if (comptonscatteringOn && !processlist->contains(comptonscattering)) {
        gammaProcessManager->AddProcess(comptonscattering);
    }
    else if (!comptonscatteringOn) {
        gammaProcessManager->RemoveProcess(comptonscattering);
    }
    
    //Add rayleigh scattering to processes if it doesn't already contain it and user wants it on
    if (rayleighscatteringOn && !processlist->contains(rayleighscattering)) {
        gammaProcessManager->AddProcess(rayleighscattering);
    }
    else if (!rayleighscatteringOn) {
        gammaProcessManager->RemoveProcess(rayleighscattering);
    }
    
    //Add gamma refraction to processes if it doesn't already contain it and user wants it on  
    if (refractionOn && !processlist->contains(gamma_refraction)) {
        gammaProcessManager->AddProcess(gamma_refraction);
    }
    else if (!refractionOn) {
        gammaProcessManager->RemoveProcess(gamma_refraction);
    }
    
    //Add gamma absoprtion to processes if it doesn't already contain it and user wants it on  
    if (gamma_absorptionOn && !processlist->contains(gamma_absorption)) {
        gammaProcessManager->AddProcess(gamma_absorption);
    }
    else if (gamma_absorptionOn) {
        gammaProcessManager->RemoveProcess(gamma_absorption);
    }
    
    if (fluorescenceOn)
    {
        de->SetFluo(true);
  		de->SetAuger(true);   
  		de->SetPIXE(true);  
    }
    else 
    {  
        de->SetFluo(false);
  		de->SetAuger(false);   
  		de->SetPIXE(false);
    }
    
    G4cout << "\nBUG: Fluorescence can only be changed once?" << G4endl;
}

void PhysicsList::SetCuts()
{	
    //SetCutValue(5*mm, "gamma");
    //SetCutValue(1000*mm, "e-");
    //SetCutValue(1000*mm, "e+");
    //SetCutValue(1000*mm, "proton");
    
    //DumpCutValuesTable();
}

void PhysicsList::ReadOutInfo(SettingsLog& log)
{
    PrintToEndOfTerminal(log, '-');
    log << "PHYSICS PROCESSES: "; 
	    //<< "\n- " << PhysicsPackageCmd << ":";

	if (photoelectricOn)      {log << "\n- Photoelectric effect (livermore)";}
	if (comptonscatteringOn)  {log << "\n- Compton scattering (livermore)";}
	if (rayleighscatteringOn) {log << "\n- Rayleigh scattreing (livermore)";}
	if (fluorescenceOn)       {log << "\n- Fluorescence";}
	if (refractionOn)         {log << "\n- Gamma refraction";}
	if (gamma_absorptionOn)   {log << "\n- Gamma absorption";}
}

