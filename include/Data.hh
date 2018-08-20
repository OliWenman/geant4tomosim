#ifndef Data_h
#define Data_h 1

#include <iostream>
#include <vector>

class Data
{
	public:
		Data(){}
		
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
	
		void SetUpDetectors(G4int Nrow, G4int Ncolumn)
		{
			SetNumberRows(Nrow);
			SetNumberColumns(Ncolumn);

			std::vector<std::vector<G4int> > iDataMatrix;
			iDataMatrix.resize(Nrow, std::vector<G4int>(Ncolumn));
			SetData(iDataMatrix);

			SetNumberOfTotalHits(0);
		}

		void SaveData(G4int DetectorNumber)
		{
			G4int column = GetNumberColumns();
			G4int x = Quotient(DetectorNumber, column);
			G4int y = Remainder(DetectorNumber, column);

			++DataMatrix[x][y]; 

			SetNumberOfTotalHits(GetNumberOfTotalHits() + 1);
		}
		
		void Print()
		{
			std::vector <std::vector<int> > Data = GetData();
			G4int Nrows = GetNumberRows();
			G4int Ncolumns = GetNumberColumns();
			
			for(G4int x = 0; x < Nrows; x++)  // loop 3 times for three lines
    			{
    				for(G4int y = 0; y < Ncolumns; y++)  // loop for the three elements on the line
        			{	
					std::cout << Data[x][y] << " ";
        			}
    				std::cout << std::endl;  // when the inner loop is done, go to a new line
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

		
