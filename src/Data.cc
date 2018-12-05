#include "Data.hh"
#include <iomanip>
#include <fstream>

Data::Data(){}

Data::~Data(){}

void Data::SetUpData(int nDetectorsY, int nDetectorsZ, int nImage)
{
	if (nImage == 0)
	{
		//Saves the rows and columns inputted using the Set methods
		SetNumberRows(nDetectorsY);
		SetNumberColumns(nDetectorsZ);

		//Creates a 1D vector for the hit data
		std::vector<int> iHitDataArray(nDetectorsY*nDetectorsZ, 0);
		HitDataArray = iHitDataArray;
		if (NoBins_Cmd > 0)
		{
			std::vector<double> iEnergyBins (NoBins_Cmd, 0);
			std::vector<int> iEnergyFreq (NoBins_Cmd, 0);

			int Energy = 0;
			for (int ele = 0 ; ele < NoBins_Cmd ; ele++)
			{
				++Energy;
				iEnergyBins[ele] = (MaxE/NoBins_Cmd)*Energy;
			}

			EnergyBins = iEnergyBins;
			EnergyFreq = iEnergyFreq;
		}
	}
	else 
	{
		//Reset the data to zero ready for the next image
		memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * columns*rows);

		if (NoBins_Cmd > 0)
		{
			//Resets the energy frequency to zero for the next image
			memset(&EnergyFreq[0], 0, sizeof(EnergyFreq[0]) * NoBins_Cmd);
		}
	}
}

void Data::SaveEnergyFreq(double E)//
{
	++EnergyFreq[floor(E/(MaxE/NoBins_Cmd))-1];
}

void Data::MakeSpaces(int nSpaces, std::ofstream &outdata)
{
	//Function to create a number of spaces within the data stream
	if (nSpaces > 0)
	{	for (int i = 0; i < nSpaces ; ++i)
			{outdata << " " ;}
	}
}

void Data::WriteToTextFile(int nImage)
{
	if (TextFileCmd == true)
	{
		//Creation of the data stream
		std::ofstream outdata; 

		G4String FilePath = "./../Output/Text/";
		
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

		G4cout << G4endl << "Saving the hit data... ";

		//Finds how many digits the detector numbers need to be to keep aligned
		G4int NDigits = std::to_string(HitDataArray[0]).length() + 2 ;
		
		//Saves the 1D array as a 2D array
		for (G4int Element = 0; Element < columns*rows; Element++)
		{
			outdata << std::setfill(' ') << std::setw(NDigits) << HitDataArray[Element];
			if (Remainder(Element, columns) >= columns-1)
				{outdata << "\n";}
		}

		//Let user know data has been saved successfully
		outdata.close();
		G4cout << "\nThe data has been successfully written to " << FilePath << HitFileName << "\n \n";

//--------------------------------------------------------------------------------------
		if (NoBins_Cmd > 0)
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
		}
	}
}
