#include "Input.hh"
#include "InputMessenger.hh"
#include "Data.hh"

#include <iomanip>
#include <fstream>

#include "G4UnitsTable.hh"
//#include <string.h>
#include <vector>
#include "G4ThreeVector.hh"
#include <string> 

Input::Input(Data* DataObject): data(DataObject)
{
	inputMessenger = new InputMessenger(this, data);
	data -> SetMaxEnergy(GetMaxEnergy());
}

Input::~Input()
{
	delete inputMessenger;
}

void Input::WriteToTextFile()
{
	//Creation of the writing to data file stream
	std::ofstream outdata; 

	G4String FilePath = "./Data_Output/Text/";

	G4cout << G4endl << "Saving the SimulationSettings... ";
	G4String SettingsName = "SimulationSettings.txt";
	G4ThreeVector DetDimensions = GetDetectorDimensions()*2;

	//Open the file within the path set
	outdata.open(FilePath+SettingsName); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "Error: " << SettingsName << " file could not be opened" << std::endl;
      		exit(1);
   	}

	//Save information about the conditions used for the simulation
	outdata << "Simulation of X-ray data." << std::endl << std::endl;

	outdata << "Conditioons used in this simulation: " << std::endl;
	outdata << "- Physics package: " << PhysicsUsed << std::endl;
	outdata << "- Seed: " << seedCmd << std::endl << std::endl;

	outdata << "- Intial energy of the monochromatic beam: " << G4BestUnit(MaxE,"Energy") << std::endl;
	outdata << "- Beam dimensions: " << BeamWidth << " x " << G4BestUnit(BeamHeight, "Length") << std::endl; 
	outdata << "- Distance beam travelled to the detectors: " << G4BestUnit(BeamLength, "Length") << std::endl << std::endl;

	outdata << "- Number of detectors: " << rows << " x " << columns << std::endl;
	outdata << "- Detector dimensions: " << G4BestUnit(DetDimensions.x(), "Length") << "x " << G4BestUnit(DetDimensions.y(), "Length") << "x " << G4BestUnit(DetDimensions.z(), "Length") << std::endl;
	if (EnergyDataCmd == true)
		{outdata << "- Detectors used are 100%\ efficient" << std::endl;}
	else 
		{outdata << "- Detector material: " << GetDetectorMaterial() << std::endl;}

	outdata << std::endl << "- Number of photons used per image: " << NoPhotons_Cmd << std::endl;
	outdata << std::endl << "- Number of photons per detector on average is (assuming total number of detectors are the same dimensions as the beam): "
			     << std::stoull(NoPhotons_Cmd)/(rows*columns) << std::endl;
	outdata << "- Number of images: " << NoImages_Cmd << std::endl;
	if (GetEnergyDataOption() == true)
		{outdata << "- Number of bins: " << NoBins_Cmd << std::endl << std::endl;}
	else 
		{outdata << "- Energy data was not recorded" << std::endl << std::endl;}

	outdata << "- Real simulation time: " << SimulationTime << "s" << std::endl;
	outdata << std::endl;

	outdata.close();
}

