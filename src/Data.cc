#include "Data.hh"
#include "DataMessenger.hh"
#include <iomanip>
#include <fstream>
#include "G4UnitsTable.hh"
//#include <string.h>
#include <vector>
#include "G4ThreeVector.hh"

//#include "hdf5.h"
//#include "H5Cpp.h"

//using namespace H5;

Data::Data()
{
	G4cout << G4endl << "Data class has been created "<< G4endl;
	dataMessenger = new DataMessenger(this);
}

Data::~Data()
{
	G4cout << G4endl << "Data class has been deleted ";
	delete dataMessenger;
}

void Data::SetUpHitData(G4int Nrow, G4int Ncolumn)
{
	//Saves the rows and columns inputted using the Set methods
	SetNumberRows(Nrow);
	SetNumberColumns(Ncolumn);

	//Creates a 1D vector for the hit data
	std::vector<G4int> iHitDataMatrix(Nrow*Ncolumn, 0);
	SetHitData(iHitDataMatrix);
}

void Data::SetUpEnergyData()
{
	//Works out the number of detectors for the energy data
	G4int EnergyColumns = GetNumberRows() * GetNumberColumns();

	//Creates a 2D vector for the energy data	
	std::vector<std::vector<G4int> > iEnergyMatrix(EnergyColumns, std::vector<G4int>(GetNoBins(),0));
	SetEnergyData(iEnergyMatrix);
}

void Data::SaveHitData(G4int DetectorNumber)
{	
	//Finds the coordinates of the detector inputted for the matrix
	//+1 for each hit. Written on one line to increase speed
	
	++HitDataMatrix[DetectorNumber]; 
}

void Data::PrintHitData()
{
	//Find each needed value using the Get functions
	std::vector<G4int> HitData = GetHitData();

	//Prints the 1D Data as 2D	
	G4cout << G4endl << "Hit count data: " << "Image " << GetCurrentImage()+1 << G4endl << G4endl;
	G4cout << "[[";

	for (G4int Element = 0; Element < GetNumberRows()*GetNumberColumns(); Element++)
	{
		G4int Column = Remainder(Element, GetNumberColumns());	
		G4int Row = Quotient(Element, GetNumberColumns()); 

		if (Column >= GetNumberColumns()-1)
		{
			G4cout << std::setfill(' ') << std::setw(5) << HitData[Element];
			if (Column == GetNumberColumns()-1 && Row == GetNumberRows()-1)
				{G4cout << "]]" << G4endl;}
			else
				{G4cout << "]," << G4endl << " [";}
		}
		else 
		{
			G4cout << std::setfill(' ') << std::setw(5) << HitData[Element];
			G4cout << ",";
		}
	}
}

G4int Data::BinNumber(G4int DetectorNumber, G4double edep)
{
	//Calculates the size of each bin
	//floor function calculates which bin the energy should be placed into
	return floor(edep/(GetMaxEnergy()/GetNoBins()));
}

void Data::SaveEnergyData(G4int DetectorNumber, G4double edep)
{
	G4int Bin = BinNumber(DetectorNumber, edep);
	//If detectors are 100% efficient (vacuum), then energy is energy of photon and will fit outside bin size. Adjusts it to max energy bin
	if (Bin == GetNoBins())
		{++EnergyMatrix[DetectorNumber][Bin-1];}
	else
		{//+1 to the energy bin
		 ++EnergyMatrix[DetectorNumber][Bin];}
}

void Data::PrintEnergyData()
{
	//Gets the needed variables
	std::vector<std::vector<G4int> > EnergyData = GetEnergyData();
	G4int NoDigits = std::to_string(GetNumberRows()*GetNumberColumns()).length();

	G4cout << G4endl << "Image " << GetCurrentImage()+1 << G4endl;
	G4cout << G4endl << "Energy detector data (keV)" << G4endl;
	G4cout << "          "; 
	
	//Prints out the energy range of the bins
	for (G4int n = 0; n < NoDigits; n++)
		{G4cout << " ";}	
	for (G4int x = 0 ; x < GetNoBins(); x++)
	{	G4cout << "   "; 
		G4cout << std::setfill(' ') << std::setw(5) << (GetMaxEnergy()/GetNoBins() * (x+1))*1000;
	}
	G4cout << G4endl;

	//Prints out the energy data
	for(G4int x = 0 ; x < GetNumberRows()*GetNumberColumns(); x++)  
    	{	for( G4int y = 0 ; y < GetNoBins(); y++)  
        	{	if (y == 0)
				{G4cout << "Detector " << std::setfill('0') << std::setw(NoDigits) << x << ":";}
			G4cout << "   " << std::setfill(' ') << std::setw(5) << EnergyData[y][x] ;
        	}
    		G4cout << G4endl;  
    	}
}

