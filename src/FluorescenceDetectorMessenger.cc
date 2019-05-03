#include "FluorescenceDetectorMessenger.hh"
#include "FluorescenceDetector.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

#include "FluorescenceSD.hh"

FluorescenceDetectorMessenger::FluorescenceDetectorMessenger(FluorescenceDetector* fluordetector): G4UImessenger(), detectorbuilder(fluordetector)
{
    directory = new G4UIdirectory("/detector/fluorescence/");
	directory -> SetGuidance("Commands to control the fluorescence detector variables. ");	

	setdimensions = new G4UIcmdWith3VectorAndUnit("/detector/fluorescence/halfdimensions", this);
	setdimensions -> SetGuidance("Set the detectors halfdimensions");
	setdimensions -> SetParameterName("x","y","z",true,true);
	setdimensions -> SetUnitCandidates("mm cm m um ");
	setdimensions -> SetRange("x > 0 || y > 0 || z > 0");
	
	setfullmapping = new G4UIcmdWithABool("/detector/fluorescence/fullmapping", this);
	setfullmapping -> SetGuidance("Set if you want full mapping fluorescence data to be recorded");
	
	setfullfield = new G4UIcmdWithABool("/detector/fluorescence/fullfield", this);
	setfullfield -> SetGuidance("Set if want full field fluorescence data to be recorded");
	
    setbins = new G4UIcmdWithAnInteger("/detector/fluorescence/bins", this);
    setbins -> SetGuidance("Set the number of bins you would like to record fluorescence with"); 
    
    setmaxenergy = new G4UIcmdWithADoubleAndUnit("/detector/fluorescence/maxenergy", this);
    setmaxenergy -> SetGuidance("Set the maximum energy that the detector can record");
}

FluorescenceDetectorMessenger::~FluorescenceDetectorMessenger()
{
    delete directory;
    delete setdimensions;
    delete setfullmapping;
    delete setfullfield;
    delete setbins;
    delete setmaxenergy;
}

void FluorescenceDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == setdimensions)
    {
		detectorbuilder->SetHalfDimensions(setdimensions -> GetNew3VectorValue(newValue));
    }
    else if (command == setfullmapping)
    {
        detectorbuilder->GetSensitiveDetector()->RecordFullMapping(setfullmapping->GetNewBoolValue(newValue));
    }
    else if (command == setfullfield)
    {
        detectorbuilder->GetSensitiveDetector()->RecordFullField(setfullfield->GetNewBoolValue(newValue));
    }
    else if (command == setbins)
    {
        detectorbuilder->GetSensitiveDetector()->SetNumberOfBins(setbins->GetNewIntValue(newValue));
    }
    else if (command == setmaxenergy)
    {
        setmaxenergy->GetNewUnitValue(newValue);
        detectorbuilder->GetSensitiveDetector()->SetMaxEnergy(setmaxenergy->GetNewDoubleValue(newValue)*1000);
    }
}
