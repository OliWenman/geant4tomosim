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
	std::ofstream outdata; 
   
	//Get needed variables to wrtie to file
	G4int TotalNoRows = GetNumberRows();
	G4int TotalNoColumns = GetNumberColumns();
	G4int CImage = GetCurrentImage();

	G4String FilePath = "./Data_Output/Text/";

	//Save the simulation settings to a textfile only once

	G4cout << G4endl << "Saving the SimulationSettings... ";
	G4String SettingsName = "SimulationSettings.txt";
	G4ThreeVector DetDimensions = GetDetectorDimensions()*2;

	outdata.open(FilePath+SettingsName); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "Error: " << SettingsName << " file could not be opened" << std::endl;
      		exit(1);
   	}

	//Save information about the conditions used for the simulation
	outdata << "Simulation of X-ray data." << std::endl << std::endl;

	outdata << "Conditioons used in this simulation: " << std::endl;
	outdata << "- Physics package: " << GetPhysicsUsed() << std::endl;
	outdata << "- Seed: " << GetSeedOption() << std::endl << std::endl;

	outdata << "- Intial energy of the monochromatic beam: " << G4BestUnit(GetMaxEnergy(),"Energy") << std::endl;
	outdata << "- Beam dimensions: " << GetBeamWidth() << " x " << G4BestUnit(GetBeamHeight(), "Length") << std::endl; 
	outdata << "- Distance beam travelled to the detectors: " << G4BestUnit(GetBeamLength(), "Length") << std::endl << std::endl;

	outdata << "- Number of detectors: " << TotalNoRows << " x " << TotalNoColumns << std::endl;
	outdata << "- Detector dimensions: " << G4BestUnit(DetDimensions.x(), "Length") << "x " << G4BestUnit(DetDimensions.y(), "Length") << "x " << G4BestUnit(DetDimensions.z(), "Length") << std::endl;
	if (GetDetectorEfficiency() == true)
		{outdata << "- Detectors used are 100%\ efficient" << std::endl;}
	else 
		{outdata << "- Detector material: " << GetDetectorMaterial() << std::endl;}

	outdata << std::endl << "- Number of photons used per image: " << GetNoPhotons() << std::endl;
	outdata << std::endl << "- Number of photons per detector on average is (assuming total number of detectors are the same dimensions as the beam): "
			     << std::stoull(GetNoPhotons())/(TotalNoColumns*TotalNoRows) << std::endl;
	outdata << "- Number of images: " << GetNoImages() << std::endl;
	if (GetEnergyDataOption() == true)
		{outdata << "- Number of bins: " << GetNoBins() << std::endl << std::endl;}
	else 
		{outdata << "- Energy data was not recorded" << std::endl << std::endl;}

	outdata << "- Real simulation time: " << GetSimulationTime() << "s" << std::endl;
	outdata << std::endl;

	outdata.close();
}

