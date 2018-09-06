#include "RunAction.hh"
#include "RunActionMessenger.hh"
#include "Data.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"
#include "time.h"

//#include "GlobalClasses.hh"

RunAction::RunAction(Data* DataObject): G4UserRunAction(), data(DataObject)
{ 
	G4cout << G4endl << "RunAction has been created ";
	runMessenger = new RunActionMessenger(this);
}

RunAction::~RunAction()
{
	delete runMessenger;
	//delete G4AnalysisManager::Instance();
	//delete data;
	G4cout << G4endl << "RunAction has been deleted " << G4endl;
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{	
	if (seedCmd != 0)	//Keeps the seed
	{
		CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
		CLHEP::HepRandom::setTheSeed(seedCmd);

		data -> SetSeed(seedCmd);
	}
	else if (seedCmd == 0)	//Random seed
	{
		//choose the Random engine
		CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());

		///set random seed with system time
		G4long RandomSeed = time(NULL);
		CLHEP::HepRandom::setTheSeed(RandomSeed);

		data -> SetSeed(RandomSeed);
	}

	G4cout << G4endl << "Starting Run." << G4endl;
	G4cout << G4endl << "Processing..." << G4endl << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{	
	G4cout << G4endl << "Run finished" << G4endl;
	if (GetPrintOption() == "true")
		{data -> PrintHitData();}

	//Print the progress of the simulation
	G4int Progress = ((data -> GetCurrentImage()+1)*100)/data -> GetNoImages();
	G4cout << G4endl << Progress << "%\ of the simulation is complete" << G4endl;
}


