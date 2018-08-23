#ifndef Data_h
#define Data_h 1

#include "globals.hh"
#include <iostream>
#include <vector>
#include <math.h>       // Used to access the floor command floor 
#include "G4SystemOfUnits.hh"

class Data
{
	public:
		Data();
		virtual ~Data();
		
		//Get Methods
		std::vector <std::vector<int> > GetHitData(){ return HitDataMatrix;}
		std::vector <std::vector<int> > GetEnergyData(){return EnergyMatrix;}

		G4int GetHitElement(G4int x, G4int y){return HitDataMatrix[x][y];}	

		G4int GetNumberRows(){return rows;}
		G4int GetNumberColumns(){return columns;}
		G4int GetNumberBins(){return bins;}

		G4int GetNumberOfTotalHits(){return TotalHits;}

		G4double GetMaxEnergy(){return MaxE;}

		//Set Methods
		void SetHitData(std::vector<std::vector<G4int> > UpdatedData){HitDataMatrix = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<G4int> > UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}
		void SetNumberBins(G4int value){bins = value;}

		void SetNumberOfTotalHits(G4int value){TotalHits = value;}

		void SetMaxEnergy(G4double value){MaxE = value;}

		//Method functions
		G4int Quotient(G4int dividend, G4int divisor ) {return dividend / divisor;}
		G4int Remainder(G4int dividend, G4int divisor) {return dividend % divisor;}

		virtual void SetUpMatricies(G4int Nrow, G4int Ncolumn, G4int Nbins);
		virtual void SaveHitData(G4int DetectorNumber);
		virtual void PrintHitData();
		virtual void SaveEnergyData(G4int DetectorNumber, G4double edep);
		virtual void PrintEnergyData();

	private:	
		//Data members
		std::vector<std::vector<int> > HitDataMatrix;
		std::vector<std::vector<int> > EnergyMatrix;

		G4int rows;
		G4int columns;	
		G4int bins;

		G4int TotalHits;
		G4double MaxE;
};

#endif

		
