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
#include "G4UnitsTable.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	G4cout << G4endl << "TrackerSD has been created "<< G4endl;

	if (data -> GetCurrentImage() == 0)
	{			
		//Save the variables into its own class
		SetNoDetectorsY(NumDetectorsY);
		SetNoDetectorsZ(NumDetectorsZ);

		data -> SetUpData(NumDetectorsZ, NumDetectorsY);	
	}
	collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD()
{
	G4cout << G4endl << "TrackerSD has been deleted "<< G4endl;
}

void TrackerSD::Initialize(G4HCofThisEvent* hce)
{	
  	//Create hits collection object
  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  	//Add this collection in hce
  	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	hce -> AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
  	//Energy deposit
  	G4double edep = aStep -> GetTotalEnergyDeposit();

  	if (edep==0.) return false;

	//Create the TrackHit class object to record hits
  	TrackerHit* newHit = new TrackerHit();

	//Save all the information about the particle that hit the detector
  	newHit -> SetTrackID  (aStep->GetTrack()->GetTrackID());
  	newHit -> SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  	newHit -> SetPos (aStep->GetPostStepPoint()->GetPosition());
	newHit -> SetParticle( aStep->GetTrack()->GetDefinition() );
	newHit -> SetEdep(edep);

	//Save the information
  	fHitsCollection -> insert( newHit );

	//Save the detector hits to the data class
	G4int DetectorNumber = newHit -> GetChamberNb();
	G4double EnergyDeposit = newHit -> GetEdep();
	data -> SaveHitData(DetectorNumber);
	data -> SaveEnergyData(DetectorNumber, EnergyDeposit);

  	return true;
}
 
void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{ 
	G4int eID = 0;
  	const G4Event* evt = G4RunManager::GetRunManager() -> GetCurrentEvent();
  	if(evt) eID = evt -> GetEventID();

	if ( verboseLevel == 1 ) 
		{ G4cout << G4endl << "Event " << eID;}
		
  	else if ( verboseLevel > 1 ) 
	{	G4int nofHits = fHitsCollection -> entries();

		G4cout << G4endl << "Event " << eID;
		if (nofHits == 0)
			{G4cout << ": No hit ";}
		if (nofHits > 0)
		{	//If there's a hit, print it if the verbose setting is greater than 1
			for (G4int i = 0; i < nofHits; i++ ) 
				{(*fHitsCollection)[i] -> Print();}
		}
  	}
}





