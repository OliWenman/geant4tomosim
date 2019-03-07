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

FluorescenceDetectorMessenger::FluorescenceDetectorMessenger(FluorescenceDetector* Detector): G4UImessenger(), FD(Detector)
{
    //Directory = new G4UIdirectory("/detector/absorption");
	//Directory -> SetGuidance("Commands to control the absorption detector variables. ");	

	SetDimensions = new G4UIcmdWith3VectorAndUnit("/detector/fluorescence/halfdimensions", this);
	SetDimensions -> SetGuidance("Set the detectors halfdimensions");
	SetDimensions -> SetParameterName("x","y","z",true,true);
	SetDimensions -> SetUnitCandidates("mm cm m um ");
	SetDimensions -> SetRange("x > 0 || y > 0 || z > 0");
}

FluorescenceDetectorMessenger::~FluorescenceDetectorMessenger()
{
    //delete Directory;
    delete SetDimensions;
}

void FluorescenceDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == SetDimensions)
    {
		FD -> SetHalfDimensions(SetDimensions -> GetNew3VectorValue(newValue));
    }
}
