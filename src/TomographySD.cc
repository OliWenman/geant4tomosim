#include "TomographySD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

TomographySD::TomographySD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, const G4bool DetEfficiency) 
          : G4VSensitiveDetector(name), data(DataObject)
{
	//Set the values in it's own class so it doesn't have to constantly get them
	DetectorEfficiency = DetEfficiency;
}

TomographySD::~TomographySD() { }

G4bool TomographySD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	//checks if detectors are 100% efficient
	if (DetectorEfficiency == false)
	{
  		//if no energy deposit, there's no hit for realisitic detectors
  		if (aStep->GetTotalEnergyDeposit() == 0) return false;
	}

	//Finds the detector number that has been hit and saves the hit into the data array
	data -> SaveHitData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());

  	return true;
}
 
