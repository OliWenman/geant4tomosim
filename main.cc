#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "globals.hh"

#include "G4Timer.hh"

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "hdf5.h"

//CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
//GenerateSeed(data);

//Not sure if to keep or to use RunAction class to do it. Currently using RunAction
void GenerateSeed(Data* data)
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
}

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


int main(int argc,char** argv)
{
  	/* Create a new file using default properties. */
   	//hid_t file_id = H5Fcreate("ff", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   	/* Terminate access to the file. */
   	//herr_t status = H5Fclose(file_id);

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
	DetectorConstruction* DC = new DetectorConstruction(data); 
	PhysicsList* PL = new PhysicsList(data); 	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);
  	runManager -> SetUserInitialization(new ActionInitialization(data, DC));

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

    	//interactive mode
	UImanager -> ApplyCommand("/control/execute settings.mac");
	
	if (data -> GetVisualization() == true)
	{
		visManager -> Initialize();
		UImanager -> ApplyCommand("/control/execute MyVis.mac");
		G4cout << G4endl << "================================================================================"
		       << G4endl << "     WARNING: GRAPHICS SYSTEM ENABLED - Will increase computational time."
	               << G4endl << "================================================================================" << G4endl;
	}
	else 
	{ visManager =0;}

	//Save variables to needed classes
	G4int Image = 0;

	//Start the simulation timer
	G4Timer FullTime;
	FullTime.Start();

	for (Image; Image < data -> GetNoImages(); Image++)
	{
		//Start internal looptimer to update the estimated time for completion
		G4Timer LoopTimer;
		LoopTimer.Start();

		//Let other clases know what the current image is 
		data -> SetCurrentImage(Image);
		
		G4cout << G4endl << "================================================================================"
		       << G4endl << "                           PROCESSING IMAGE " <<  Image+1
	               << G4endl << "================================================================================" << G4endl;
		
		//Beam on to start the simulation
		runManager -> BeamOn(data -> GetNoPhotons());
		
		//Prepare for next run that geometry has changed
		G4RunManager::GetRunManager()->ReinitializeGeometry();
		
		//Stop loop timer and estimate the remaining time left
		LoopTimer.Stop();
		CompletionTime(LoopTimer.GetRealElapsed(), Image, data -> GetNoImages());
	}
	
	//Stop the full simulation time and save to data class
	FullTime.Stop();
	data -> SetSimulationTime(FullTime.GetRealElapsed());

	G4cout << G4endl << "================================================================================"
	       << G4endl << "                      The simulation is complete! "
	       << G4endl << "             Total simulation run time : "<< FullTime
	       << G4endl << "================================================================================" << G4endl;
	
	//Write data to file if command is turned on
	if (data -> GetTextFileCmd() == true)
		{data -> WriteToTextFile();}
	else if(data -> GetHDF5FileCmd() == true)
		{data -> WriteToHDF5();}
	else 
		{G4cout << G4endl << "The data from the simulation was not saved!" << G4endl;}

	ui -> SessionStart();   	

	//Delete the remaining pointers
	G4cout << G4endl << "Deleting ui";
	delete ui;
	G4cout << G4endl << "Deleting visManager" << G4endl; 
	delete visManager;
	G4cout << G4endl << "Deleting runManager " << G4endl;
  	delete runManager;
	delete data;

   	return 0;
}

