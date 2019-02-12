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
	else 
	{
		//Reset the data to zero ready for the next image
		memset(&HitDataArray[0], 0, sizeof(HitDataArray[0]) * columns * rows);

		if (fluoreFullField == true)
			//Resets the energy frequency to zero for the next image
			memset(&EnergyFreq[0], 0, sizeof(EnergyFreq[0]) * NoBins_Cmd);

		if (fluoreFullMapping == true){
		    std::fill(fullMappingFluore.begin(), fullMappingFluore.end(), std::vector<std::vector<int> > (nDetectorsZ, std::vector<int>(NoBins_Cmd)));}
	}
}

void Data::SaveEnergyFreq(double E)//
{
    int bin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (bin > NoBins_Cmd - 1)
        bin = NoBins_Cmd -1;

	++EnergyFreq[bin];
}

void Data::SaveFullMapping(double E)
{	
	G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2) + 0.5*rows);
	G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2) + 0.5*columns); 

	G4int eBin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (eBin > NoBins_Cmd - 1)
    {   eBin = NoBins_Cmd -1;}

	++fullMappingFluore[xBin][yBin][eBin];
}
