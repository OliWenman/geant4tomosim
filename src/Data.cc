#include "Data.hh"
//#include "DataMessenger.hh"
#include <iomanip>
#include <fstream>
#include "G4UnitsTable.hh"
//#include <string.h>
#include <vector>
#include <array>
#include "G4ThreeVector.hh"
#include <string> 

//#include "hdf5.h"
//#include "H5Cpp.h"

//using namespace H5;

Data::Data()
{
	G4cout << G4endl << "Data class has been created "<< G4endl;
	//dataMessenger = new DataMessenger(this);
}

Data::~Data()
{
	G4cout << G4endl << "Data class has been deleted ";
	//delete dataMessenger;
}

void Data::SetUpHitData(G4int Nrow, G4int Ncolumn)
{
	//Saves the rows and columns inputted using the Set methods
	SetNumberRows(Nrow);
	SetNumberColumns(Ncolumn);

	//Creates a 1D vector for the hit data
	//std::vector<int> iHitDataMatrix(Nrow*Ncolumn, 0);
	//SetHitData(iHitDataMatrix);

	HitDataArray= new int[Nrow*Ncolumn];
	
	//std::array<int, Nrow*Ncolumn > HitDataArray;//(Nrow*Ncolumn, 0);
	//SetHitData(iHitDataArray);

}

void Data::SetUpEnergyData()
{
	//Creates a 2D vector for the energy data	
	std::vector<std::vector<G4int> > iEnergyMatrix(rows * columns, std::vector<G4int>(NoBins_Cmd,0));
	SetEnergyData(iEnergyMatrix);
}

G4int Data::BinNumber(G4int DetectorNumber, G4double edep)
{
	//Calculates the size of each bin
	//floor function calculates which bin the energy should be placed into
	return floor(edep/(MaxE/NoBins_Cmd));
}

void Data::SaveEnergyData(G4int DetectorNumber, G4double edep)
{
	G4int Bin = BinNumber(DetectorNumber, edep);
	//If detectors are 100% efficient (vacuum), then energy is energy of photon and will fit outside bin size. Adjusts it to max energy bin
	if (Bin == NoBins_Cmd)
		{++EnergyMatrix[DetectorNumber][Bin-1];}
	else
		{//+1 to the energy bin
		 ++EnergyMatrix[DetectorNumber][Bin];}
}

void Data::MakeSpaces(int nSpaces, std::ofstream &outdata)
{
	if (nSpaces > 0)
	{
		for (int i = 0; i < nSpaces ; ++i)
			{outdata << " " ;}
	}
}

void Data::WriteToTextFile()
{
	//Creation of the data stream
	std::ofstream outdata; 
   
	G4int CImage = GetCurrentImage();

	G4String FilePath = "./Data_Output/Text/";

	G4cout << G4endl << "Saving the hit data... ";
		
	//File name is dependent of the image number
	std::string ImageNumberString = std::to_string(CImage+1);

	//Create the File name string
	G4String HitFileName = "Image" + ImageNumberString + ".txt";

	//Opens the data stream
   	outdata.open(FilePath+"HitData/"+HitFileName);
		
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "Error: " << HitFileName << " file could not be opened" << std::endl;
      		exit(1);
   	}

	//Finds how many digits the detector numbers need to be to keep aligned
	G4int NDigits = std::to_string(HitDataArray[0]).length() + 2 ;
		
	//Saves the 1D array as a 2D array
	for (G4int Element = 0; Element < columns*rows; Element++)
	{
		outdata << std::setfill(' ') << std::setw(NDigits) << HitDataArray[Element];

		if (Remainder(Element, columns) >= columns-1)
			{outdata << std::endl;}
	}

	//Let user know data has been saved successfully
	outdata.close();
	G4cout << G4endl << "The data has been successfully written to " << FilePath << HitFileName << G4endl << G4endl;

//--------------------------------------------------------------------------------------
	if (EnergyDataCmd == true)
	{
		G4cout << "Saving the energy data... " << G4endl;
		
		double MaxEnergy = GetMaxEnergy()*1000;

		//Filename
		G4String EnergyFileName = "EnergyDataFile" + ImageNumberString + ".txt";

		//Opens the data stream
		outdata.open(FilePath+"EnergyData/"+EnergyFileName);

		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: " << EnergyFileName << " file could not be opened" << std::endl;
      			exit(1);
   		}
		
		
		G4String DetectorString = "Detector No: ";
		G4String EnergyString =   "Energy(keV): ";

		//Finds how many detectors there are and its string length
		int NDigitsDetectors = std::to_string(columns*rows).length();

		//Finds how many hits in the final bin for the first detector to find its string length
		int NDigitsBinHits = std::to_string(EnergyMatrix[0][NoBins_Cmd-1]).length();

		//Finds the energy of the first column and its string length
		int NDigitsEnergyColumnF = std::to_string((NoBins_Cmd/MaxEnergy)).length();

		//Finds the energy of the last column and its string length
		int NDigitsEnergyColumnL = std::to_string(MaxEnergy).length();

		int NDigitsEnergyColumn;

		if(NDigitsEnergyColumnF > NDigitsEnergyColumnL)
			{NDigitsEnergyColumn = NDigitsEnergyColumnF;}
		else
			{NDigitsEnergyColumn = NDigitsEnergyColumnL;}

		int LeftSpacing = NDigitsDetectors + 2;

		int Spacing;

		if(NDigitsBinHits > NDigitsEnergyColumn)
		{
			Spacing = NDigitsBinHits + 1;
		}
		else
		{
			Spacing = NDigitsEnergyColumn + 1;
		}

		outdata << EnergyString << " "; MakeSpaces(LeftSpacing, outdata);

		for( int nEnergy = 0; nEnergy < NoBins_Cmd ; nEnergy++)
		{
			outdata << std::setfill(' ') << std::setw(Spacing - 1) << ( MaxEnergy/NoBins_Cmd) * (nEnergy+1) << " ";
		} 

		outdata << std::endl;

		for(G4int NDetector = 0 ; NDetector < columns*rows; NDetector++)  
    		{	
			outdata << DetectorString << std::setfill(' ') << std::setw(LeftSpacing) << NDetector;

			for(G4int EnergyBin = 0 ; EnergyBin < NoBins_Cmd; EnergyBin++)  
    			{	
				outdata << std::setfill(' ') << std::setw(Spacing) << EnergyMatrix[NDetector][EnergyBin];
			}
			outdata << std::endl;
		}



		outdata.close();
		G4cout << "The data has been successfully written to " << FilePath << EnergyFileName << G4endl << G4endl;

		for(auto& x : EnergyMatrix) memset(&x[0],0,sizeof(int)*x.size());
	}
	
	//Reset the data to zero ready for the next image
	memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * columns*rows);
}

void Data::WriteToHDF5()
{
	
}
