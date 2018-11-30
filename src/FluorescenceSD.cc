#include "FluorescenceSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

FluorescenceSD::FluorescenceSD(const G4String& name, Data* DataObject, const G4bool DetEfficiency)
	       : G4VSensitiveDetector(name), data(DataObject)
{
	DetectorEfficiency = DetEfficiency;
}

FluorescenceSD::~FluorescenceSD(){}

G4bool FluorescenceSD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
	//checks if detectors are 100% efficient
	if (DetectorEfficiency == false)
	{
  		//if no energy deposit, there's no hit for realisitic detectors
  		if (aStep->GetTotalEnergyDeposit() == 0) return false;
	}

	//Finds the energy of the incoming photon
	data -> SaveEnergyFreq(aStep->GetPreStepPoint()->GetKineticEnergy());

	return true;
}
