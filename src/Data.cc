#include "Data.hh"
#include "DataMessenger.hh"
#include <iomanip>
#include <fstream>
#include "G4UnitsTable.hh"
//#include <string.h>
#include <vector>
#include "G4ThreeVector.hh"
#include <string> 

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
	G4cout << G4endl << "Number of particles per detector on average is " << double(GetNoPhotons()/(Nrow*Ncolumn)) << G4endl; 

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
	//+1 for each hit.
	
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
	G4int TotalNoRows = GetNumberRows();
	G4int TotalNoColumns = GetNumberColumns();

	G4cout << G4endl << "Image " << GetCurrentImage()+1 << G4endl;
	G4cout << G4endl << "Energy detector data (keV)" << G4endl;
	
	//Finds how many digits the detector numbers need to be to keep aligned
	G4int NoDigits = std::to_string(TotalNoRows*TotalNoColumns).length();

	G4cout << G4endl << "Image: " << GetCurrentImage()+1 << G4endl;
		
	G4cout << G4endl;
	G4cout << "Detector Number:  ";

	for (G4int Number = 0; Number < TotalNoColumns*TotalNoRows; Number++)
		{G4cout << std::setfill(' ') << std::setw(NoDigits+10) << Number;}

	G4cout << G4endl;

	for(G4int yE = 0 ; yE < GetNoBins(); yE++)  
    	{	if( yE == GetNoBins()-1)
			{G4cout << "≤";}
		else
			{G4cout << "<" ;}
		G4cout << std::setfill(' ') << std::setw(NoDigits+10) << G4BestUnit((GetMaxEnergy()/GetNoBins()) * (yE+1), "Energy");

		for(G4int xE = 0 ; xE < TotalNoColumns*TotalNoRows; xE++)  
    		{	
			G4cout << std::setfill(' ') << std::setw(NoDigits+10) << EnergyData[xE][yE];
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
		G4ThreeVector DetDimensions = GetDetectorDimensions()*2;

		outdata.open(FilePath+SettingsName); 
   	
		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: file could not be opened" << std::endl;
      			exit(1);
   		}

		//Save information about the conditions used for the simulation
		outdata << "Simulation of X-ray data." << std::endl << std::endl;

		outdata << "Conditioons used in this simulation: " << std::endl;
		outdata << "- Physics package: " << GetPhysicsUsed() << std::endl;
		outdata << "- Seed: " << GetSeedOption() << std::endl << std::endl;

		outdata << "- Intial energy of the monochromatic beam: " << G4BestUnit(GetMaxEnergy(),"Energy") << std::endl;
		outdata << "- Beam dimensions: " << GetBeamWidth() << " x " << G4BestUnit(GetBeamHeight(), "Length") << std::endl << std::endl; 

		outdata << "- Number of detectors: " << TotalNoRows << " x " << TotalNoColumns << std::endl;
		outdata << "- Detector dimensions: " << G4BestUnit(DetDimensions.x(), "Length") << "x " << G4BestUnit(DetDimensions.y(), "Length") << "x " << G4BestUnit(DetDimensions.z(), "Length") << std::endl;
		if (GetDetectorEfficiency() == true)
			{outdata << "- Detectors used are 100%\ efficient" << std::endl;}
		else 
			{outdata << "- Detector material: " << GetDetectorMaterial() << std::endl;}

		outdata << std::endl << "- Number of photons used per image: " << GetNoPhotons() << std::endl;
		outdata << std::endl << "- Number of photons per detector on average is (assuming total number of detectors are the same dimensions as the beam): " << GetNoPhotons()/(TotalNoColumns*TotalNoRows) << std::endl;
		outdata << "- Number of images: " << GetNoImages() << std::endl;
		if (GetEnergyDataOption() == true)
			{outdata << "- Number of bins: " << GetNoBins() << std::endl << std::endl;}
		else 
			{outdata << "- Energy data was not recorded" << std::endl << std::endl;}

		outdata << "- Real simulation time: " << GetSimulationTime() << "s" << std::endl;
		outdata << std::endl;

		outdata.close();
	}

//--------------------------------------------------------------------------------------

	else 
	{
		G4cout << G4endl << "Saving the data... ";
		
		std::string ImageNumberString = std::to_string(CImage+1);

		//Save the HitData
		G4String HitFileName = "Image" + ImageNumberString + ".txt";

   		outdata.open(FilePath+"HitData/"+HitFileName);
		
		//Output error if can't open file
		if( !outdata ) 
		{ 	std::cerr << "Error: file could not be opened" << std::endl;
      			exit(1);
   		}

		//Finds how many digits the detector numbers need to be to keep aligned
		G4int NDigits = std::to_string(HitData[0]).length() + 2 ;
		
		//Save each image in the text file as a matrix
		for (G4int Element = 0; Element < TotalNoColumns*TotalNoRows; Element++)
		{
			G4int Column = Remainder(Element, TotalNoColumns);	
			G4int Row = Quotient(Element, TotalNoColumns); 

			outdata << std::setfill(' ') << std::setw(NDigits) << HitData[Element];

			if (Column >= TotalNoColumns-1)
				{outdata << std::endl;}
		}

		outdata.close();
		G4cout << G4endl << "The data has been successfully written to " << FilePath << HitFileName << G4endl << G4endl;

//--------------------------------------------------------------------------------------
		if (GetEnergyDataOption() == true)
		{
			G4String EnergyFileName = "EnergyDataFile" + ImageNumberString + ".txt";

			outdata.open(FilePath+"EnergyData/"+EnergyFileName);

			//Output error if can't open file
			if( !outdata ) 
			{ 	std::cerr << "Error: file could not be opened" << std::endl;
      				exit(1);
   			}

			G4int NBins = GetNoBins();
			G4double MaxEnergy = GetMaxEnergy();

			//Finds how many digits the numbers needed to be to keep the columns aligned
			NDigits = std::to_string(TotalNoRows*TotalNoColumns).length() + 1;
			G4int NDigitsBinHits = std::to_string(EnergyData[0][NBins]).length();
			if (NDigitsBinHits > NDigits)
				{NDigits = NDigitsBinHits + 1;}
		
			outdata << "Detector No: ";

			for (G4int Number = 0; Number < TotalNoColumns*TotalNoRows; Number++)
				{outdata << std::setfill(' ') << std::setw(NDigits) << Number;}

			outdata << std::endl;

			for(G4int EnergyBin = 0 ; EnergyBin < NBins; EnergyBin++)  
    			{	if( EnergyBin == NBins-1)
					{outdata << "≤";}
				else
					{outdata << "<" ;}
				outdata << std::setfill(' ') << std::setw(NDigits) << G4BestUnit((MaxEnergy/NBins) * (EnergyBin+1), "Energy");

				for(G4int NDetector = 0 ; NDetector < TotalNoColumns*TotalNoRows; NDetector++)  
    		    		{	
					outdata << std::setfill(' ') << std::setw(NDigits) << EnergyData[NDetector][EnergyBin];
       		 		}
    				outdata << std::endl;
    			}

			outdata.close();
			G4cout << "The data has been successfully written to " << FilePath << EnergyFileName << G4endl << G4endl;

			for(auto& x : EnergyData) memset(&x[0],0,sizeof(int)*x.size());
			SetEnergyData(EnergyData);
		}
	
		//Reset the data to zero ready for the next image
		memset(&HitData[0], 0, sizeof(HitData[0]) * HitData.size());
		SetHitData(HitData);
	}
}

void Data::WriteToHDF5()
{
	
}
