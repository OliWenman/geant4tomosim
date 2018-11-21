#include "VisTrackerSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

VisTrackerSD::VisTrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, G4bool DetectorEfficiency) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	//Setup the data only at the begining of simulation so it doesn't waste time doing work it has already done	
	DetectorEfficiency_Cmd = DetectorEfficiency;
	EnergyOption_Cmd = data ->GetEnergyDataOption();

	//data -> SetUpHitData(NumDetectorsZ, NumDetectorsY);
	//if (EnergyOption_Cmd == true)
	//	{data -> SetUpEnergyData();}
	
	collectionName.insert(hitsCollectionName);
}

VisTrackerSD::~VisTrackerSD() {}

void VisTrackerSD::Initialize(G4HCofThisEvent* hce)
{	
  	//Create hits collection object
  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 
  	//Add this collection in hce
  	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	hce -> AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool VisTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	if (DetectorEfficiency_Cmd == false)
	{
  		// energy deposit
  		G4double edep = aStep->GetTotalEnergyDeposit();
  		if (edep==0.) return false;
	}
	
	G4double ParticleEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

	//Create the TrackerHit class object to record hits
  	TrackerHit* newHit = new TrackerHit();

	//Save all the information about the particle that hit the detector
  	//newHit -> SetTrackID  (aStep->Gep->GetetTrack()->GetTrackID());
	//newHit -> SetParticle( aStTrack()->GetDefinition() );
  	newHit -> SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
	newHit -> SetEdep(ParticleEnergy);
	
	//Needed for visualization of hits, only need it if it's turned on
	
	newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	fHitsCollection -> insert( newHit );
	
	//Save the detector hits to the data class
	G4int nDetector = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
	data -> SaveHitData(nDetector);;

	if (EnergyOption_Cmd  == true)
		{data -> SaveEnergyData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(), ParticleEnergy);}


	aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	//G4Track*track;
	//track->SetTrackStatus(fStopAndKill);
	//aStep -> UpdateTrack;

  	return true;
}
 
void VisTrackerSD::EndOfEvent(G4HCofThisEvent*)
{ 
	if (verboseLevel > 0)
	{	
  		const G4Event* evt = G4RunManager::GetRunManager() -> GetCurrentEvent();

		G4cout << G4endl << "Event " << evt -> GetEventID();
		
  		if ( verboseLevel > 1)
		{
			G4int nofHits = fHitsCollection -> entries();
			G4cout << ": ";
			if (nofHits == 0)
				{G4cout << " No hit ";}
			else
			{	
				if (EnergyOption_Cmd == true)//If there's a hit, print it if the verbose setting is greater than 1
					{for (G4int i = 0; i < nofHits; i++ ) 
						{(*fHitsCollection)[i] -> PrintEnergy();}
					}
				else 
					{for (G4int i = 0; i < nofHits; i++ ) 
						{(*fHitsCollection)[i] -> PrintHit();}
					}
			}
  		}
	}
}

