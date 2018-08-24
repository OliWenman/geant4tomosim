#include "Data.hh"

Data::Data(){G4cout << G4endl << "Data class has been created "<< G4endl;}

Data::~Data(){G4cout << G4endl << "Data class has been deleted "<< G4endl;}

void Data::SetUpMatricies(G4int Nrow, G4int Ncolumn, G4int Nbins)
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

void Data::SaveHitData(G4int DetectorNumber)
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

void Data::PrintHitData()
{
	//Find each needed value using the Get functions
	std::vector <std::vector<G4int> > HitData = GetHitData();
	G4int Nrows = GetNumberRows();
	G4int Ncolumns = GetNumberColumns();
			
	G4cout << G4endl << "Hit count data " << G4endl;
			
	//Prints out the matrix
	for(G4int x = 0; x < Nrows; x++)  
    	{
    		for(G4int y = 0; y < Ncolumns; y++)  
        	{	
			G4cout << HitData[x][y] << " ";
        	}
    		G4cout << G4endl;  
    	}
}

void Data::SaveEnergyData(G4int DetectorNumber, G4double edep)
{
	G4double MaxEnergy = 30*keV;

	SetMaxEnergy(MaxEnergy);

	G4double BinSize = MaxEnergy/GetNumberBins();
			
	G4int BinNumber = floor(edep/BinSize);

	++EnergyMatrix[BinNumber][DetectorNumber];
}

void Data::PrintEnergyData()
{
	std::vector <std::vector<G4int> > EnergyData = GetEnergyData();
	G4cout << G4endl << "Energy detector data " << G4endl;

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
			G4cout << EnergyData[y][x] << " ";
        	}
    		G4cout << G4endl;  
    	}
}



