#ifndef Data_h
#define Data_h 1

#include <iostream>
#include <vector>

class Data
{
	public:
		Data(G4int Nrow, G4int Ncolumn)
		{
			//Saves the rows and columns inputted using the Set methods
			SetNumberRows(Nrow);
			SetNumberColumns(Ncolumn);

			//Creates a 2D vector when the object is created as big as the columns and rows inputted
			std::vector<std::vector<G4int> > iDataMatrix;
			iDataMatrix.resize(Nrow, std::vector<G4int>(Ncolumn));
			
			//Saves the matrix using the set method			
			SetData(iDataMatrix);

			SetNumberOfTotalHits(0);
		}
		
		~Data(){};
		
		//Get Methods
		std::vector <std::vector<int> > GetData(){ return DataMatrix;}
		G4int GetElement(G4int x, G4int y){return DataMatrix[x][y];}	
		G4int GetNumberRows(){return rows;}
		G4int GetNumberColumns(){return columns;}
		G4int GetNumberOfTotalHits(){return TotalHits;}

		//Set Methods
		void SetData(std::vector<std::vector<G4int> > UpdatedData){DataMatrix = UpdatedData;}
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}
		void SetNumberOfTotalHits(G4int value){TotalHits = value;}

		//Method functions
		G4int Quotient(G4int dividend, G4int divisor ) {return dividend / divisor;}
		G4int Remainder(G4int dividend, G4int divisor) {return dividend % divisor;}

		void SaveData(G4int DetectorNumber)
		{
			//Finds the number of columns the matrix has
			G4int column = GetNumberColumns();
			//Finds the coordinates of the detector inputted for the matrix
			G4int x = Quotient(DetectorNumber, column);
			G4int y = Remainder(DetectorNumber, column);

			//+1 for a hit
			++DataMatrix[x][y]; 

			//Keeps track of the total number of hits
			SetNumberOfTotalHits(GetNumberOfTotalHits() + 1);
		}
		
		void Print()
		{
			//Find each needed value using the Get functions
			std::vector <std::vector<G4int> > Data = GetData();
			G4int Nrows = GetNumberRows();
			G4int Ncolumns = GetNumberColumns();
			
			//Prints out the matrix
			for(G4int x = 0; x < Nrows; x++)  
    			{
    				for(G4int y = 0; y < Ncolumns; y++)  
        			{	
					std::cout << Data[x][y] << " ";
        			}
    				std::cout << std::endl;  
    			}
		}

	private:	
		//Data members
		std::vector<std::vector<int> > DataMatrix;
		G4int rows;
		G4int columns;	
		G4int TotalHits;
};

#endif

		
