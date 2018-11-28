#include "TrackerSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, const G4bool DetEfficiency) 
          : G4VSensitiveDetector(name), data(DataObject)
{
	//Set the values in it's own class so it doesn't have to constantly get them
	DetectorEfficiency = DetEfficiency;
	nBins = data -> GetNoBins();
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
	if (nBins > 0)
		{data -> SaveEnergyData(nDetector, aStep->GetPreStepPoint()->GetKineticEnergy());
		data -> SaveEnergyFreq(aStep->GetPreStepPoint()->GetKineticEnergy());}

	//if (aStep->GetPreStepPoint()->GetKineticEnergy() != 0.025)
	//{
	//	G4cout << nDetector <<": Energy of Photon = " << (aStep->GetPreStepPoint()->GetKineticEnergy())*1000 << " eV " <<G4endl;
	//	G4cout << nDetector <<":     Delta Energy = " << (0.025 - aStep->GetPreStepPoint()->GetKineticEnergy())*1000 << " eV " <<G4endl;
	//}

  	return true;
}
 
