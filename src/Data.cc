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
	//G4cout << G4endl << "Number of photons is " << GetNoPhotons() << G4endl;
	//G4cout << G4endl << "Number of particles per detector on average is " << double(GetNoPhotons()/(Nrow*Ncolumn)) << G4endl; 

	//Creates a 1D vector for the hit data
	//std::vector<int> iHitDataMatrix(Nrow*Ncolumn, 0);
	//SetHitData(iHitDataMatrix);

	HitDataArray= new int[Nrow*Ncolumn];
	
	//std::array<int, Nrow*Ncolumn > HitDataArray;//(Nrow*Ncolumn, 0);
	//SetHitData(iHitDataArray);

}

void Data::SetUpEnergyData()
{
	//Works out the number of detectors for the energy data
	G4int EnergyColumns = GetNumberRows() * GetNumberColumns();

	//Creates a 2D vector for the energy data	
	std::vector<std::vector<G4int> > iEnergyMatrix(EnergyColumns, std::vector<G4int>(GetNoBins(),0));
	SetEnergyData(iEnergyMatrix);
}

/*void Data::SaveHitData(G4int DetectorNumber)
{	
	//Finds the coordinates of the detector inputted for the matrix
	//+1 for each hit.
	
	++HitDataArray[DetectorNumber]; 
}*/

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
	std::ofstream outdata; 
   
	//Get needed variables to wrtie to file
   	//std::vector<int> HitData = GetHitData();
	//std::array<int> HitData = GetHitData();
	G4int TotalNoRows = GetNumberRows();
	G4int TotalNoColumns = GetNumberColumns();
	G4int CImage = GetCurrentImage();

	G4String FilePath = "./Data_Output/Text/";

	G4cout << G4endl << "Saving the data... ";
		
	std::string ImageNumberString = std::to_string(CImage+1);

	//Save the HitData
	G4String HitFileName = "Image" + ImageNumberString + ".txt";

   	outdata.open(FilePath+"HitData/"+HitFileName);
		
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "Error: " << HitFileName << " file could not be opened" << std::endl;
      		exit(1);
   	}

	//Finds how many digits the detector numbers need to be to keep aligned
	G4int NDigits = std::to_string(HitDataArray[0]).length() + 2 ;
		
	//Save each image in the text file as a matrix
	for (G4int Element = 0; Element < TotalNoColumns*TotalNoRows; Element++)
	{
		//G4int Column = Remainder(Element, TotalNoColumns);	
		//G4int Row = Quotient(Element, TotalNoColumns); 

		outdata << std::setfill(' ') << std::setw(NDigits) << HitDataArray[Element];

		if (Remainder(Element, TotalNoColumns) >= TotalNoColumns-1)
			{outdata << std::endl;}
	}

	outdata.close();
	G4cout << G4endl << "The data has been successfully written to " << FilePath << HitFileName << G4endl << G4endl;

//--------------------------------------------------------------------------------------
	if (GetEnergyDataOption() == true)
	{
		std::vector<std::vector<G4int> > EnergyData = GetEnergyData();

		G4String EnergyFileName = "EnergyDataFile" + ImageNumberString + ".txt";

		outdata.open(FilePath+"EnergyData/"+EnergyFileName);

		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: " << EnergyFileName << " file could not be opened" << std::endl;
      			exit(1);
   		}

		G4int NBins = GetNoBins();
		G4double MaxEnergy = GetMaxEnergy();

		//Finds how many digits the numbers needed to be to keep the columns aligned
		G4int NDigitsDetectors = std::to_string(TotalNoRows*TotalNoColumns).length() + 1;
		G4int NDigitsBinHits = std::to_string(EnergyData[0][NBins-1]).length() + 1;

		G4int Spacing;

		//Finds which spacing is bigger to keep aligned 
		if (NDigitsDetectors >= NDigitsBinHits)
			{Spacing = NDigitsDetectors;}
		else
			{Spacing =  NDigitsBinHits;}

		
		G4String String = "Detector No: ";
		outdata << String;

		G4int StringLength = String.length();

		G4int EnergyNumberLength = std::to_string(1/MaxEnergy).length();

		int Difference = EnergyNumberLength + 5 - StringLength;
		if (EnergyNumberLength + 5 > StringLength)
		{
			MakeSpaces(Difference, outdata);
		}
			

		for (G4int Number = 0; Number < TotalNoColumns*TotalNoRows; Number++)
			{outdata << std::setfill(' ') << std::setw(Spacing) << Number;}

		outdata << std::endl;

		for(G4int EnergyBin = 0 ; EnergyBin < NBins; EnergyBin++)  
    		{	if( EnergyBin == NBins-1)
				{outdata << "≤";}
			else
				{outdata << "<";}

			MakeSpaces(Difference, outdata);
			outdata <<  std::setfill(' ') << std::setw(Spacing) << G4BestUnit((MaxEnergy/NBins) * (EnergyBin+1), "Energy");

			for(G4int NDetector = 0 ; NDetector < TotalNoColumns*TotalNoRows; NDetector++)  
    		    	{	
				MakeSpaces(Difference, outdata);
				outdata << std::setfill(' ') << std::setw(Spacing) << EnergyData[NDetector][EnergyBin];
       		 	}
    			outdata << std::endl;
    		}

		outdata.close();
		G4cout << "The data has been successfully written to " << FilePath << EnergyFileName << G4endl << G4endl;

		for(auto& x : EnergyData) memset(&x[0],0,sizeof(int)*x.size());
		SetEnergyData(EnergyData);
	}
	
	//Reset the data to zero ready for the next image
	memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * GetNumberRows()*GetNumberColumns());
	//SetHitData(HitData);
}

void Data::WriteToHDF5()
{
	
}
