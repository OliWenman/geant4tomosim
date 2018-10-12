#include "DataMessenger.hh"
#include "Data.hh"
#include <stdlib.h>
#include <climits>

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

DataMessenger::DataMessenger(Data *DataObject):data(DataObject)
{	
	G4cout << G4endl << "DataMessenger has been created " << G4endl;

	DataDirectory = new G4UIdirectory("/data/");
	DataDirectory -> SetGuidance("Commands to control Data class");

	TextFileCmd = new G4UIcmdWithABool("/data/TextFile", this);
	TextFileCmd -> SetGuidance("Choose if you would like the data to be saved to a text file ");
	TextFileCmd -> SetDefaultValue(true);

	HDF5FileCmd = new G4UIcmdWithABool("/data/HDF5File", this);
	HDF5FileCmd -> SetGuidance("Choose if you would like the data to be saved to a text file ");
	HDF5FileCmd -> SetDefaultValue(true);

	NoBins_Cmd = new G4UIcmdWithAnInteger("/data/bins", this);
	NoBins_Cmd -> SetDefaultValue(5);
	NoBins_Cmd -> SetGuidance("Pick the number of bins you would like the energy to be sorted into.  ");

	NoImages_Cmd = new G4UIcmdWithAnInteger("/data/NumberOfImages", this);
	NoImages_Cmd -> SetDefaultValue(2);
	NoImages_Cmd -> SetGuidance("Pick the number of images you would to have taken ");

	NoPhotons_Cmd = new G4UIcmdWithAString("/data/NumberOfPhotons", this);
	//NoPhotons_Cmd -> SetDefaultValue(1000);
	NoPhotons_Cmd -> SetGuidance("Pick the number of photons you would to have per image ");

	Visualization_Cmd = new G4UIcmdWithABool("/data/Visualization", this);
	Visualization_Cmd -> SetGuidance("Choose if you want visualization ");
	Visualization_Cmd -> SetDefaultValue(true);

	seedCmd = new G4UIcmdWithAnInteger("/data/Seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	
	EnergyDataCmd = new G4UIcmdWithABool("/data/Energy", this);
	EnergyDataCmd -> SetGuidance("Choose if you would like energy data to be processed");
	EnergyDataCmd -> SetDefaultValue(false);
}

DataMessenger::~DataMessenger()
{
	delete DataDirectory;
	delete TextFileCmd;
	delete HDF5FileCmd;
	delete NoBins_Cmd;
	delete NoImages_Cmd;
	delete NoPhotons_Cmd;
	delete Visualization_Cmd;
	delete seedCmd;
	delete EnergyDataCmd;

	G4cout << G4endl << "DataMessenger has been deleted " << G4endl << G4endl;
}

void DataMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == TextFileCmd )
  	{ 
		data -> SetTextFileCmd(TextFileCmd -> GetNewBoolValue(newValue));
		G4cout << "Data -> TextFile command detected  " << G4endl;
	}	
	else if( command == HDF5FileCmd )
  	{ 
		data -> SetHDF5FileCmd(HDF5FileCmd -> GetNewBoolValue(newValue));
		G4cout << "Data -> HDF5File command detected  " << G4endl;
	}
	else if( command == NoBins_Cmd )
	{
		data -> SetNoBins(NoBins_Cmd -> GetNewIntValue(newValue));	
		G4cout << "Data -> SetNoBins command detected "<< G4endl;
	}
	else if( command == NoImages_Cmd )
	{
		data -> SetNoImages(NoImages_Cmd -> GetNewIntValue(newValue));	
		G4cout << "Data -> SetNoImages command detected " << G4endl;
	}
	else if( command == NoPhotons_Cmd )
	{
		//data -> SetNoPhotons(NoPhotons_Cmd -> GetNewIntValue(newValue));	
		G4cout << "Data -> SetNoPhotons command detected = "<< newValue <<G4endl;
		data -> SetNoPhotons(newValue);
	}
	else if( command == Visualization_Cmd )
	{
		data -> SetVisualization(Visualization_Cmd -> GetNewBoolValue(newValue));	
		G4cout << "Data -> SetVisualization command detected "<< G4endl;
	}
	else if( command == seedCmd )
	{
		data -> SetSeedOption(seedCmd -> GetNewIntValue(newValue));	
		G4cout << "Data -> SetSeedOption command detected "<< G4endl;
	}
	else if (command == EnergyDataCmd)
	{
		data -> SetEnergyDataOption(EnergyDataCmd -> GetNewBoolValue(newValue));
		G4cout << "Data -> SetEnergyDataOption command detected "<< G4endl;
	}
}
