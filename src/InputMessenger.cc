#include "InputMessenger.hh"
#include "Input.hh"
#include "Data.hh"
#include <stdlib.h>
#include <climits>

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

InputMessenger::InputMessenger(Input *InputObject, Data* DataObject):input(InputObject), data(DataObject)
{	
	InputDirectory = new G4UIdirectory("/data/");
	InputDirectory -> SetGuidance("Commands to control Data class");

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
	NoPhotons_Cmd -> SetGuidance("Pick the number of photons you would to have per image ");

	seedCmd = new G4UIcmdWithAnInteger("/data/Seed", this);
	seedCmd -> SetDefaultValue(0);
	seedCmd -> SetGuidance("Enter a seed for the simulation. Input 0 for a random seed ");
	
	EnergyDataCmd = new G4UIcmdWithABool("/data/Energy", this);
	EnergyDataCmd -> SetGuidance("Choose if you would like energy data to be processed");
	EnergyDataCmd -> SetDefaultValue(false);
}

InputMessenger::~InputMessenger()
{
	delete InputDirectory;
	delete TextFileCmd;
	delete HDF5FileCmd;
	delete NoBins_Cmd;
	delete NoImages_Cmd;
	delete NoPhotons_Cmd;
	delete seedCmd;
	delete EnergyDataCmd;
}

void InputMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == TextFileCmd )
  	{ 
		input -> SetTextFileCmd(TextFileCmd -> GetNewBoolValue(newValue));
		data -> SetTextFileCmd(TextFileCmd -> GetNewBoolValue(newValue));
	}	
	else if( command == HDF5FileCmd )
  	{ 
		input -> SetHDF5FileCmd(HDF5FileCmd -> GetNewBoolValue(newValue));
	}
	else if( command == NoBins_Cmd )
	{
		input -> SetNoBins(NoBins_Cmd -> GetNewIntValue(newValue));
		data ->	SetNoBins(NoBins_Cmd -> GetNewIntValue(newValue));
	}
	else if( command == NoImages_Cmd )
	{
		input -> SetNoImages(NoImages_Cmd -> GetNewIntValue(newValue));	
	}
	else if( command == NoPhotons_Cmd )
	{
		input -> SetNoPhotons(newValue);
	}
	else if( command == seedCmd )
	{
		input -> SetSeedOption(seedCmd -> GetNewIntValue(newValue));	
	}
	else if (command == EnergyDataCmd)
	{
		input -> SetEnergyDataOption(EnergyDataCmd -> GetNewBoolValue(newValue));
		data -> SetEnergyDataOption(EnergyDataCmd -> GetNewBoolValue(newValue));
	}
}
