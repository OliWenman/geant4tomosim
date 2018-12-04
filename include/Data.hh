#ifndef Data_h
#define Data_h 1

#include <vector>

//Used to access the function floor 
#include <math.h>      
#include "globals.hh"

class Data
{
	public:
		Data();
		~Data();
		
		//Class methods
		//inline G4int Quotient(int dividend, int divisor ) {return dividend / divisor;}
		inline G4int Remainder(int dividend, int divisor) {return dividend % divisor;}

		void SetUpData(int nDetectorsY, int nDetectorsZ, int nImage);

		void SaveEnergyFreq(double E);//		
		inline void SaveHitData(G4int DetectorNumber){++HitDataArray[DetectorNumber];}

		void WriteToTextFile(int nImage);

		//Get Methods
		std::vector<int> GetHitData(){return HitDataArray;}
		std::vector<std::vector<int> > GetEnergyData(){return EnergyMatrix;}

		std::vector<double> GetEnergyBins(){return EnergyBins;}//
		std::vector<int> GetEnergyFreq(){return EnergyFreq;}//

		G4int GetNoBins(){return NoBins_Cmd;}
		G4double GetMaxEnergy(){return MaxE;}

		//Set Methods
		void SetHitData(std::vector<int> UpdatedData){HitDataArray = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<int> >  UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}

		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetMaxEnergy(G4double value){MaxE = value;}

	private:	
		void MakeSpaces(int nSpaces, std::ofstream &outdata);

		//Data members
		std::vector<int> HitDataArray;
		std::vector<double> EnergyBins;//
		std::vector<int> EnergyFreq;//

		std::vector<std::vector<int> > EnergyMatrix;

		G4int rows;
		G4int columns;	

		G4bool TextFileCmd;

		G4int NoBins_Cmd;
		G4double MaxE;
};

#endif

		
