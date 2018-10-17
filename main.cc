#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "Input.hh"
#include "globals.hh"

#include "G4Timer.hh"
#include <ctime>
#include <climits>

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include <fstream>
#include <cstdlib>
#include <string>

#include "hdf5.h"

//CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
//GenerateSeed(data);

//Not sure if to keep or to use RunAction class to do it. Currently using RunAction
/*void GenerateSeed(Data* data)
{
	G4int seedCmd = data -> GetSeedOption();	
	if (seedCmd != 0)	//Keeps the seed
	{		
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}
	else if (seedCmd == 0)	//Random seed
	{
		//set random seed with system time
		seedCmd = time(NULL);
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}
	if (data -> GetCurrentImage() == 0)
		{data -> SetSeedOption(seedCmd);}
}*/

void CompletionTime(double LoopTimer, int Image, int NoImages)
{
	//Calculates the estimated time
	G4double ETSeconds = (LoopTimer * NoImages) - (LoopTimer * Image);

	if (NoImages > 1)
	{
		//Prints out the sustiable units for the estimated time 
		if (ETSeconds > 60)
		{
			if(ETSeconds > 60*60)
				{G4cout << G4endl << int(ETSeconds/(60*60)) << " hours left" << G4endl;}
			else
				{G4cout << G4endl << int(ETSeconds/60) << " minutes left" << G4endl;}
		}
		else
			{G4cout << G4endl << "Less than a minute left to go " << G4endl;}
	}
}

void SaveDataToFile(G4int Image, Data* data)
{
	//Write data to file if command is turned on
	if (data -> GetTextFileCmd() == true)
		{data -> WriteToTextFile();}
	else if(data -> GetHDF5FileCmd() == true)
		{data -> WriteToHDF5();}
	else 
		{G4cout << G4endl << "The data from the simulation was not saved!" << G4endl;}

}

//Function to correctly input the correct number of particles if it is over the limit 
void BeamOn(G4RunManager* runManager, unsigned long long int nParticles)
{
	//Max limit for an integer (for Geant4 BeamOn), must be unsigned long long int as well to compare it if input is above limit.
	unsigned long long int limit = 2147483647;

	//Checks to see if the input is above the limit
	if (nParticles > limit)
	{
		//Rounds up to the nearest number to do the correct number of loops
		int NumberOfLoops = std::ceil(nParticles/limit);

		for (int loop = 1 ; loop <= NumberOfLoops ; loop++)
		{
			if (nParticles > limit)
			{	//Beam on to start the simulation with the max number limit
				runManager -> BeamOn(limit);
				//Subtract from the true number of inputted particles until it is within the limit
				nParticles = nParticles - limit;
			}
			else 
			{	//Once within the limit, fire the remaining particles that are left 
				runManager -> BeamOn(nParticles);
			}	
		}
	}
	else 
	{	//If the inputted number of particles is within the limit, fire that number of particles
		runManager -> BeamOn(nParticles);
	}
}

int main(int argc,char** argv)
{
  	/* Create a new file using default properties. */
   	//hid_t file_id = H5Fcreate("ff", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   	/* Terminate access to the file. */
   	//herr_t status = H5Fclose(file_id);


	//Prints the time and date of the local time that the simulation started
	time_t now = time(0);
	// Convert now to tm struct for local timezone
	tm* localtm = localtime(&now);
	G4cout << G4endl << "This simulation started on: " << asctime(localtm) << G4endl;


  	//Detect interactive mode (if no arguments) and define UI session
  	G4UIExecutive* ui = 0;
  	
	if ( argc == 1 ) 
		{ui = new G4UIExecutive(argc, argv);}
  
  	//Construct the default run manager
  	G4RunManager* runManager = new G4RunManager();

	//Initialize visualization
  	G4VisManager* visManager = new G4VisExecutive();

	//Create an instance of the classes
	Data* data = new Data();
	Input* input = new Input(data);
	DetectorConstruction* DC = new DetectorConstruction(data, input); 
	PhysicsList* PL = new PhysicsList(input); 

	//Setup the Geant4 user and action intialization	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);
  	runManager -> SetUserInitialization(new ActionInitialization(input, DC, data));

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

    	//interactive mode
	UImanager -> ApplyCommand("/control/execute settings.mac");
	
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
		visManager -> Initialize();
		UImanager -> ApplyCommand("/control/execute MyVis.mac");
		
		//Prints a warning incase user forgot to turn off visualization as will heavily affect simulation time. Use only to check geometry position
		G4cout << G4endl << "////////////////////////////////////////////////////////////////////////////////"
		       << G4endl 
		       << G4endl << "     WARNING: GRAPHICS SYSTEM ENABLED - Will increase computational time."
	               << G4endl 
		       << G4endl << "////////////////////////////////////////////////////////////////////////////////" << G4endl;
	}
	else 
		{visManager = 0;}

	//Find the total number of images
	G4int TotalImages = input -> GetNoImages();

	//Find the total number of particles and convert to a number
	unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());
	G4cout << G4endl << "Number of photons per image is " << TotalParticles;
	G4cout << G4endl << "Number of particles per detector on average is " << TotalParticles/(DC -> GetNoDetectorsY() * DC->GetNoDetectorsZ()) << G4endl; 

	//Start the simulation timer
	G4Timer FullTime;
	FullTime.Start();

	for (G4int Image = 0; Image < TotalImages; Image++)
	{
		//Start internal looptimer to update the estimated time for completion
		G4Timer LoopTimer;
		LoopTimer.Start();

		//Let other clases know what the current image is 
		data -> SetCurrentImage(Image);
		input ->SetCurrentImage(Image);
		
		G4cout << G4endl << "================================================================================"
		       << G4endl << "                           PROCESSING IMAGE " <<  Image+1
	               << G4endl << "================================================================================" << G4endl;
		
		//Beam on to start the simulation
		BeamOn(runManager, TotalParticles);
		
		//Prepare for next run that geometry has changed
		G4RunManager::GetRunManager() -> ReinitializeGeometry();
		
		SaveDataToFile(Image, data);

		//Stop loop timer and estimate the remaining time left
		LoopTimer.Stop();
		CompletionTime(LoopTimer.GetRealElapsed(), Image, TotalImages);
	}
	
	//Stop the full simulation time and save to data class
	FullTime.Stop();
	input -> SetSimulationTime(FullTime.GetRealElapsed());
	input -> WriteToTextFile();

	G4cout << G4endl << "================================================================================"
	       << G4endl << "                        The simulation is finihsed! "
	       << G4endl << "             Total simulation run time : "<< FullTime
	       << G4endl << "================================================================================" << G4endl;

	//ui -> SessionStart();   

	//Save the times for speed tests
	std::ofstream outdata; 
	G4String filepath = "./Data_Output/Text/SpeedTests/";
	G4String txt = ".txt";
	outdata.open(filepath+ std::to_string(TotalParticles) + txt, std::ios::out | std::ios::app);
	if( !outdata ) 
	{ 	std::cerr << "Error: " << TotalParticles << " file could not be opened" << std::endl;
      		exit(1);
   	}
	else
		{std::cout << std::endl << "File opened" << std::endl;}
	outdata << FullTime.GetRealElapsed() << std::endl;
	outdata.close();

	//Delete the remaining pointers
	G4cout << G4endl << "Deleting ui";
	delete ui;
	G4cout << G4endl << "Deleting visManager"; 
	delete visManager;
	G4cout << G4endl << "Deleting runManager ";
  	delete runManager;
	delete data;

   	return 0;
}

