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

	RecordFullMapping = new  G4UIcmdWithABool("/data/fluorescence/fullmapping", this);
	RecordFullMapping -> SetGuidance("Set if you want fullmapping fluorescence data to be recorded");
	
	RecordFullField = new G4UIcmdWithABool("/data/fluorescence/fullfield", this);
	RecordFullField -> SetGuidance("Set if you want fullfield fluorescence data to be recorded");
	
	SetBins = new G4UIcmdWithAnInteger("/data/bins",this);
}

DataMessenger::~DataMessenger()
{
    //delete Directory;
    delete RecordFullMapping;
    delete RecordFullField;
    delete SetBins;
}

void DataMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == RecordFullMapping)
    {
		data -> SetFullMappingFluorescence(RecordFullMapping -> GetNewBoolValue(newValue));
    }
    else if (command == RecordFullField)
    {
        data -> SetFullFieldFluorescence(RecordFullField -> GetNewBoolValue(newValue));
    }
    else if (command == SetBins)
    {
        data -> SetNoBins(SetBins -> GetNewIntValue(newValue));
    }
}
