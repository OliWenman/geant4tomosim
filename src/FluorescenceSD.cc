#include "FluorescenceSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

FluorescenceSD::FluorescenceSD(const G4String& name, Data* DataObject, const G4bool DetEfficiency)
	       : G4VSensitiveDetector(name), data(DataObject)
{
	DetectorEfficiency = DetEfficiency;
	fluoreFullField = true;
	fluoreFullMapping = false;
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

	G4double PhotonEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

	if (fluoreFullMapping == true)
	{
		data -> SaveFullMapping(PhotonEnergy);
	}
	if (fluoreFullField == true)
	{
		data -> SaveEnergyFreq(PhotonEnergy);
	}

	return true;
}