void Data::WriteToTextFile(G4String SimulationData)
{
	std::ofstream outdata; 
   
	//Get needed variables to wrtie to file
   	std::vector<G4int> HitData = GetHitData();
   	std::vector<std::vector<G4int> > EnergyData = GetEnergyData();
	G4int TotalNoRows = GetNumberRows();
	G4int TotalNoColumns = GetNumberColumns();
	G4int CImage = GetCurrentImage();

	G4String FilePath = "./Data_Output/Text/";

	//Save the simulation settings to a textfile only once
	if (SimulationData == "SimulationSettings")
	{
		G4String SettingsName = "SimulationSettings.txt";
		G4ThreeVector DetectorDimensions = GetDetectorDimensions()*2;

		outdata.open(FilePath+SettingsName); 
   	
		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: file could not be opened" << std::endl;
      			exit(1);
   		}

		//Save information about the conditions used for the simulation
		outdata << "Simulation of X-Ray data." << std::endl << std::endl;

		outdata << "Conditioons used in this simulation: " << std::endl;
		outdata << "- Physics package: " << GetPhysicsUsed() << std::endl;
		outdata << "- Seed: " << GetSeedOption() << std::endl << std::endl;

		outdata << "- Intial energy of the monochromatic beam: " << G4BestUnit(GetMaxEnergy(),"Energy") << std::endl;
		outdata << "- Beam dimensions: " << GetBeamWidth() << " x " << G4BestUnit(GetBeamHeight(), "Length") << std::endl << std::endl; 

		outdata << "- Number of detectors: " << TotalNoRows << " x " << TotalNoColumns << std::endl;
		outdata << "- Detector dimensions: " << G4BestUnit(DetectorDimensions.x(), "Length") << "x " << G4BestUnit(DetectorDimensions.y(), "Length") << "x " << G4BestUnit(DetectorDimensions.z(), "Length") << std::endl;
		if (GetDetectorEfficiency() == true)
			{outdata << "- Detectors used are 100%\ efficient" << std::endl;}
		else 
			{outdata << "- Detector material: " << GetDetectorMaterial() << std::endl;}

		outdata << std::endl <<"- Number of photons used per image: " << GetNoPhotons() << std::endl;
		outdata << "- Number of images: " << GetNoImages() << std::endl;
		outdata << "- Number of bins: " << GetNoBins() << std::endl << std::endl;

		outdata << "- Real simulation time: " << GetSimulationTime() << "s" << std::endl;
		outdata << std::endl;

		outdata.close();
	}

//--------------------------------------------------------------------------------------

	else 
	{
		G4cout << G4endl << "Saving the data... ";
		//Save the HitData
		G4String HitFileName = "HitDataFile.txt";

		//If it is the first image, override the file. If not, append to the file 
		if (CImage != 0)
			{outdata.open(FilePath+HitFileName, std::ofstream::app); }
		else
			{outdata.open(FilePath+HitFileName);} 
   	
		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: file could not be opened" << std::endl;
      			exit(1);
   		}

		//Save each image in the text file as a matrix
		outdata << std::endl << "Image: " << CImage+1 << std::endl << std::endl;
		outdata << "[[";

		for (G4int Element = 0; Element <TotalNoColumns*TotalNoRows; Element++)
		{
			G4int Column = Remainder(Element, TotalNoColumns);	
			G4int Row = Quotient(Element, TotalNoColumns); 

			if (Column >= TotalNoColumns-1)
			{
				outdata << std::setfill(' ') << std::setw(5) << HitData[Element];
				if (Column == TotalNoColumns-1 && Row == TotalNoRows-1)
					{outdata << "]]" << std::endl;}
				else
					{outdata << "]," << std::endl << " [";}
			}
			else 
			{
				outdata << std::setfill(' ') << std::setw(5) << HitData[Element];
				outdata << ",";
			}
		}

		outdata.close();
		G4cout << G4endl << "The data has been successfully written to " << FilePath << HitFileName << G4endl << G4endl;

//--------------------------------------------------------------------------------------

		//Save the EnergyData
		G4String EnergyFileName = "EnergyFile.txt";
	
		//If it is the first image, override the file. If not, append to the file 
		if (CImage != 0)
			{outdata.open(FilePath+EnergyFileName, std::ofstream::app); }
		else
			{outdata.open(FilePath+EnergyFileName);} 

		outdata << G4endl << "Energy detector data (keV)" << G4endl;

		//Finds how many digits the detector numbers need to be to keep aligned
		G4int NoDigits = std::to_string(TotalNoRows*TotalNoColumns).length();

		outdata << G4endl << "Image: " << CImage+1 << G4endl;
		
		outdata << std::endl;
		outdata << "Detector Number:  ";

		for (G4int Number = 0; Number < TotalNoColumns*TotalNoRows; Number++)
			{outdata << std::setfill(' ') << std::setw(NoDigits+10) << Number;}

		outdata << std::endl;

		for(G4int yE = 0 ; yE < GetNoBins(); yE++)  
    		{	if( yE == GetNoBins()-1)
				{outdata << "≤";}
			else
				{outdata << "<" ;}
			outdata << std::setfill(' ') << std::setw(NoDigits+10) << G4BestUnit((GetMaxEnergy()/GetNoBins()) * (yE+1), "Energy");

			for(G4int xE = 0 ; xE < TotalNoColumns*TotalNoRows; xE++)  
    		    	{	
				outdata << std::setfill(' ') << std::setw(NoDigits+10) << EnergyData[xE][yE];
       		 	}
    			outdata << std::endl;
    		}

		outdata.close();
		G4cout << "The data has been successfully written to " << FilePath << EnergyFileName << G4endl << G4endl;
	
		//Reset the data to zero ready for the next image
		memset(&HitData[0], 0, sizeof(HitData[0]) * HitData.size());
		for(auto& x : EnergyData) memset(&x[0],0,sizeof(int)*x.size());
		SetHitData(HitData);
		SetEnergyData(EnergyData);
	}
}

void Data::WriteToHDF5()
{
	
}
