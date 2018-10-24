#include "RunAction.hh"
#include "RunActionMessenger.hh"
#include "Input.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"
#include "time.h"

RunAction::RunAction(Input* InputObject): G4UserRunAction(), input(InputObject)
{ 
	G4cout << G4endl << "RunAction has been created ";
	runMessenger = new RunActionMessenger(this);

	//Create the engine for the simulation
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	loop = 0;
}

RunAction::~RunAction()
{
	delete runMessenger;
	G4cout << G4endl << "RunAction has been deleted ";
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{	
	GenerateSeed();
	G4cout << G4endl << "Starting Run" << G4endl;
	G4cout << G4endl << "Processing..." << G4endl << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{	
	G4cout << G4endl << "Run finished" << G4endl;

	unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());
	unsigned long long int limit = 2147483647;

	G4int Progress;

	//Print the progress depending if all the particles have been done
	if(limit - TotalParticles > 0)
	{
		//Print the progress of the simulation
		Progress = ((input -> GetCurrentImage()+1)*100)/input -> GetNoImages();
		G4cout << G4endl << Progress << "%\ of the simulation is complete" << G4endl;
	}
	else
	{
		++loop;
		Progress = (((input -> GetCurrentImage()+1)*100)/input -> GetNoImages()) * ((TotalParticles - limit*loop)/TotalParticles);
		G4cout << G4endl << Progress << "%\ of the simulation is complete" << G4endl;
		G4cout << "Starting next run for the remaining number of photons" << G4endl;
	}
}

void RunAction::GenerateSeed()
{	
	//Keeps the seed
	if (seedCmd != 0)	
		{CLHEP::HepRandom::setTheSeed(seedCmd);}

	//Random seed
	else if (seedCmd == 0)	
	{
		//set random seed with system time
		seedCmd = time(NULL);
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}

	if (input -> GetCurrentImage() == 0)
		{input -> SetSeedOption(seedCmd);}
}
