#include "DiffractionSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

DiffractionSD::DiffractionSD() : G4VSensitiveDetector("DiffractionDetecotor"), fHitsCollection(0)
{

}

DiffractionSD::~DiffractionSD() {}

void DiffractionSD::Initialize(G4HCofThisEvent* hce)
{	
    if(graphicsOn)
    {
  	    //Create hits collection object
  	    fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 
  	    //Add this collection in hce
  	    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	    hce -> AddHitsCollection( hcID, fHitsCollection ); 
    }
}

G4bool DiffractionSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
    //G4cout << "\n" << n << ")DIFFRACTION HIT " << G4endl;
    //n++;

	G4int nDetector = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
	//Save the detector hits to the data class
	//data -> SaveDiffraction(nDetector);
	
	aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	
	if(graphicsOn)
	{
	    //Create the TrackerHit class object to record hits
  	    TrackerHit* newHit = new TrackerHit();

	    //Save all the information about the particle that hit the detector
  	    newHit -> SetChamberNb(nDetector);
	    newHit -> SetEdep(aStep->GetPreStepPoint()->GetKineticEnergy());
	    newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	    fHitsCollection -> insert( newHit );
	}

  	return true;
}
