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
	nImage = 0;	
}

Data::~Data()
	{G4cout << G4endl << "Data class has been deleted ";}

void Data::SetUpHitData(int Nrow, int Ncolumn)
{
	//Saves the rows and columns inputted using the Set methods
	SetNumberRows(Nrow);
	SetNumberColumns(Ncolumn);

	//Creates a 1D vector for the hit data
	//std::vector<int> iHitDataMatrix(Nrow*Ncolumn, 0);
	//SetHitData(iHitDataMatrix);

	HitDataArray = new int[Nrow*Ncolumn];
}

void Data::SetUpEnergyData()
{
	//Creates a 2D vector for the energy data	
	std::vector<std::vector<int> > iEnergyMatrix(rows * columns, std::vector<int>(NoBins_Cmd,0));
	SetEnergyData(iEnergyMatrix);
}

void Data::SaveEnergyData(G4int DetectorNumber, G4double edep)
{
	//Calculates which bin the energy should be placed into, floor function rounds down to the nearest interger
	int nBin = floor(edep/(MaxE/NoBins_Cmd));

	//If detectors are 100% efficient (vacuum), then energy is energy of photon and will fit outside bin size. Adjusts it to max energy bin
	if (nBin == NoBins_Cmd)
		{&++EnergyMatrix[DetectorNumber][nBin-1];}
	else
		{//+1 to the energy bin
		 &++EnergyMatrix[DetectorNumber][nBin];}
}

void Data::MakeSpaces(int nSpaces, std::ofstream &outdata)
{
	//Function to create a number of spaces within the data stream
	if (nSpaces > 0)
	{	for (int i = 0; i < nSpaces ; ++i)
			{outdata << " " ;}
	}
}

void Data::WriteToTextFile()
{
	if (TextFileCmd == true)
	{
		//Creation of the data stream
		std::ofstream outdata; 

		G4String FilePath = "./Data_Output/Text/";

		G4cout << G4endl << "Saving the hit data... ";
		
		//File name is dependent of the image number
		std::string ImageNumberString = std::to_string(nImage+1);

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
			G4String EnergyFileName = "EnergyData" + ImageNumberString + ".txt";

			//Opens the data stream
			outdata.open(FilePath+"EnergyData/"+EnergyFileName);

			//Output error if can't open file
			if( !outdata ) 
			{ 	std::cerr << "Error: " << EnergyFileName << " file could not be opened" << std::endl;
      				exit(1);
   			}
		
			//String variables
			G4String DetectorString = "Detector:";
			G4String EnergyString =   "Energy(keV): ";

			int DetectorLength = DetectorString.length();

			//Finds how many detectors there are and its string length
			int NDigitsDetectors = std::to_string(columns*rows).length();

			//Finds how many hits in the final bin for the first detector to find its string length
			int NDigitsBinHits = std::to_string(EnergyMatrix[0][NoBins_Cmd-1]).length();

			//Finds the energy of the first column and its string length
			int NDigitsEnergyColumnF = std::to_string((NoBins_Cmd/MaxEnergy)).length();

			//Finds the energy of the last column and its string length
			int NDigitsEnergyColumnL = std::to_string(MaxEnergy).length();

			//Compares the first and last energy column for the largest string length
			int NDigitsEnergyColumn;
			if(NDigitsEnergyColumnF > NDigitsEnergyColumnL)
				{NDigitsEnergyColumn = NDigitsEnergyColumnF;}
			else
				{NDigitsEnergyColumn = NDigitsEnergyColumnL;}
	
			//Left Spacing size
			int LeftSpacing; 
			if (NDigitsDetectors > DetectorLength)
				{LeftSpacing = NDigitsDetectors;}
			else 
				{LeftSpacing = DetectorLength;}

			//Compares the string size of the number of hits in the bin and the energy to determine the spacing
			int Spacing;
			if(NDigitsBinHits > NDigitsEnergyColumn)
				{Spacing = NDigitsBinHits + 1;}
			else
				{Spacing = NDigitsEnergyColumn + 1;}

			//Outputs spaces that are equal to the leftspacing and then the energy title
			MakeSpaces(LeftSpacing, outdata); outdata << EnergyString << G4endl; 

			//Outputs the detector title and then (if needed) creates spaces to the right of it
			outdata << DetectorString; MakeSpaces(NDigitsDetectors - DetectorLength, outdata);

			//Prints the energy along the top row
			for( int nEnergy = 0; nEnergy < NoBins_Cmd ; nEnergy++)
				{outdata << std::setfill(' ') << std::setw(Spacing - 1) << ( MaxEnergy/NoBins_Cmd) * (nEnergy+1) << " ";} 

			//New line
			outdata << std::endl;

			//prints the hits of the energy aligned with the energy bin
			for(G4int NDetector = 0 ; NDetector < columns*rows; NDetector++)  
    			{	
				outdata << std::setfill(' ') << std::setw(LeftSpacing - 1) << NDetector;
				for(G4int EnergyBin = 0 ; EnergyBin < NoBins_Cmd; EnergyBin++)  
    					{outdata << std::setfill(' ') << std::setw(Spacing) << EnergyMatrix[NDetector][EnergyBin];}
				outdata << std::endl;
			}

			//Lets the user know that the data was saved successfully
			outdata.close();
			G4cout << "The data has been successfully written to " << FilePath << EnergyFileName << G4endl << G4endl;

			//Resets the energy data to zero for the next image
			for(auto& x : EnergyMatrix) memset(&x[0],0,sizeof(int)*x.size());
		}
	}
	
	//Reset the data to zero ready for the next image
	memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * columns*rows);

	++nImage;
}

void Data::WriteToHDF5()
{
	if (HDF5FileCmd == true)
	{

	}
}
