#include "TrackerSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, const G4bool DetEfficiency, const G4bool EOption) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	G4cout << G4endl << "TrackerSD has been created "<< G4endl;

	DetectorEfficiency = DetEfficiency;
	EnergyDataOption = EOption;

	//Setup data for energy if the setting is turned on
	data -> SetUpHitData(NumDetectorsZ, NumDetectorsY);
	if (EnergyDataOption == true)
		{data -> SetUpEnergyData();}
	
	//collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD() {G4cout << G4endl << "TrackerSD has been deleted ";}

/*void TrackerSD::Initialize(G4HCofThisEvent* hce)
{}*/

G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	//checks if detectors are 100% efficient
	if (DetectorEfficiency == false)
	{
  		//if no energy deposit, there's no hit for realisitic detectors
  		if (aStep->GetTotalEnergyDeposit() == 0) return false;
	}

	//Finds the detector number that has been hit
	G4int nDetector = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();	

	//Saves the hit into the data
	data -> SaveHitData(nDetector);
	
	//Saves the data only if the energy command is true
	if (EnergyDataOption  == true)
		{data -> SaveEnergyData(nDetector, aStep->GetPreStepPoint()->GetKineticEnergy());}

	//Kill the track
	//aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	//aStep->UpdateTrack();

  	return true;
}
 
void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{ 
	//Get information about the events
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
				if (EnergyDataOption == true)//If there's a hit, print it if the verbose setting is greater than 1
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

