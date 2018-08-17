#include "TrackerSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL)
{
  	collectionName.insert(hitsCollectionName);
}


TrackerSD::~TrackerSD()
{
	delete fHitsCollection;
}


void TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  	// Create hits collection

  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  	// Add this collection in hce

  	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	hce->AddHitsCollection( hcID, fHitsCollection ); 
}



G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
  	// energy deposit
  	G4double edep = aStep->GetTotalEnergyDeposit();

  	if (edep==0.) return false;

  	TrackerHit* newHit = new TrackerHit();

  	newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  	newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  	newHit->SetEdep(edep);
  	newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());
	newHit->SetParticle( aStep->GetTrack()->GetDefinition() );

  	fHitsCollection->insert( newHit );
	G4int nofHits = fHitsCollection->entries();
		/*if (nofHits > 1);
		{
			newHit->Print();
		}*/

  	return true;
}

void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  	if ( verboseLevel>1 ) 
	{ 
     		G4int nofHits = fHitsCollection->entries();
		if (nofHits > 0)
		{
     			G4cout << G4endl
		       	       //<< "-----------------------------------------------------------------------------"
		       	       //<< G4endl
		       	       << nofHits << " photon(s) " << G4endl;
     			for ( G4int i=0; i<nofHits; i++ ) {
				(*fHitsCollection)[i]->Print();	
				//auto array (*fHitsCollection)[i]->ConvertToArray();
				//auto saver;				
				//saver.save(array);
			}
		}

  	}
	
	
}

