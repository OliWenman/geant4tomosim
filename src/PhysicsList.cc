#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
#include "Input.hh"

#include "G4SystemOfUnits.hh"
//#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4LossTableManager.hh"
#include "G4PhysicsListHelper.hh"

//Physic lists (contained inside the Geant4 distribution)
#include "G4EmStandardPhysics.hh"

#include "G4EmLivermorePhysics.hh"

#include "G4Gamma.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4UAtomicDeexcitation.hh"

#include "G4eMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4LivermoreIonisationModel.hh"

PhysicsList::PhysicsList(Input* InputObject) : G4VModularPhysicsList(), input(InputObject)
{
  	PhysicsMessenger = new PhysicsListMessenger(this);

	G4double Cutvalue = 1*mm;
}

PhysicsList::~PhysicsList()
{
  	delete PhysicsMessenger;
	G4cout << G4endl << "PhysicsList has been deleted";
}
void PhysicsList::ConstructParticle()
{
	G4Gamma::GammaDefinition();
	G4Electron::ElectronDefinition();
	G4Positron::PositronDefinition();
	G4GenericIon::GenericIonDefinition();
}

void PhysicsList::ConstructProcess()
{
	AddTransportation();
	ConstructEM();
}
void PhysicsList::ConstructEM()
{
  	auto particleIterator = GetParticleIterator();
  	particleIterator->reset();
	
  	while( (*particleIterator)() )
	{
  	  	G4ParticleDefinition* particle = particleIterator->value();
    		G4ProcessManager* pmanager = particle->GetProcessManager();
    		G4String particleName = particle->GetParticleName();
     
		if(particleName == "gamma")
		{
			if(PhysicsPackageCmd == "LivermorePhysics")
			{
				G4cout << G4endl << "LIVERMORE PHYSICS: ";

				if (PhotoElectricCmd == true)
				{
					G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
					thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel());
					pmanager->AddDiscreteProcess(thePhotoElectricEffect); 
					G4cout << G4endl << " -PhotoElectric Effect";
				}
				if (ComptonScatteringCmd == true)
				{
					G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
					theComptonScattering->SetEmModel(new G4LivermoreComptonModel());
					pmanager->AddDiscreteProcess(theComptonScattering); 
					G4cout << G4endl << "- Compton Scattering";
				}
				if (RayleighScatteringCmd == true)
				{
					G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
					theRayleighScattering->SetEmModel(new G4LivermoreRayleighModel());
					pmanager->AddDiscreteProcess(theRayleighScattering); 
					G4cout << G4endl << "- Rayleigh Scattering";
				}
			}
			else if(PhysicsPackageCmd == "StandardPhysics")
			{
				G4cout << G4endl << "STANDARD PHYSICS: ";

				if (PhotoElectricCmd == true)
				{
					G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
					pmanager->AddDiscreteProcess(thePhotoElectricEffect); 
					G4cout << G4endl << "- PhotoElectricEffect";
				}
				if (ComptonScatteringCmd == true)
				{
					G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
					pmanager->AddDiscreteProcess(theComptonScattering); 
					G4cout << G4endl << "- ComptonScattering";
				}
				if (RayleighScatteringCmd == true)
				{
					G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
					pmanager->AddDiscreteProcess(theRayleighScattering); 
					G4cout << G4endl << "- RayleighScattering";
				}

			}
			else
			{
				G4cout << G4endl << "================================================================================"
	       	       		       << G4endl << "            ERROR - INVALID PHYSICS INPUT: " << PhysicsPackageCmd << G4endl
		                       << G4endl << "            Refer to the README for list of available Physics inputs "
	                               << G4endl << "================================================================================" << G4endl;
				exit(-1);
			}
			if (FluorescenceCmd == true)
			{
				G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  				de->SetFluo(true);
  				de->SetAuger(false);   
  				de->SetPIXE(false);  
  				G4LossTableManager::Instance()->SetAtomDeexcitation(de);
				G4cout << G4endl << "- Fluorescence";
			}

		}
	}

	G4cout << G4endl;
}


void PhysicsList::AddPhysicsList(G4String& name)
{
	G4String EM = "Em";
  	if (name == "StandardPhysics")
	{
		Print(EM+name);
		input -> SetPhysicsUsed(EM+name);
	}
	else if (name == "LivermorePhysics") 
	{
    		Print(EM+name);
		input -> SetPhysicsUsed(EM+name);
 	} 
	else 
	{	
		emPhysicsList = 0;
		G4cout << G4endl << "================================================================================"
	       	       << G4endl << "            ERROR - INVALID PHYSICS INPUT: " << name << G4endl
		       << G4endl << "            Refer to the README for list of available Physics inputs "
	               << G4endl << "================================================================================" << G4endl;
	}
}

void PhysicsList::SetCuts(G4double Cutvalue)
	{SetCutValue(Cutvalue, "gamma");}

void PhysicsList::Print(G4String name)
{
	G4cout << G4endl << "================================================================================"
	       << G4endl << "        THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED" << G4endl
	       << G4endl << "                          " << name 
	       << G4endl << "================================================================================" << G4endl;

}

