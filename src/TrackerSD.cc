#include "TrackerSD.hh"
#include "Data.hh"
#include <vector>

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL)
{
	//Save the number of detectors into its own class
	SetNoDetectorsY(NumDetectorsY);
	SetNoDetectorsZ(NumDetectorsZ);

	//Create the data object
	data = new Data(NumDetectorsZ, NumDetectorsY);

  	collectionName.insert(hitsCollectionName);	

	
}


TrackerSD::~TrackerSD()
{
	//Free up memory
	delete fHitsCollection;
	delete data;
}


void TrackerSD::Initialize(G4HCofThisEvent* hce)
{	
  	//Create hits collection object
  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  	//Add this collection in hce
  	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	hce->AddHitsCollection( hcID, fHitsCollection ); 
}



G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
  	//Energy deposit
  	G4double edep = aStep->GetTotalEnergyDeposit();

  	if (edep==0.) return false;

	//Create the TrackHit class object to record hits
  	TrackerHit* newHit = new TrackerHit();

	//Save all the information about the particle that hit the detector
  	newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  	newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  	newHit->SetEdep(edep);
  	newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());
	newHit->SetParticle( aStep->GetTrack()->GetDefinition() );

	//Save the information
  	fHitsCollection->insert( newHit );

	//Save the detector hit to the data class
	G4int DetectorNumber = newHit -> GetChamberNb();
	data -> SaveData(DetectorNumber);

  	return true;
}

//Maybe try const 
//void TrackerSD::EndOfEvent(G4Run* aRun, G4HCofThisEvent*)
void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{ 
	//
	if (verboseLevel == 0)
	{
		G4int EventID = 0;
		G4int TotalEventID = 0;

  		const G4Event* CurrentEvent = G4RunManager::GetRunManager()->GetCurrentEvent();
		//G4int NumberOfEventsLeft = aRun -> GetNumberOfEventToBeProcessed();
		
		//G4cout << G4endl << NumberOfEventsLeft << G4endl;
			
  		if(CurrentEvent) EventID = CurrentEvent -> GetEventID();

		if (EventID == 119)
		{
			G4cout << "The final resulting data : " << G4endl;
			data -> Print();
		}
	}
		
  	if ( verboseLevel > 0 ) 
	{ 
     		G4int nofHits = fHitsCollection -> entries();
		if (nofHits > 0)
		{	
			//If there's a hit, print it if the verbose setting is greater than 0
			for ( G4int i = 0; i < nofHits; i++ ) 
			{	(*fHitsCollection)[i]->Print();	
				(*fHitsCollection)[0] -> RootFile();}
			
			//Print the new matrix if the verbose setting is greater than 1
			if ( verboseLevel > 1)
			{	data -> Print();}
		}
		
  	}
}





