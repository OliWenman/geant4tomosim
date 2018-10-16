#include "TrackerSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, G4bool DetectorEfficiency) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	G4cout << G4endl << "TrackerSD has been created "<< G4endl;

	//Setup the data only at the begining of simulation so it doesn't waste time doing work it has already done	
	DetectorEfficiency_Cmd = DetectorEfficiency;
	EnergyOption_Cmd = data ->GetEnergyDataOption();

	data -> SetUpHitData(NumDetectorsZ, NumDetectorsY);
	if (EnergyOption_Cmd == true)
		{data -> SetUpEnergyData();}
	
	collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD() {G4cout << G4endl << "TrackerSD has been deleted ";}

void TrackerSD::Initialize(G4HCofThisEvent* hce)
{}

G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	if (DetectorEfficiency_Cmd == false)
	{
  		// energy deposit
  		if (aStep->GetTotalEnergyDeposit()==0.) return false;
	}

	data -> SaveHitData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
	if (EnergyOption_Cmd  == true)
		{data -> SaveEnergyData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(), aStep->GetPreStepPoint()->GetKineticEnergy());}

	aStep->GetTrack()->SetTrackStatus(fStopAndKill);

  	return true;
}
 
void TrackerSD::EndOfEvent(G4HCofThisEvent*)
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



//149.56s 10,000,000 photons
//146.61s 10,000,000 photons without data class
//144.72s
//151.34s without deleting newHit
//72.88s 10,000,000 photons 1D data  

