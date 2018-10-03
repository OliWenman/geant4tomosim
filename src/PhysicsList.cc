#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
#include "Data.hh"

#include "G4SystemOfUnits.hh"
//#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicsListHelper.hh"

//Physic lists (contained inside the Geant4 distribution)
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmLivermorePolarizedPhysics.hh"

#include "G4EmLowEPPhysics.hh"

#include "G4EmPenelopePhysics.hh"

#include "G4Gamma.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

#include "G4eMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermoreRayleighModel.hh"

PhysicsList::PhysicsList(Data* DataObject) : G4VModularPhysicsList(), data(DataObject)
{
  	PhysicsMessenger = new PhysicsListMessenger(this);

  	//EM physics
  	emPhysicsList = new G4EmStandardPhysics();

	G4double Cutvalue = 1*mm;
}

PhysicsList::~PhysicsList()
{
  	delete PhysicsMessenger;
	//if (GetPhysicsUsed() != LMPhotoElectricEffect || GetPhysicsUsed() != LivermoreGamma )
	delete emPhysicsList;
  	
	G4cout << G4endl << "PhysicsList has been deleted";
}

void PhysicsList::ConstructParticle()
{
	G4Gamma::GammaDefinition();
	G4Electron::ElectronDefinition();
  	G4Positron::PositronDefinition();
	G4GenericIon::GenericIonDefinition();

	emPhysicsList->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
  	//Transportation
  	AddTransportation();

  	//Electromagnetic physics list
	if (GetPhysicsUsed() == LMPhotoElectricEffect || GetPhysicsUsed() == LivermoreGamma) 
		{ConstructEM(GetPhysicsUsed());}
	else
		{emPhysicsList->ConstructProcess();}

}

void PhysicsList::ConstructEM(G4String Physics)
{
	auto particleIterator = GetParticleIterator();
  	particleIterator->reset();
	
  	while( (*particleIterator)() )
	{
  	  	G4ParticleDefinition* particle = particleIterator->value();
 	   	G4String particleName = particle->GetParticleName();
		G4ProcessManager* pmanager = particle->GetProcessManager(); 
     
   		if (particleName == "gamma") 
		{	
			if (Physics == LMPhotoElectricEffect)
			{
				G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
				thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel());
				//thePhotoElectricEffect->SetHighEnergy(1.*GeV);
				pmanager->AddDiscreteProcess(thePhotoElectricEffect); 
			}
			else if (Physics == LivermoreGamma)
			{
				G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
				theComptonScattering->SetEmModel(new G4LivermoreComptonModel());
				//thePhotoElectricEffect->SetHighEnergy(1.*GeV);
				pmanager->AddDiscreteProcess(theComptonScattering); 

				G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
				theRayleighScattering->SetEmModel(new G4LivermoreRayleighModel());
				//thePhotoElectricEffect->SetHighEnergy(1.*GeV);
				pmanager->AddDiscreteProcess(theRayleighScattering); 
			}
  		}
	}
}


void PhysicsList::AddPhysicsList(G4String& name)
{
	G4String EM = "Em";
  	if (name == "StandardPhysics")
	{
		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
	}
	else if (name == "StandardPhysics_option1") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option1();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "StandardPhysics_option2") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option2();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
  	else if (name == "StandardPhysics_option3") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option3();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "StandardPhysics_option4") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option4();
		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "LivermorePhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLivermorePhysics();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "LivermorePolarizedPhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLivermorePolarizedPhysics();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "LowEPPhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLowEPPhysics();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
 	} 
	else if (name == "PenelopePhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmPenelopePhysics();
    		Print(EM+name);
		data -> SetPhysicsUsed(EM+name);
	}
	else if (name == LMPhotoElectricEffect) 
	{
    		emPhysicsList = 0;
    		Print(name);
		data -> SetPhysicsUsed(name);
	}
	else if (name == LivermoreGamma) 
	{
    		emPhysicsList = 0;
    		Print(name);
		data -> SetPhysicsUsed(name);
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
{
	SetCutValue(Cutvalue, "gamma");
  	SetCutValue(Cutvalue, "e-");
  	SetCutValue(Cutvalue, "e+");
}

void PhysicsList::Print(G4String name)
{
	G4cout << G4endl << "================================================================================"
	       << G4endl << "        THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED" << G4endl
	       << G4endl << "                          " << name 
	       << G4endl << "================================================================================" << G4endl;

}

//--------------------------------------------------------------------------


