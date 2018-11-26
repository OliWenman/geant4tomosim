#ifndef Data_h
#define Data_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UserLimits.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>       // Used to access the function floor 
#include "G4SystemOfUnits.hh"
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

		void SetUpData(int nDetectorsY, int nDetectorsZ);
	
		//void SetUpHitData(int Nrow, int Ncolumn);
		//void SetUpEnergyData();

		inline void SaveHitData(G4int DetectorNumber){++HitDataArray[DetectorNumber];}

		void SaveEnergyData(G4int DetectorNumber, G4double edep);

		void MakeSpaces(int nSpaces, std::ofstream &outdata);
		void WriteToTextFile();
		void WriteToHDF5();

		//Get Methods
		std::vector<int> GetHitData(){return HitDataArray;}
		std::vector<std::vector<int> > GetEnergyData(){return EnergyMatrix;}

		G4int GetNoImages(){return NoImages_Cmd;}

		G4int GetNoBins(){return NoBins_Cmd;}
		G4bool GetEnergyDataOption(){return EnergyDataCmd;}
		G4double GetMaxEnergy(){return MaxE;}

		//Set Methods
		void SetHitData(std::vector<int> UpdatedData){HitDataArray = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<int> >  UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}

		void SetNoImages(G4int value){NoImages_Cmd = value;}
		
		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetEnergyDataOption(G4bool value){EnergyDataCmd = value;}
		void SetMaxEnergy(G4double value){MaxE = value;}

		void SetTextFileCmd(G4bool value){TextFileCmd = value;}
		void SetHDF5FileCmd(G4bool value){HDF5FileCmd = value;}

	private:	
		DataMessenger* dataMessenger;

		//Data members
		std::vector<int> HitDataArray;
		std::vector<std::vector<int> > EnergyMatrix;

		G4int rows;
		G4int columns;	

		G4int nImage;

		G4bool TextFileCmd;
		G4bool HDF5FileCmd;

		G4int NoBins_Cmd;
		G4int NoImages_Cmd;
		G4bool EnergyDataCmd;
		G4double MaxE;
};

#endif

		
