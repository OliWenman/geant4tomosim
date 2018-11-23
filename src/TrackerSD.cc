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
          : G4VSensitiveDetector(name), data(DataObject)
{
	DetectorEfficiency = DetEfficiency;
	EnergyDataOption = EOption;
}

TrackerSD::~TrackerSD() { }

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

  	return true;
}
 
