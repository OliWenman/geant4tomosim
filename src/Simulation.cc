#include "Simulation.hh"
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "Input.hh"

#include <fstream>
#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4Timer.hh"

Simulation::Simulation()
{	
	Reset = false;
	Ready = false;

	G4cout << "\nWelcome to the tomography data simulation!\n"; 

	Setup();
}

Simulation::~Simulation()
{
	G4cout << "\nGoodbye.\n";

	//delete runManager;
	delete UImanager;
	delete visManager;

	delete data;
	delete input;
}

void Simulation::Setup()
{ 
	G4cout << "Setting up simulation for you... \n";

	//Create an instance of the classes
	runManager = new G4RunManager();
	data = new Data();
	input = new Input(data);
	DC = new DetectorConstruction(data, input); 
	PL = new PhysicsList(input); 

	//Setup the Geant4 user and action intialization	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);
  	runManager -> SetUserInitialization(new ActionInitialization(input, DC, data));

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

	visManager = 0;
}

void Simulation::ResetSimulation()
{
	if (Reset == true)
		{Setup();}
	else
		{G4cout << "\nSimulation doesn't need to be reset!\n ";}
}

void Simulation::Initialize()
{
	if (Reset == true)
		{Setup();}

	G4String PathToFiles = "./../scripts/";

	//Apply the commands from the macro files to fill the values
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "Geometry.mac");
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "settings.mac");

	//Find the total number of images
	G4int TotalImages = input -> GetNoImages();

	//Find the total number of particles and convert to a number
	unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());
	G4cout << "\nNumber of projections being processed is " << TotalImages;
	G4cout << "\nNumber of photons per image is " << TotalParticles;
	G4cout << "\nNumber of detectors is " << DC -> GetNoDetectorsY() << " x " << DC -> GetNoDetectorsZ();
	G4cout << "\nNumber of particles per detector on average is " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ()) << "\n"; 

	G4cout << "\nSimulation Ready!\n";

	Ready = true;
}

void Simulation::Visualisation()
{
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
		G4VisManager* visManager = new G4VisExecutive();

		//Prints a warning incase user forgot to turn off visualization as will heavily affect simulation time. Use only to check geometry position
		G4cout << "\n////////////////////////////////////////////////////////////////////////////////\n"
		       << "\n     WARNING: GRAPHICS SYSTEM ENABLED - Will increase computational time.\n" 
		       << "\n////////////////////////////////////////////////////////////////////////////////\n";

		visManager -> Initialize();
		UImanager -> ApplyCommand("/control/execute MyVis.mac");
	}
}

void Simulation::RunSimulation()
{
	if (Ready == true)
	{
		//Start the simulation timer
		G4Timer FullTime;
		FullTime.Start();

		G4cout << "Starting simulation \n";

		//Prints the time and date of the local time that the simulation started
		time_t now = time(0);
		// Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);
		G4cout << "\n" << asctime(localtm) << "\n";

		G4cout << "Beam on... \n"; 

		//Find the total number of images
		G4int TotalImages = input -> GetNoImages();

		//Find the total number of particles and convert to a number
		unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());

		for (G4int Image = 0; Image < TotalImages; Image++)
		{
			//Start internal looptimer to update the estimated time for completion
			G4Timer LoopTimer;
			LoopTimer.Start();
		
			G4cout << "\n================================================================================"
		       	       << "\n                           PROCESSING IMAGE " <<  Image+1
	               	       << "\n================================================================================\n";
		
			//Beam on to start the simulation
			BeamOn(runManager, TotalParticles);
		
			//Prepare for next run that geometry has changed
			G4RunManager::GetRunManager() -> ReinitializeGeometry();
		
			SaveDataToFile(data);

			//Stop loop timer and estimate the remaining time left
			LoopTimer.Stop();
			//CompletionTime(LoopTimer.GetRealElapsed(), Image, TotalImages);
		}
	
		//Stop the full simulation time and save to data class
		FullTime.Stop();
		input -> SetSimulationTime(FullTime.GetRealElapsed());
		input -> WriteToTextFile();

		G4cout << "\n================================================================================"
	               << "\n                        The simulation is finihsed! "
	               << "\n             Total simulation run time : "<< FullTime
	               << "\n================================================================================\n";
	}
	else if (Reset == true || Ready == false)
	{
		G4cout << "\nSIMULATION IS NOT READY! Check the macro files and Initialise the simulation first! \n";
	}
}

void Simulation::KillSimulation()
{
	delete runManager;
	delete UImanager;
	delete visManager;

	delete data;
	delete input;

	Reset = true;
}

//Private functions
void Simulation::BeamOn(G4RunManager* runManager, unsigned long long int nParticles)
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

void Simulation::SaveDataToFile(Data* data)
{
	data -> WriteToTextFile();
	data -> WriteToHDF5();
}

void Simulation::CompletionTime(double LoopTimer, int Image, int NoImages)
{
	//Calculates the estimated time
	G4double ETSeconds = (LoopTimer * NoImages) - (LoopTimer * Image);

	if (NoImages > 1)
	{
		//Prints out the sustiable units for the estimated time 
		if (ETSeconds > 60)
		{
			if(ETSeconds > 60*60)
				{G4cout << int(ETSeconds/(60*60)) << " hours left \n";}
			else
				{G4cout << int(ETSeconds/60) << " minutes left \n";}
		}
		else
			{G4cout << "\nLess than a minute left to go... \n";}
	}
}
