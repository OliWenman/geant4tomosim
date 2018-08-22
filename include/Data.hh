#ifndef Data_h
#define Data_h 1

#include <iostream>
#include <vector>
#include <math.h>       // Used to access the floor command floor 
#include "G4SystemOfUnits.hh"

class Data
{
	public:
		Data(G4int Nrow, G4int Ncolumn, G4int Nbins)
		{
			//Saves the rows and columns inputted using the Set methods
			SetNumberRows(Nrow);
			SetNumberColumns(Ncolumn);
			SetNumberBins(Nbins);
		
			G4int EnergyColumns = GetNumberRows() * GetNumberColumns();

			//Creates a 2D vector when the object is created as big as the columns and rows inputted
			std::vector<std::vector<G4int> > iHitDataMatrix;
			iHitDataMatrix.resize(Nrow, std::vector<G4int>(Ncolumn));
			
			//Creates a 2D vector when the object is created as big as the columns and rows inputted
			std::vector<std::vector<G4int> > iEnergyMatrix;
			iEnergyMatrix.resize(bins, std::vector<G4int>(EnergyColumns));
			
			//Saves the matrix using the set method			
			SetHitData(iHitDataMatrix);
			SetEnergyData(iEnergyMatrix);

			SetNumberOfTotalHits(0);
		}
		
		~Data(){};
		
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

		void SaveHitData(G4int DetectorNumber)
		{
			//Finds the number of columns the matrix has
			G4int column = GetNumberColumns();
			
			//Finds the coordinates of the detector inputted for the matrix
			G4int x = Quotient(DetectorNumber, column);
			G4int y = Remainder(DetectorNumber, column);

			//+1 for a hit
			++HitDataMatrix[x][y]; 

			//Keeps track of the total number of hits
			SetNumberOfTotalHits(GetNumberOfTotalHits() + 1);
		}
		
		void PrintHitData()
		{
			//Find each needed value using the Get functions
			std::vector <std::vector<G4int> > Data = GetHitData();
			G4int Nrows = GetNumberRows();
			G4int Ncolumns = GetNumberColumns();
			
			G4cout << G4endl << "The updated matrix of hits " << G4endl;
			
			//Prints out the matrix
			for(G4int x = 0; x < Nrows; x++)  
    			{
    				for(G4int y = 0; y < Ncolumns; y++)  
        			{	
					G4cout << Data[x][y] << " ";
        			}
    				G4cout << G4endl;  
    			}
		}

		void SaveEnergyData(G4int DetectorNumber, G4double edep)
		{
			G4double MaxEnergy = 30*keV;

			SetMaxEnergy(MaxEnergy);

			G4double BinSize = MaxEnergy/GetNumberBins();
			
			G4int BinNumber = floor(edep/BinSize);
			
			G4cout << "It has been placed in bin number: " << BinNumber << G4endl;

			++EnergyMatrix[BinNumber][DetectorNumber];
		}
		
		void PrintEnergyData()
		{
			std::vector <std::vector<G4int> > Data = GetEnergyData();
			G4cout << G4endl << "The updated matrix of energy " << G4endl;

			G4cout << "            ";
			
			for (G4int x = 0 ; x < GetNumberBins(); x++)
			{
				G4cout << (GetMaxEnergy()/GetNumberBins()) * (x+1) << " ";
			}
			G4cout << G4endl;

			//Prints out the matrix
			for(G4int x = 0 ; x < GetNumberRows()*GetNumberColumns(); x++)  
    			{
    				for( G4int y = 0 ; y < GetNumberBins(); y++)  
        			{	
					if (y == 0)
					{
						G4cout << "Detector " << x << ": ";
					}
					G4cout << Data[y][x] << " ";
        			}
    				G4cout << G4endl;  
    			}
		}

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

		
