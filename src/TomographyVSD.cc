#include "TomographyVSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

TomographyVSD::TomographyVSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, const G4bool DetEfficiency) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	//Setup the data only at the begining of simulation so it doesn't waste time doing work it has already done	
	DetectorEfficiency = DetEfficiency;
	
	collectionName.insert(hitsCollectionName);
}

TomographyVSD::~TomographyVSD() {}

void TomographyVSD::Initialize(G4HCofThisEvent* hce)
{	
  	//Create hits collection object
  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 
  	//Add this collection in hce
  	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	hce -> AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool TomographyVSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	if (DetectorEfficiency == false)
	{
  		// energy deposit
  		if (aStep->GetTotalEnergyDeposit() == 0.) return false;
	}
	
	G4int nDetector = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
	G4double ParticleEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

	//Create the TrackerHit class object to record hits
  	TrackerHit* newHit = new TrackerHit();

	//Save all the information about the particle that hit the detector
  	newHit -> SetChamberNb(nDetector);
	newHit -> SetEdep(ParticleEnergy);
	newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	fHitsCollection -> insert( newHit );
	
	//Save the detector hits to the data class
	data -> SaveHitData(nDetector);;

  	return true;
}
 
void TomographyVSD::EndOfEvent(G4HCofThisEvent*)
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
			{	//If there's a hit, print it if the verbose setting is greater than 1
				if (EnergyOption == true)
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

