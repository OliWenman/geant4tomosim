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
	G4cout << G4endl << "Starting Run " << G4endl;
	G4cout << G4endl << "Processing... " << G4endl << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{	
	G4cout << G4endl << "Run finished " << G4endl;

	unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());
	unsigned long long int limit = 2147483647;

	//Rounds up to the nearest number to do the correct number of loops
	int NumberOfLoops = std::ceil(TotalParticles/limit);

	//Print the progress of the simulation
	int Progress = ((loop+1)*100)/input -> GetNoImages();

	//Print the progress depending if all the particles have been done
	if(limit - TotalParticles  > 0)
	{	
		//Outputs how much of the simulation is complete
		G4cout << G4endl << Progress << "%\ is complete " << G4endl;
	}
	else
	{	
		//Equation to work out how much of the simulation is complete if it has to do another run for the remaining number of photons
		Progress = Progress * (TotalParticles - limit*(NumberOfLoops - loop+1)/TotalParticles);
		G4cout << G4endl << Progress << "%\ is complete " << G4endl;

		//If less then 100% complete, output preparing to do another run of the same image
		if (Progress < 100)
			{G4cout << "Starting next run for the remaining number of photons " << G4endl;}
	}

	++loop;
}

void RunAction::GenerateSeed()
{	
	//Keeps the seed
	if (seedCmd != 0)	
		{CLHEP::HepRandom::setTheSeed(seedCmd);}

	//Random seed
	else if (seedCmd == 0 && loop == 0)	
	{
		//set random seed with system time
		seedCmd = time(NULL);
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}

	if (loop == 0)
		{input -> SetSeedOption(seedCmd);}
}
