#include "Data.hh"
#include "DataMessenger.hh"
#include <iomanip>
#include <fstream>
#include "G4UnitsTable.hh"

Data::Data()
{
	G4cout << G4endl << "Data class has been created "<< G4endl;
	dataMessenger = new DataMessenger(this);
}

Data::~Data()
{
	G4cout << G4endl << "Data class has been deleted "<< G4endl;
	delete dataMessenger;
}

void Data::SetUpData(G4int Nrow, G4int Ncolumn)
{
	//Saves the rows and columns inputted using the Set methods
	SetNumberRows(Nrow);
	SetNumberColumns(Ncolumn);
	
	G4int Nbins = GetNoBins();
	G4int NImages = GetNoImages();
	
		
	G4int EnergyColumns = GetNumberRows() * GetNumberColumns();

	//Creates a 2D vector when the object is created as big as the columns and rows inputted
	std::vector<std::vector<std::vector<G4int> > > iHitDataMatrix;
	//iHitDataMatrix.resize(Nrow, std::vector<G4int>(Ncolumn), std::vector<std::vector<G4int> >(GetNumberOfImages()));
	
	iHitDataMatrix.resize(Nrow,std::vector<std::vector<G4int> >(Ncolumn,std::vector<G4int>(NImages)));
			
	//Creates a 2D vector when the object is created as big as the columns and rows inputted
	std::vector<std::vector<G4int> > iEnergyMatrix;
	iEnergyMatrix.resize(Nbins, std::vector<G4int>(EnergyColumns));
			
	//Saves the matrix using the set method			
	SetHitData(iHitDataMatrix);
	SetEnergyData(iEnergyMatrix);
}

void Data::SaveHitData(G4int DetectorNumber)
{
	//Finds the number of columns the matrix has
	G4int column = GetNumberColumns();
			
	//Finds the coordinates of the detector inputted for the matrix
	G4int x = Quotient(DetectorNumber, column);
	G4int y = Remainder(DetectorNumber, column);
	
	G4int Image = GetCurrentImage();

	//+1 for a hit
	++HitDataMatrix[x][y][Image]; 
}

void Data::PrintHitData()
{
	//Find each needed value using the Get functions
	std::vector<std::vector<std::vector<G4int> > > HitData = GetHitData();
	G4int Nrows = GetNumberRows();
	G4int Ncolumns = GetNumberColumns();
			
	G4int x = 0;
	G4int y = 0;
	G4int Image = GetCurrentImage();

	//Prints out the matrix
	G4cout << G4endl << "Hit count data " << G4endl;
	G4cout << "Image: " << Image+1 << G4endl; 
	G4cout << "[";
	for(G4int x = 0 ; x < Nrows; x++)  
    	{
		if (x > 0)
			{G4cout << " [";}
		else
			{G4cout << "[";}
    		for(G4int y = 0 ; y < Ncolumns; y++)  
        	{	G4cout << std::setfill(' ') << std::setw(5) << HitData[x][y][Image];
			if ((Nrows - 1) == y)	
				{G4cout << " ";}
			else 
				{G4cout << ",";}
		}
		if (y < Ncolumns - 1 && x < Nrows - 1)
			{G4cout << "]," << G4endl;}	
		else
			{G4cout << "]]" << G4endl;}
    	
	}
}

void Data::SaveEnergyData(G4int DetectorNumber, G4double edep)
{
	G4double BinSize = GetMaxEnergy()/GetNoBins();	
	G4int BinNumber = floor(edep/BinSize);
	++EnergyMatrix[BinNumber][DetectorNumber];
}

void Data::PrintEnergyData()
{
	std::vector <std::vector<G4int> > EnergyData = GetEnergyData();
	G4int NoDigits = std::to_string(GetNumberRows()*GetNumberColumns()).length();
	G4cout << G4endl << "Energy detector data (keV)" << G4endl;

	G4cout << "          "; 
	for (G4int n = 0; n < NoDigits; n++)
		{G4cout << " ";}	
	for (G4int x = 0 ; x < GetNumberBins(); x++)
	{	G4cout << "   "; 
		G4cout << std::setfill(' ') << std::setw(5) << (GetMaxEnergy()/GetNumberBins() * (x+1))*1000;
	}
	G4cout << G4endl;

	//Prints out the matrix
	for(G4int x = 0 ; x < GetNumberRows()*GetNumberColumns(); x++)  
    	{	for( G4int y = 0 ; y < GetNumberBins(); y++)  
        	{	if (y == 0)
				{G4cout << "Detector " << std::setfill('0') << std::setw(NoDigits) << x << ":";}
			G4cout << "   " << std::setfill(' ') << std::setw(5) << EnergyData[y][x] ;
        	}
    		G4cout << G4endl;  
    	}
}

