#ifndef Data_h
#define Data_h 1

#include "globals.hh"
#include <iostream>
#include <vector>
#include <math.h>       // Used to access the function floor 
#include "G4SystemOfUnits.hh"

class Data
{
	public:
		Data();
		virtual ~Data();
		
		//Get Methods
		std::vector<std::vector<std::vector<G4int> > > GetHitData(){ return HitDataMatrix;}
		std::vector <std::vector<G4int> > GetEnergyData(){return EnergyMatrix;}

		G4int GetHitElement(G4int x, G4int y){return HitDataMatrix[x][y][0];}	

		G4int GetNumberRows(){return rows;}
		G4int GetNumberColumns(){return columns;}
		G4int GetNumberBins(){return bins;}

		G4double GetMaxEnergy(){return MaxE;}

		G4int GetSeed(){return Seed;}
		G4int GetNumberOfPhotons(){return NumberOfPhotons;}
		G4int GetNumberOfImages(){return NumberOfImages;}
		G4int GetCurrentImage(){return CurrentImage;}

		G4String GetPhysicsUsed(){return PhysicsUsed;}

		G4double GetSimulationTime(){return SimulationTime;}

		//Set Methods
		void SetHitData(std::vector<std::vector<std::vector<G4int> > > UpdatedData){HitDataMatrix = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<G4int> >  UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}
		void SetNumberBins(G4int value){bins = value;}

		void SetMaxEnergy(G4double value){MaxE = value;}

		void SetSeed(G4int value){Seed = value;}
		void SetNumberOfPhotons(G4int value){NumberOfPhotons = value;}
		void SetNumberOfImages(G4int value){NumberOfImages = value;}
		void SetCurrentImage(G4int value){CurrentImage = value;}

		void SetPhysicsUsed(G4String value){PhysicsUsed = value;}

		void SetSimulationTime(G4double value){SimulationTime = value;}

		//Method functions
		G4int Quotient(G4int dividend, G4int divisor ) {return dividend / divisor;}
		G4int Remainder(G4int dividend, G4int divisor) {return dividend % divisor;}

		virtual void SetUpData(G4int Nrow, G4int Ncolumn, G4int Nbins, G4int NoImages);
		virtual void SaveHitData(G4int DetectorNumber);
		virtual void PrintHitData();
		virtual void SaveEnergyData(G4int DetectorNumber, G4double edep);
		virtual void PrintEnergyData();
		virtual void WriteToTextFile();

	private:	

		//Data members
		std::vector<std::vector<std::vector<G4int> > > HitDataMatrix;
		std::vector<std::vector<G4int> > EnergyMatrix;

		G4int rows;
		G4int columns;	
		G4int bins;

		G4double MaxE;

		G4int Seed;
		G4int NumberOfPhotons;
		G4int NumberOfImages;
		G4int CurrentImage;

		G4String PhysicsUsed;
		G4double SimulationTime;
};

#endif

		
