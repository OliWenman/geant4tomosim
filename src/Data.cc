#include "Data.hh"
#include "DataMessenger.hh"
#include <iomanip>

Data::Data()
{
    dataMessenger = new DataMessenger(this);
    
    fullfieldFluorescence = false;
    fullmappingFluorescence = false;
    fullmappingDiffraction = false;
}

Data::~Data()
{
    G4cout << "\nData::Deleting messenger" << std::flush;
    delete dataMessenger;
    G4cout << "\nData::Deleted" << std::flush;
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
		//G4cout << "\nCreating absorption data set..." << G4endl;
		//std::vector<int> iabsorptionData(nDetectorsY*nDetectorsZ, 0);
		//absorptionData = iabsorptionData;
		//G4cout << "Success!" << G4endl;
		
		if (NoBins_Cmd > 0)
		{
			if (fullfieldFluorescence)
			{
			    G4cout << "Creating full field fluorescence data set..." << G4endl;
				std::vector<int> ifluorescence (NoBins_Cmd, 0);
				fullfieldFluorescenceData = ifluorescence;
				//G4cout << "Success!" << G4endl;
			}
			
			if (fullmappingFluorescence)
			{
			    //G4cout << "Creating full mapping fluorescence data set..." << G4endl;
				std::vector<std::vector<std::vector<int> > > ifullMappingFluore (nDetectorsY, std::vector<std::vector<int> >
				                                                                           (nDetectorsZ, std::vector<int>
				                                                                           (NoBins_Cmd)));
				fullmappingFluorescenceData = ifullMappingFluore;
				//G4cout << "Success!" << G4endl;
			}
			
			if (fullmappingDiffraction)
			{
			    //G4cout << "Creating diffraction data set..." << G4endl;
			    int beamPosX = nDetectorsY;
			    int beamPosY = nDetectorsZ;
			    
			    std::vector<std::vector<int> > idiffractionData (nDetectorsY, std::vector <int>
			                                                    (nDetectorsZ));
			    diffractionData = idiffractionData;
			    //G4cout << "Success!" << G4endl;
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
		/*//Reset the data to zero ready for the next image
		memset(&absorptionData[0], 0, 
		       sizeof(absorptionData[0]) * columns * rows);
        */

		if (fullfieldFluorescence == true)
		{
			memset(&fullfieldFluorescenceData[0], 0, 
			       sizeof(fullfieldFluorescenceData[0]) * NoBins_Cmd);
	    }

		if (fullmappingFluorescence == true)
		{
		    std::fill(fullmappingFluorescenceData.begin(), 
		              fullmappingFluorescenceData.end(), 
		              std::vector<std::vector<int> > (nDetectorsZ, std::vector<int>(NoBins_Cmd)));
		}
	}
}

void Data::SaveFluorescence(double E)
{
    int bin = floor(E*1000/(MaxE/NoBins_Cmd) -1);
    if (bin > NoBins_Cmd - 1) {bin = NoBins_Cmd -1;}

	++fullfieldFluorescenceData[bin];
}

void Data::SaveFullMapping(double E)
{	
	//G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2) + 0.5*rows);
	//G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2) + 0.5*columns); 

    G4int xBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2/rows) + 0.5*rows -1);
	G4int yBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2/columns) + 0.5*columns -1);
	
	G4int eBin = floor(E*1000/(MaxE/NoBins_Cmd));
    if (eBin > NoBins_Cmd - 1) {eBin = NoBins_Cmd -1;}

    //if (eBin <= 1000)
    //{G4cout << "\nebin = " << eBin << G4endl;}


	++fullmappingFluorescenceData[xBin][yBin][eBin];
}

void Data::SaveDiffraction(int copyNum)
{
    //G4int xbeamBin = floor(ParticlePosition.y()/(halfDetectorDimensions.y()*2/rows) + 0.5*rows);
	//G4int ybeamBin = floor(ParticlePosition.z()/(halfDetectorDimensions.z()*2/columns) + 0.5*columns);
	
    G4int detectorxBin = Quotient(copyNum, columns);
    G4int detectoryBin = Remainder(copyNum, columns);
    
    ++diffractionData[detectorxBin][detectoryBin];
}