void Data::WriteToTextFile()
{
	G4cout << G4endl << "Writing the data to DataFile.txt" << G4endl;
	//Create file stream
   	std::ofstream outdata; 
   
	//Get needed variables to wrtie to file
   	std::vector<std::vector<std::vector<G4int> > > HitData = GetHitData();
   	std::vector <std::vector<G4int> > EnergyData = GetEnergyData();
	G4int Nrows = GetNumberRows();
	G4int Ncolumns = GetNumberColumns();
	G4int NImage = GetNoImages();

	//G4int NoDigits = std::to_string(GetNumber).length();

	outdata.open("DataFile.txt"); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "Error: file could not be opened" << std::endl;
      		exit(1);
   	}

	outdata << "Simulation of X-Ray data." << std::endl << std::endl;

	outdata << "Conditioons used in this simulation are: " << std::endl;
	outdata << "- Physics package: " << GetPhysicsUsed() << std::endl;
	outdata << "- Seed: " << GetSeed() << std::endl;
	outdata << "- Intial energy of the monochromatic beam: " << G4BestUnit(GetMaxEnergy(),"Energy") << std::endl;
	outdata << "- Number of detectors along the y axis: " << GetNumberColumns() << std::endl;
	outdata << "- Number of detectors along the z axis: " << GetNumberRows() << std::endl;
	outdata << "- Number of photons used per image: " << GetNumberOfPhotons() << std::endl;
	outdata << "- Number of images: " << GetNumberOfImages() << std::endl;
	outdata << "- Real simulation time: " << GetSimulationTime() << std::endl;
	
	outdata << std::endl;

	G4int x = 0;
	G4int y = 0;
	G4int Image = 0;

	outdata << "The hit count data" << std::endl;

	for (G4int Image = 0; Image < NImage; Image++)
	{
		outdata << "Image: " << Image+1 << std::endl;
		outdata << "[";
		for(G4int x = 0 ; x < Nrows; x++)  
    		{
			if (x > 0)
				{outdata << " [";}
			else
				{outdata << "[";}
    			for(G4int y = 0 ; y < Ncolumns; y++)  
        		{	outdata << std::setfill(' ') << std::setw(5) << HitData[x][y][Image];
				if ((Nrows - 1) == y)	
					{outdata << " ";}
				else 
					{outdata << ",";}
			}
			if (y < Ncolumns - 1 && x < Nrows - 1)
				{outdata << "]," << G4endl;}	
			else
				{outdata << "]]" << G4endl;}
    		}
	}

	outdata << G4endl << "Energy detector data (keV)" << G4endl;

	G4int NoDigits = std::to_string(GetNumberRows()*GetNumberColumns()).length();

	outdata << "          "; 
	for (G4int n = 0; n < NoDigits; n++)
		{outdata << " ";}
			
	for (G4int x = 0 ; x < GetNoBins(); x++)
	{	outdata << "   "; 
		outdata << std::setfill(' ') << std::setw(5) << (GetMaxEnergy()/GetNoBins() * (x+1))*1000;
	}
	outdata << G4endl;

	//Prints out the matrix
	for(G4int x = 0 ; x < GetNumberRows()*GetNumberColumns(); x++)  
    	{	for( G4int y = 0 ; y < GetNoBins(); y++)  
        	{	if (y == 0)
				{outdata << "Detector " << std::setfill('0') << std::setw(NoDigits) << x << ":";}
			outdata << "   " << std::setfill(' ') << std::setw(5) << EnergyData[y][x];
        	}
    		outdata << G4endl;  
    	}

  	outdata.close();
	G4cout << G4endl << "The data has been successfully written to DataFile.txt" << G4endl << G4endl;
}


