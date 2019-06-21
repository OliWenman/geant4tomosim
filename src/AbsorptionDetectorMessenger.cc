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

//Create the commands in the constructor
AbsorptionDetectorMessenger::AbsorptionDetectorMessenger(AbsorptionDetector* Detector): G4UImessenger(), absorptiondetector(Detector)
{
    directory = new G4UIdirectory("/detector/absorption/");
	directory -> SetGuidance("Commands to control the absorption detector variables. ");	

	halfdimensions = new G4UIcmdWith3VectorAndUnit("/detector/absorption/halfdimensions", this);
	halfdimensions -> SetGuidance("Set the detectors halfdimensions");
	halfdimensions -> SetParameterName("x","y","z",true,true);
	halfdimensions -> SetUnitCandidates("mm cm m um ");
	halfdimensions -> SetRange("x > 0 || y > 0 || z > 0");

	xpixels = new G4UIcmdWithAnInteger("/detector/absorption/xpixels", this);
	xpixels -> SetGuidance("Set the number of xpixels for the detector");
	xpixels -> SetParameterName("x",true,true);
	xpixels -> SetRange("x > 2");

	ypixels = new G4UIcmdWithAnInteger("/detector/absorption/ypixels", this);
	ypixels -> SetGuidance("Set the number of ypixels for the detector");
	ypixels -> SetParameterName("x",true,true);
	ypixels -> SetRange("x > 2");
}

AbsorptionDetectorMessenger::~AbsorptionDetectorMessenger()
{
    delete directory;
    delete halfdimensions;
    delete xpixels;
    delete ypixels;
}

//Method for what happens when a command is found
void AbsorptionDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == halfdimensions)
    {
		absorptiondetector -> SetHalfDimensions(halfdimensions -> GetNew3VectorValue(newValue));
    }
    else if (command == xpixels)
    {
        absorptiondetector -> SetxPixels(xpixels -> GetNewIntValue(newValue));	
    }
    else if (command == ypixels)
    {
        absorptiondetector -> SetyPixels(ypixels -> GetNewIntValue(newValue));	
    }
}
