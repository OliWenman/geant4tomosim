#include "RunAction.hh"
#include "RunActionMessenger.hh"
#include "Data.hh"

#include "Analysis.hh"

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
	
	// Create analysis manager
  	auto analysisManager = G4AnalysisManager::Instance();
  	analysisManager->SetVerboseLevel(1);
  	analysisManager->SetFirstHistoId(0);
}

RunAction::~RunAction()
{
	delete runMessenger;
	delete G4AnalysisManager::Instance();
	delete data;
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
	G4int NoEvents = aRun->GetNumberOfEventToBeProcessed();
	SetTotalNoEvents(NoEvents);

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

	const G4String& name = "X-Ray_Picture";
	const G4String& title = "Projection Test";
        G4int nxbins = 100;
	G4double xmin = -0.5*m;
	G4double xmax = 0.5*m;
        G4int nybins = 100;
	G4double ymin = -0.5*m;
	G4double ymax = 0.5*m;

        const G4String& xunitName = "m";
        const G4String& yunitName = "m";
        const G4String& zunitName = "m";
        const G4String& xfcnName = "none";
        const G4String& yfcnName = "none";
        const G4String& zfcnName = "none";
        const G4String& xbinSchemeName = "linear";
        const G4String& ybinSchemeName = "linear";
        const G4String& zbinSchemeName = "linear";


//----------------------------------------------------------------------------
  	//inform the runManager to save random number seed
  	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	//Get analysis manager
  	auto analysisManager = G4AnalysisManager::Instance();
  	// Open an output file
  	analysisManager->OpenFile("Output");

	// Creating histograms
  	analysisManager->CreateH2(name, title,
               			  nxbins, xmin, xmax,
               			  nybins, ymin, ymax,
               			  xunitName,
               			  yunitName,
               			  xfcnName,
               		          yfcnName,
               			  xbinSchemeName,
               			  ybinSchemeName);

}


void RunAction::EndOfRunAction(const G4Run*)
{	
	G4cout << G4endl << "The simulation is complete" << G4endl;
	G4cout << "==============================================================" << G4endl << "Information" << G4endl;
	G4cout << "==============================================================" << G4endl;
	G4cout << G4endl << "The seed value used for this simulation is " << data -> GetSeed() << G4endl;
	data -> PrintHitData();
	data -> PrintEnergyData();

	// Save histograms
  	auto analysisManager = G4AnalysisManager::Instance();
  	analysisManager->Write();
  	analysisManager->CloseFile();
}

