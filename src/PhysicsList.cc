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

//Physics for opticalphotons
#include "G4OpBoundaryProcess.hh"
#include "G4OpAbsorption.hh"


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

#include "G4eIonisation.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList(), photoelectriceffect(0), liv_photoelectric(0)/*livpol_photoeletriceffect(0)*/,
                                                      comptonscattering(0), liv_comptonscattering(0),
                                                      rayleighscattering(0), liv_rayleighscattering(0),
                                                      gamma_refraction(0), 
                                                      opticalphoton_absorption(0), opticalphoton_refraction(0)
{
	//Creates the messenger class
  	physicsMessenger = new PhysicsListMessenger(this);

	//Sets the cutvalues
	cutForGamma    = 1*mm;
	cutForElectron = 1*mm;
	cutForOptical  = 1*mm;
	
	photoelectricOn      = true;
	comptonscatteringOn  = true;
	rayleighscatteringOn = true;
	fluorescenceOn       = true;
	gamma_refractionOn         = true;
	
	optical_refractionOn = false;
	optical_absorptionOn = false;
	
	setup = false;
}

PhysicsList::~PhysicsList()
{
  	delete physicsMessenger; physicsMessenger = 0;
  	
  	delete photoelectriceffect; photoelectriceffect = 0;
  	delete liv_photoelectric;   liv_photoelectric = 0;
  	//delete livpol_photoeletriceffect; livpol_photoeletriceffect = 0;
  	
  	delete comptonscattering; comptonscattering = 0;
  	delete liv_comptonscattering; liv_comptonscattering = 0;
  	
  	delete rayleighscattering; rayleighscattering = 0;
  	delete liv_rayleighscattering; liv_rayleighscattering = 0;
  	
  	delete gamma_refraction; gamma_refraction = 0;
  	
  	delete opticalphoton_absorption; opticalphoton_absorption = 0;
  	delete opticalphoton_refraction; opticalphoton_refraction = 0;
}
void PhysicsList::ConstructParticle()
{
	//Define the particles you are going to be using
	G4Gamma::GammaDefinition();
	G4Electron::ElectronDefinition();
	G4Positron::PositronDefinition();
	
	G4GenericIon::GenericIonDefinition();

	G4Proton::ProtonDefinition();
	G4MuonPlus::MuonPlusDefinition();
  	G4MuonMinus::MuonMinusDefinition();
  	
  	G4OpticalPhoton::OpticalPhotonDefinition();
}

void PhysicsList::ConstructProcess()
{
	//Add the transportation of the particles
	AddTransportation();
	//Register EM physics
	ConstructEM();
	//AddStepMax();
}
void PhysicsList::ConstructEM()
{
  	auto particleIterator = GetParticleIterator();
  	particleIterator->reset();
  	
  	setup = false;

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
		        photoelectriceffect->SetMaxKinEnergy(175*keV);
			    liv_photoelectric = new G4LivermorePhotoElectricModel();
			    photoelectriceffect->SetEmModel(liv_photoelectric);
			    pmanager->AddDiscreteProcess(photoelectriceffect);
		    }

            //Setup compton scattering
            if (!comptonscattering)
            {
			    comptonscattering = new G4ComptonScattering();
			    comptonscattering->SetMaxKinEnergy(175*keV);
			    liv_comptonscattering = new G4LivermoreComptonModel();
			    comptonscattering->SetEmModel(liv_comptonscattering);
			    pmanager->AddDiscreteProcess(comptonscattering); 
			}
			
			//Setup the rayleigh scattering
			if (!rayleighscattering)
			{
			    rayleighscattering = new G4RayleighScattering();
			    rayleighscattering->SetMaxKinEnergy(175*keV);
			    liv_rayleighscattering = new G4LivermoreRayleighModel();
			    rayleighscattering->SetEmModel(liv_rayleighscattering);
			    pmanager->AddDiscreteProcess(rayleighscattering); 
		    }
		    
			//Setup fluorescence
			if (!G4LossTableManager::Instance()->AtomDeexcitation())
			{
			    G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
			    de -> SetVerboseLevel(0);
  			    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
            }
            //Setup gamma refraction
            if (!gamma_refraction)
            {
                gamma_refraction = new GammaOpticalRefraction();
		        pmanager -> AddDiscreteProcess(gamma_refraction);
            }
		}
        else if (particleName == "opticalphoton")
        {
            if (!opticalphoton_absorption)
		    {
		        opticalphoton_absorption = new G4OpAbsorption();
		        pmanager -> AddDiscreteProcess(opticalphoton_absorption);
		    }
		    
		    if (!opticalphoton_refraction)
		    {
		        opticalphoton_refraction = new G4OpBoundaryProcess();
		        pmanager -> AddDiscreteProcess(opticalphoton_refraction);
		    }
		    /*fRayleighScatteringProcess = new G4OpRayleigh();
            fMieHGScatteringProcess = new G4OpMieHG();
            pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
            pmanager->AddDiscreteProcess(fMieHGScatteringProcess);*/
		}
		
	    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 175*keV);
	}
	
	SetCuts();
}

