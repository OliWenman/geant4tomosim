#include "Data.hh"
#include "DataMessenger.hh"
#include <iomanip>

Data::Data()
{
    dataMessenger = new DataMessenger(this);
}

Data::~Data()
{
    delete dataMessenger;
}

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
			if (fluoreFullField)
			{
				std::vector<int> ifluorescence (NoBins_Cmd, 0);
				fluorescence = ifluorescence;
			}
			
			if (fluoreFullMapping)
			{
				std::vector<std::vector<std::vector<int> > > ifullMappingFluore (nDetectorsY, std::vector<std::vector<int> >
				                                                                (nDetectorsZ, std::vector<int>
				                                                                (NoBins_Cmd)));
				fullMappingFluore = ifullMappingFluore;
			}
			
			if (diffraction)
			{
			    int beamPosX = nDetectorsY;
			    int beamPosY = nDetectorsZ;
			    std::vector<std::vector<std::vector<std::vector<int> > > > idiffractionData (beamPosX, std::vector<std::vector<std::vector<int> > >
			                                                                                (beamPosY, std::vector<std::vector<int> >
			                                                                                (nDetectorsY, std::vector <int>
			                                                                                (nDetectorsZ))));
			    diffractionData = idiffractionData;
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

		if (fluoreFullField == true){
			memset(&fluorescence[0], 0, sizeof(fluorescence[0]) * NoBins_Cmd);}

		if (fluoreFullMapping == true){
		    std::fill(fullMappingFluore.begin(), fullMappingFluore.end(), std::vector<std::vector<int> > (nDetectorsZ, std::vector<int>(NoBins_Cmd)));}
	}
}

void Data::SaveFluorescence(double E)//
{
    int bin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (bin > NoBins_Cmd - 1) {bin = NoBins_Cmd -1;}

	++fluorescence[bin];
}

void Data::SaveFullMapping(double E)
{	
	//G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2) + 0.5*rows);
	//G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2) + 0.5*columns); 

    G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2/rows) + 0.5*rows);
	G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2/columns) + 0.5*columns);
	
	G4int eBin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (eBin > NoBins_Cmd - 1) {eBin = NoBins_Cmd -1;}

	++fullMappingFluore[xBin][yBin][eBin];
}
