#include "AbsorptionDetectorMessenger.hh"
#include "AbsorptionDetector.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

AbsorptionDetectorMessenger::AbsorptionDetectorMessenger(AbsorptionDetector* Detector): G4UImessenger(), AD(Detector)
{
    //Directory = new G4UIdirectory("/detector/absorption");
	//Directory -> SetGuidance("Commands to control the absorption detector variables. ");	

	SetDimensions = new G4UIcmdWith3VectorAndUnit("/detector/absorption/halfdimensions", this);
	SetDimensions -> SetGuidance("Set the detectors halfdimensions");
	SetDimensions -> SetParameterName("x","y","z",true,true);
	SetDimensions -> SetUnitCandidates("mm cm m um ");
	SetDimensions -> SetRange("x > 0 || y > 0 || z > 0");

	SetRows = new G4UIcmdWithAnInteger("/detector/absorption/rows", this);
	SetRows -> SetGuidance("Set the number of cells along the detectors rows");

	SetColumns = new G4UIcmdWithAnInteger("/detector/absorption/columns", this);
	SetColumns -> SetGuidance("Set the number of cells along the detectors columns");
}

AbsorptionDetectorMessenger::~AbsorptionDetectorMessenger()
{
    //delete Directory;
    delete SetDimensions;
    delete SetRows;
    delete SetColumns;
}

void AbsorptionDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == SetDimensions)
    {
		AD -> SetHalfDimensions(SetDimensions -> GetNew3VectorValue(newValue));
    }
    else if (command == SetRows)
    {
        AD -> SetRows(SetRows -> GetNewIntValue(newValue));	
    }
    else if (command == SetColumns)
    {
        AD -> SetColumns(SetColumns -> GetNewIntValue(newValue));	
    }
}
