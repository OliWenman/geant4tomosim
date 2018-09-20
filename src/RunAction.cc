#include "RunAction.hh"
#include "RunActionMessenger.hh"
#include "Data.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"
#include "time.h"

RunAction::RunAction(Data* DataObject): G4UserRunAction(), data(DataObject)
{ 
	G4cout << G4endl << "RunAction has been created ";
	runMessenger = new RunActionMessenger(this);

	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
}

RunAction::~RunAction()
{
	delete runMessenger;
	G4cout << G4endl << "RunAction has been deleted " << G4endl;
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
	if (GetPrintOption() == true)
		{data -> PrintHitData();}

	//Print the progress of the simulation
	G4int Progress = ((data -> GetCurrentImage()+1)*100)/data -> GetNoImages();
	G4cout << G4endl << Progress << "%\ of the simulation is complete" << G4endl;
}

void RunAction::GenerateSeed()
{	
	if (seedCmd != 0)	//Keeps the seed
	{		
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}
	else if (seedCmd == 0)	//Random seed
	{
		//set random seed with system time
		seedCmd = time(NULL);
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}

	if (data ->GetCurrentImage() == 0)
		{data -> SetSeedOption(seedCmd);}
}
