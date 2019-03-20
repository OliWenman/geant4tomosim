#include "DataMessenger.hh"
#include "Data.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

DataMessenger::DataMessenger(Data* DataCollection): G4UImessenger(), data(DataCollection)
{
    //Directory = new G4UIdirectory("/detector/absorption");
	//Directory -> SetGuidance("Commands to control the absorption detector variables. ");	

	RecordFullMappingFluorescence = new  G4UIcmdWithABool("/data/fluorescence/fullmapping", this);
	RecordFullMappingFluorescence -> SetGuidance("Set if you want fullmapping fluorescence data to be recorded");
	
	RecordFullFieldFluorescence = new G4UIcmdWithABool("/data/fluorescence/fullfield", this);
	RecordFullFieldFluorescence -> SetGuidance("Set if you want fullfield fluorescence data to be recorded");
	
	RecordFullMappingDiffraction = new G4UIcmdWithABool("/data/diffraction/fullmapping", this);
	RecordFullMappingDiffraction -> SetGuidance("Set if you want to record full mapping diffraction data");
	
	SetBins = new G4UIcmdWithAnInteger("/data/bins",this);
}

DataMessenger::~DataMessenger()
{
    //delete Directory;
    delete RecordFullMappingFluorescence;
    delete RecordFullFieldFluorescence;
    delete RecordFullMappingDiffraction;
    delete SetBins;
}

void DataMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == RecordFullMappingFluorescence)
    {
		data -> SetFullMappingFluorescence(RecordFullMappingFluorescence -> GetNewBoolValue(newValue));
    }
    else if (command == RecordFullFieldFluorescence)
    {
        data -> SetFullFieldFluorescence(RecordFullFieldFluorescence -> GetNewBoolValue(newValue));
    }
    else if (command == RecordFullMappingDiffraction)
    {
        data -> SetFullMappingDiffraction(RecordFullMappingDiffraction -> GetNewBoolValue(newValue));
    }
    else if (command == SetBins)
    {
        data -> SetNoBins(SetBins -> GetNewIntValue(newValue));
    }
}
