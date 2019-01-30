#include "Data.hh"
#include <iomanip>
#include <fstream>

Data::Data(){}

Data::~Data(){}

void Data::SetUpData(int nDetectorsY, int nDetectorsZ, int nImage)
{
	//Setups the data depending on which data the user wants
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
			if (fluoreFullField == true)
			{
				std::vector<int> iEnergyFreq (NoBins_Cmd, 0);
				EnergyFreq = iEnergyFreq;
			}
			if (fluoreFullMapping == true)
			{
				std::vector<std::vector<std::vector<int> > > ifullMappingFluore (nDetectorsY, std::vector<std::vector<int> >(nDetectorsZ, std::vector<int>(NoBins_Cmd)));
				fullMappingFluore = ifullMappingFluore;
			}

			//Setups the energy bins if any of the energy options are wanted
			if (fluoreFullField == true || fluoreFullMapping == true || beamEnergy == true)
			{ 
				std::vector<double> iEnergyBins (NoBins_Cmd, 0);
				int Energy = 0;
				for (int ele = 0 ; ele < NoBins_Cmd ; ele++)
				{
					++Energy;
					iEnergyBins[ele] = (MaxE/NoBins_Cmd)*Energy;
				}

				EnergyBins = iEnergyBins;
			}
		}
	}
	else 
	{
		//Reset the data to zero ready for the next image
		memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * columns * rows);

		if (fluoreFullField == true)
			//Resets the energy frequency to zero for the next image
			memset(&EnergyFreq[0], 0, sizeof(EnergyFreq[0]) * NoBins_Cmd);

		if (fluoreFullMapping == true)
			std::fill(fullMappingFluore.begin(), fullMappingFluore.end(), std::vector<std::vector<int> > (nDetectorsZ, std::vector<int>(NoBins_Cmd)));
	}
}

void Data::SaveEnergyFreq(double E)//
{
    int bin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (bin < 0)
        bin = 0;
    else if (bin > EnergyFreq.size())
        bin = EnergyFreq.size();

	++EnergyFreq[bin];
}

void Data::SaveFullMapping(double E)
{	
	G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2) + 0.5*rows);
	G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2) + 0.5*columns); 

	G4int eBin = floor(E*1000/(MaxE/NoBins_Cmd));
	if (eBin < 0)
	    eBin = 0;
    else if (eBin > fullMappingFluore.size())
        eBin = fullMappingFluore.size();

	++fullMappingFluore[xBin][yBin][eBin];
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
	}
}
