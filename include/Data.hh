#ifndef Data_h
#define Data_h 1

#include <vector>

//Used to access the function floor 
#include <math.h>      
#include "globals.hh"
#include "G4ThreeVector.hh"

class DataMessenger;

class Data
{
	public:
		Data();
		~Data();
		
		//Class methods
		inline G4int Quotient(int dividend, int divisor ) {return dividend / divisor;}
		inline G4int Remainder(int dividend, int divisor) {return dividend % divisor;}

		void SetUpData(int nDetectorsY, int nDetectorsZ, int nImage);

        inline void SaveAbsorption(G4int DetectorNumber){/*++absorptionData[DetectorNumber];*/}
		void SaveFluorescence(double E);		
		void SaveFullMapping(double E);
        void SaveDiffraction(int copyNumber);

		//Get Methods
		std::vector<double>                          GetEnergyBins(){return EnergyBins;}
		std::vector<int>                             GetHitData(){return absorptionData;}
		std::vector<int>                             GetFluorescence(){return fullfieldFluorescenceData;}
		std::vector<std::vector<std::vector<int> > > GetFullMapping(){return fullmappingFluorescenceData;}
		std::vector<std::vector<int> >               GetDiffractionData(){return diffractionData;}

		G4int GetNoBins(){return NoBins_Cmd;}
		G4int GetNumberOfBins(){return NoBins_Cmd;}
		G4double GetMaxEnergy(){return MaxE;}
		
		G4int GetAbsorptionSizeType(){return sizeof(absorptionData[0]);}
		G4int GetFullMappingFluoreSizeType(){return sizeof(fullmappingFluorescenceData[0][0][0]);}
		G4int GetDiffractionSizeType(){return sizeof(diffractionData[0][0]);}
		
		bool GetFullMapping_Option(){return fullmappingFluorescence;}
		bool GetFluorescence_Option(){return fullfieldFluorescence;}
		bool DoFullMappingDiffraction() {return fullmappingDiffraction;}

		//Set Methods
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}

		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetMaxEnergy(G4double value){MaxE = value;}

		void SetHalfDetectorDimensions(G4ThreeVector value){halfDetectorDimensions = value;}
		void SetParticlePosition(G4ThreeVector value){ParticlePosition = value;}

		void SetFullFieldFluorescence(bool value)  {fullfieldFluorescence = value;}
		void SetFullMappingFluorescence(bool value){fullmappingFluorescence = value;}
		void SetFullMappingDiffraction(bool value) {fullmappingDiffraction = value;}

	private:	
	    DataMessenger* dataMessenger;

		//Data members
		std::vector<double>                          EnergyBins;
		std::vector<int>                             absorptionData;
		std::vector<int>                             fullfieldFluorescenceData;
		std::vector<std::vector<std::vector<int> > > fullmappingFluorescenceData;
		std::vector<std::vector<int> >               diffractionData;

		G4int rows;
		G4int columns;	

		G4int NoBins_Cmd;
		G4double MaxE;

		G4ThreeVector ParticlePosition;
		G4ThreeVector halfDetectorDimensions; 

        bool fullfieldFluorescence;
		bool fullmappingFluorescence;
		bool fullmappingDiffraction;		
};

#endif

		
