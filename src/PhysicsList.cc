#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
#include "Data.hh"

#include "G4SystemOfUnits.hh"
//#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

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

#include "G4EmStandardPhysicsGS.hh"


#include "G4PhotoElectricEffect.hh"
#include "G4Gamma.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4PhysicsListHelper.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

#include "G4eMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

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
  	delete emPhysicsList;
  	
	G4cout << G4endl << "PhysicsList has been deleted" << G4endl;
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
	if (GetPhysicsUsed() == "EmPhotoElectricEffect") 
		{ConstructEM();}
	else
		{emPhysicsList->ConstructProcess();}	
}

void PhysicsList::ConstructEM()
{
  	G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
  	auto particleIterator=GetParticleIterator();
  	particleIterator->reset();
  	while( (*particleIterator)() )
	{
  	  	G4ParticleDefinition* particle = particleIterator->value();
 	   	G4String particleName = particle->GetParticleName();
     
   		if (particleName == "gamma") 
			{ph->RegisterProcess(new G4PhotoElectricEffect, particle);}
      			//ph->RegisterProcess(new G4ComptonScattering,   particle);
      			//ph->RegisterProcess(new G4GammaConversion,     particle);
      			//ph->RegisterProcess(new G4RayleighScattering, particle);
      
  	}
}


void PhysicsList::AddPhysicsList(G4String& name)
{
  	if (name == "EMStandardPhysics")
	{
		Print(name);
		data -> SetPhysicsUsed(name);
	}
	else if (name == "EMStandardPhysics_option1") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option1();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EMStandardPhysics_option2") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option2();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
  	else if (name == "EMStandardPhysics_option3") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option3();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmStandardPhysics_option4") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysics_option4();
		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmLivermorePhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLivermorePhysics();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmLivermorePolarizedPhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLivermorePolarizedPhysics();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmLowEPPhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmLowEPPhysics();
    		Print(name);
		data -> SetPhysicsUsed(name);
 	} 
	else if (name == "EmPenelopePhysics") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmPenelopePhysics();
    		Print(name);
		data -> SetPhysicsUsed(name);
	}
	else if (name == "EmStandardPhysicsGS") 
	{
    		delete emPhysicsList;
    		emPhysicsList = new G4EmStandardPhysicsGS();
    		Print(name);
		data -> SetPhysicsUsed(name);
	}
	else if (name == "EmPhotoElectricEffect") 
	{
    		delete emPhysicsList;
    		Print(name);
		data -> SetPhysicsUsed(name);
	}
	else 
	{	
		delete emPhysicsList;
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