void PhysicsList::ActivateUserPhysics()
{   
    if (setup) {return;}

    G4ProcessManager* gamma_processmanager = G4Gamma::GammaDefinition()->GetProcessManager();    
    G4ProcessVector* gamma_processlist = gamma_processmanager->GetProcessList();
    
    //Add photoelectric effect to processes if it doesn't already contain it and user wants it on
    if (photoelectricOn && !gamma_processlist->contains(photoelectriceffect)) {
        gamma_processmanager->AddProcess(photoelectriceffect);
    }
    else if (!photoelectricOn) {   
        gamma_processmanager->RemoveProcess(photoelectriceffect);    
    }
   
    //Add compton scattering to processes if it doesn't already contain it and user wants it on
    if (comptonscatteringOn && !gamma_processlist->contains(comptonscattering)) {
        gamma_processmanager->AddProcess(comptonscattering);
    }
    else if (!comptonscatteringOn) {
        gamma_processmanager->RemoveProcess(comptonscattering);
    }
    
    //Add rayleigh scattering to processes if it doesn't already contain it and user wants it on
    if (rayleighscatteringOn && !gamma_processlist->contains(rayleighscattering)) {
        gamma_processmanager->AddProcess(rayleighscattering);
    }
    else if (!rayleighscatteringOn) {
        gamma_processmanager->RemoveProcess(rayleighscattering);
    }
    
    //Add gamma refraction to processes if it doesn't already contain it and user wants it on  
    if (gamma_refractionOn && !gamma_processlist->contains(gamma_refraction)) {
        gamma_processmanager->AddProcess(gamma_refraction);
    }
    else if (!gamma_refractionOn) {
        gamma_processmanager->RemoveProcess(gamma_refraction);
    }
    
    G4VAtomDeexcitation* atomDeexcitation = G4LossTableManager::Instance()->AtomDeexcitation();
    
    if(!atomDeexcitation)
    {
        G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
	    de -> SetVerboseLevel(0);
  	    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
    }
    
    if (fluorescenceOn)
    {   
        atomDeexcitation->SetFluo(true);
  		atomDeexcitation->SetAuger(true);   
  		atomDeexcitation->SetPIXE(true);  
    }
    else
    {  
        atomDeexcitation->SetFluo(false);
  		atomDeexcitation->SetAuger(false);   
  		atomDeexcitation->SetPIXE(false);  
    }

    //Optical process list
    G4ProcessManager* optical_processmanager = G4OpticalPhoton::OpticalPhotonDefinition()->GetProcessManager();    
    G4ProcessVector* optical_processlist = optical_processmanager->GetProcessList();
    
    if (optical_refractionOn && !optical_processlist->contains(opticalphoton_refraction))
    {
        optical_processmanager->AddProcess(opticalphoton_refraction);
    }
    else if (!optical_refractionOn)
    {
        optical_processmanager->RemoveProcess(opticalphoton_refraction);
    }
    if (optical_absorptionOn && !optical_processlist->contains(opticalphoton_absorption))
    {
        optical_processmanager->AddProcess(opticalphoton_absorption);
    }
    else if (!optical_absorptionOn)
    {
        optical_processmanager->RemoveProcess(opticalphoton_absorption);
    }

    setup = true;
}

void PhysicsList::SetCuts()
{	
    SetCutValue(cutForGamma, "gamma");
    SetCutValue(cutForElectron, "e-");
    SetCutValue(cutForOptical, "opticalphoton");
    //SetCutValue(1000*mm, "proton");
    
    //DumpCutValuesTable();
}

#include "StepMax.hh"

//Loop through each particle and add its max step for each process
void PhysicsList::AddStepMax()
{
    /*// Step limitation seen as a process
    fStepMaxProcess = new StepMax();

    auto particleIterator=GetParticleIterator();
    particleIterator->reset();
    while ((*particleIterator)())
    {
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();

        if (fStepMaxProcess->IsApplicable(*particle))
        {
            pmanager->AddDiscreteProcess(fStepMaxProcess);
        }
  }*/
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
	if (gamma_refractionOn)   {log << "\n- Gamma refraction";}
	if (optical_refractionOn) {log << "\n- OpticalPhoton - refraction";}
	if (optical_absorptionOn) {log << "\n- OpticalPhoton - absorption";}
}

