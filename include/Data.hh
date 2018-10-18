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
		
		//Main functions
		inline G4int Quotient(G4int dividend, G4int divisor ) {return dividend / divisor;}
		inline G4int Remainder(G4int dividend, G4int divisor) {return dividend % divisor;}

		void SetUpHitData(G4int Nrow, G4int Ncolumn);
		inline void SaveHitData(G4int DetectorNumber){&++HitDataArray[DetectorNumber];}

		void SetUpEnergyData();
		G4int BinNumber(G4int DetectorNumber, G4double edep);
		void SaveEnergyData(G4int DetectorNumber, G4double edep);

		void MakeSpaces(int nSpaces, std::ofstream &outdata);
		void WriteToTextFile();
		void WriteToHDF5();

		//Get Methods
		//std::vector<int> GetHitData(){ return HitDataMatrix;}
		//std::array<int> GetHitData(){ return HitDataArray;}
		std::vector<std::vector<G4int> > GetEnergyData(){return EnergyMatrix;}

		G4int GetNumberRows(){return rows;}
		G4int GetNumberColumns(){return columns;}
		G4int GetCurrentImage(){return CurrentImage;}

		//Set Methods
		//void SetHitData(std::vector<int> UpdatedData){HitDataMatrix = UpdatedData;}
		//void SetHitData(std::array<int> UpdatedData){HitDataArray = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<G4int> >  UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}

		//Commands
		G4int GetNoBins(){return NoBins_Cmd;}
		G4int GetNoImages(){return NoImages_Cmd;}
		G4bool GetTextFileCmd(){return TextFileCmd;}
		G4bool GetHDF5FileCmd(){return HDF5FileCmd;}
		G4bool GetEnergyDataOption(){return EnergyDataCmd;}
		G4double GetMaxEnergy(){return MaxE;}
		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetNoImages(G4int value){NoImages_Cmd = value;}
		void SetTextFileCmd(G4bool value){TextFileCmd = value;}
		void SetHDF5FileCmd(G4bool value){HDF5FileCmd = value;}
		void SetEnergyDataOption(G4bool value){EnergyDataCmd = value;}
		void SetMaxEnergy(G4double value){MaxE = value;}

	private:	
		DataMessenger* dataMessenger;

		//Data members
		//std::vector<int> HitDataMatrix;
		std::vector<std::vector<G4int> > EnergyMatrix;

		int* HitDataArray;

		G4int rows;
		G4int columns;	

		G4int CurrentImage;

		G4bool TextFileCmd;
		G4bool HDF5FileCmd;

		G4int NoBins_Cmd;
		G4int NoImages_Cmd;
		G4bool EnergyDataCmd;
		G4double MaxE;
};

#endif

		
