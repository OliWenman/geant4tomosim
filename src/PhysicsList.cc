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

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
	//Creates the messenger class
  	PhysicsMessenger = new PhysicsListMessenger(this);

	//Sets the cutvalues
	cutForGamma = 10*mm;
	cutForElectron = 1*um;
	
	PhotoElectricCmd = true;
	ComptonScatteringCmd = true;
	RayleighScatteringCmd = true;
	FluorescenceCmd = true;
	RefractionCmd = true;
	GammaAbsorption = false;
}

PhysicsList::~PhysicsList()
{
  	delete PhysicsMessenger;
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
}
void PhysicsList::ConstructEM()
{
  	auto particleIterator = GetParticleIterator();
  	particleIterator->reset();

	//Cycles through he different particles	
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
			if (PhotoElectricCmd)
			{
				G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
				G4LivermorePhotoElectricModel* livPhotoElectricEffect = new G4LivermorePhotoElectricModel();
				//livPhotoElectricEffect->SetVerboseLevel(0);
				thePhotoElectricEffect->SetVerboseLevel(0);
				thePhotoElectricEffect->SetEmModel(livPhotoElectricEffect);
				pmanager->AddDiscreteProcess(thePhotoElectricEffect); 
			}
			if (ComptonScatteringCmd)
			{
				G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
				G4LivermoreComptonModel* livComptonScattering = new G4LivermoreComptonModel();
				theComptonScattering->SetEmModel(livComptonScattering);
				pmanager->AddDiscreteProcess(theComptonScattering); 
			}
			if (RayleighScatteringCmd)
			{
				G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
				theRayleighScattering->SetEmModel(new G4LivermoreRayleighModel());
				pmanager->AddDiscreteProcess(theRayleighScattering); 
			}
			/*else if(PhysicsPackageCmd == "StandardPhysics")
			{
				if (PhotoElectricCmd)
				{
					G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
					pmanager->AddDiscreteProcess(thePhotoElectricEffect); 
				}
				if (ComptonScatteringCmd)
				{
					G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
					pmanager->AddDiscreteProcess(theComptonScattering); 
				}
				if (RayleighScatteringCmd)
				{
					G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
					pmanager->AddDiscreteProcess(theRayleighScattering); 
				}
			}*/
			/*else
			{
				//Outputs an error saying if the physics inputted is not a valid input and stops the simulation
				G4cout << "\n================================================================================"
	       	       	   << "\n            ERROR - INVALID PHYSICS INPUT: " << PhysicsPackageCmd << G4endl
		               << "\n            Refer to the README for list of available Physics inputs "
	                   << "\n================================================================================" << G4endl;
				exit(-1);
			}*/
			if (FluorescenceCmd)
			{
				//Relaxtion processes after the photoelctric effect
				G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
				de -> SetVerboseLevel(0);
  				de->SetFluo(true);
  				de->SetAuger(true);   
  				de->SetPIXE(true);  
  				G4LossTableManager::Instance()->SetVerbose(0);
  				G4LossTableManager::Instance()->SetAtomDeexcitation(de);
			}
		    if (RefractionCmd)
		    {
		        GammaOpticalRefraction* photonRefraction = new GammaOpticalRefraction();
		        pmanager -> AddDiscreteProcess(photonRefraction);
		    }
		    if(GammaAbsorption)
		    {
		        GammaOpticalAbsorption* photonAbsorption = new GammaOpticalAbsorption();
		        pmanager -> AddDiscreteProcess(photonAbsorption);
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
    log << "PHYSICS PROCESSES" 
	    << "\n- " << PhysicsPackageCmd << ":";

	for (int element = 0 ; element < PhysicProcesses.size() ; element++){
		log << "\n" << PhysicProcesses[element];
	}	
}

