//Own classes
#include "Simulation.hh"
#include "SimulationMessenger.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "DefineMaterials.hh"

//Geant4 managers for running the simulation
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

//Used for timing simulation and getting a random seed
#include "G4Timer.hh"
#include "time.h"
#include "Randomize.hh"

//Geant4 units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ThreeVector.hh"

//Read/write to a file
#include <fstream>
#include <iomanip>

Simulation::Simulation()
{	
	simMessenger = new SimulationMessenger(this);

	Reset = false;
	Ready = false;

	WriteToTextCmd = false;

	G4cout << "\nWelcome to the tomography data simulation!\n"; 

	Setup();
}

Simulation::~Simulation()
{
	G4cout << "\nDeleting simulation... \n";

	delete simMessenger;

	delete runManager;
	delete visManager;

	delete data;
	delete materials;

	G4cout << "\nSimulation deleted! \n" << G4endl;
}

void Simulation::Setup()
{ 
	G4cout << "Setting up... \n";

	//Create an instance of the classes
	runManager = new G4RunManager();
	//runManager -> SetVerboseLevel(1);
	data = new Data();
	DC = new DetectorConstruction(data); 
	PL = new PhysicsList();
	materials = new DefineMaterials(); 
	visManager = 0;

	//Setup the Geant4 user and action intialization	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);

	PGA = new PrimaryGeneratorAction();
	runManager -> SetUserAction(PGA);

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
}

void Simulation::pyInitialise(int nDetectorsY, int nDetectorsZ, std::vector<double> DetDimensions, int nBins)
{
	G4String PathToFiles = "./../scripts/";

	DC -> SetNoDetectorsY(nDetectorsY);
	DC -> SetNoDetectorsZ(nDetectorsZ);
	DC -> SetDetectorSize(G4ThreeVector(DetDimensions[0], DetDimensions[1], DetDimensions[2])); 

	if (nBins > 0)
		{DC -> SetFluorescenceDet(true);}
	else
		{DC -> SetFluorescenceDet(false);}
	data -> SetNoBins(nBins);

	//Apply the commands from the macro files to fill the values
	G4cout << "\nReading Geometry.mac... ";
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "Geometry.mac");

	G4cout << "\nReading pySettings.mac...\n";
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "pySettings.mac");

	if (PL -> GetFluorescence() == false)
		{runManager -> SetUserAction(new StackingAction());}

	SaveLogPath = "./../Output/Text/SimulationLog.txt";

	//Let the PrimaryGeneratorAction class know where to position the start of the beam
	PGA-> SetValues(nBins, DC -> GetWorldSize().x());

	//Tell the data class what the max energy is
	data -> SetMaxEnergy(PGA -> GetMaxEnergy());

	//Readout the inputs from the user and save to a file
	DC -> ReadOutInfo(SaveLogPath);
	PGA -> ReadOutInfo(SaveLogPath);
	PL -> ReadOutInfo(SaveLogPath);

        G4cout << "\nCommands successfully added\n"

	       << "\nSimulation Ready!" << G4endl;

	Visualisation();

	Ready = true;	
}

std::vector<int> Simulation::pyRun(unsigned long long int TotalParticles, int Image, int NumberOfImages, double TotalAngle)
{
	//Checks to see if simulation is ready (if pyInitialise has been used before)
	if (Ready == true)
	{
		if(Image == 0)
		{
			//Keeps the seed
			if (seedCmd != 0)	
				{CLHEP::HepRandom::setTheSeed(seedCmd);}

			//Random seed
			else if (seedCmd == 0)	
			{
				//set random seed with system time
				seedCmd = time(NULL);
				CLHEP::HepRandom::setTheSeed(seedCmd);
			}

			DC -> RelayToTC(NumberOfImages, TotalAngle);

			//Prints the time and date of the local time that the simulation started
			time_t now = time(0);
			//Convert now to tm struct for local timezone
			tm* localtm = localtime(&now);

			G4cout << "\nStarting simulation! \n\n"

			       << asctime(localtm)

			       << "\nMETA DATA: \n"

			       << "\n- The seed used: " << seedCmd
			       << "\n- Number of projections being processed: " << NumberOfImages
	                       << "\n- Number of photons per image: " << TotalParticles
	                       << "\n- Number of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ())
			       << "\n- Full rotation angle: " << G4BestUnit(TotalAngle, "Angle") << G4endl;

			//Creation of the writing to data file stream
			std::fstream outdata; 

			//Open the file within the path set
			outdata.open(SaveLogPath, std::fstream::app); 
   	
			//Output error if can't open file
			if( !outdata ) 
			{ 	std::cerr << "\nError: " << SaveLogPath << " file could not be opened from Simulation::pyRun\n" << std::endl;
      				exit(1);
   			}

			outdata << "\nMETA DATA: \n" 

				<< "\n- The seed used: " << seedCmd
				<< "\n- Number of projections being processed: " << NumberOfImages
	                        << "\n- Number of photons per image: " << TotalParticles
	                        << "\n- Number of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ())
				<< "\n- Full rotation angle: " << G4BestUnit(TotalAngle, "Angle") << " \n";

			outdata.close();
		}

		//Start internal looptimer to update the estimated time for completion
		G4Timer LoopTimer;
		LoopTimer.Start();

		//Creates the arrays for the data, wipes them after each image
		data -> SetUpData(DC -> GetNoDetectorsY(), DC -> GetNoDetectorsZ(), Image);
		
		G4cout << "\n================================================================================"
		       << "\n                                 IMAGE " <<  Image+1
	               << "\n================================================================================\n" 

		       << "\nProcessing..." << G4endl;
		
		//Beam on to start the simulation
		BeamOn(TotalParticles);
		
		//Prepare for next run that geometry has changed
		G4RunManager::GetRunManager() -> ReinitializeGeometry();

		G4cout << "\nRun finished " << G4endl;

		if (WriteToTextCmd == true)
			{data -> WriteToTextFile(Image);}

		unsigned long long int limit = 2147483647;

		//Print the progress of the simulation
		int Progress = ((Image+1)*100)/NumberOfImages;

		//Print the progress depending if all the particles have been done
		if(limit - TotalParticles  > 0)
		{	
			//Outputs how much of the simulation is complete
			G4cout << "\n" << Progress << "%\ complete " << G4endl;
		}
		else
		{	
			//Equation to work out how much of the simulation is complete if it has to do another run for the remaining number of photons
			Progress = Progress * (TotalParticles - limit*(NumberOfImages - Image+1)/TotalParticles);
			G4cout << "\n" << Progress << "%\ complete " << G4endl;

			//If less then 100% complete, output preparing to do another run of the same image
			if (Progress < 100)
				{G4cout << "Starting next run for the remaining number of photons " << G4endl;}
		}

		//Stop loop timer and estimate the remaining time left
		LoopTimer.Stop();

		if (Image == 0)
			{PGA -> SetBeamCheck(true);}

		CompletionTime(LoopTimer.GetRealElapsed(), Image + 1, NumberOfImages);

		if (Image + 1 == NumberOfImages)
		{
			G4cout << "\n================================================================================"
	                       << "\n                        The simulation is finihsed! "
	                       << "\n================================================================================" << G4endl;
		}
	
		return data -> GetHitData();
	}
	else if (Ready == false)
	{
		G4cout << "\nSIMULATION IS NOT READY! Check the macro files and initialize the simulation first! \n";
	}
}

void Simulation::Visualisation()
{
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
		visManager = new G4VisExecutive();

		//Prints a warning incase user forgot to turn off visualization as will heavily affect simulation time. Use only to check geometry position
		G4cout << "\n\n////////////////////////////////////////////////////////////////////////////////\n"
		       << "\n     WARNING: GRAPHICS SYSTEM ENABLED - Will increase computational time.\n" 
		       << "\n////////////////////////////////////////////////////////////////////////////////\n" << G4endl;

		visManager -> Initialize();
		UImanager -> ApplyCommand("/control/execute ./../scripts/MyVis.mac");
	}
}

void Simulation::Initialize()
{
	/*G4String PathToFiles = "./../scripts/";

	//Apply the commands from the macro files to fill the values
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "Geometry.mac");
	UImanager -> ApplyCommand("/control/execute " + PathToFiles + "cSettings.mac");
	Visualisation();

	//Find the total number of images
	G4int TotalImages = input -> GetNoImages();

	//Find the total number of particles and convert to a number
	unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());

	G4cout << "\nNumber of projections being processed: " << TotalImages
	       << "\nNumber of photons per image: " << TotalParticles
	       << "\nNumber of detectors: " << DC -> GetNoDetectorsY() << " x " << DC -> GetNoDetectorsZ()
	       << "\nNumber of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ())

	       << "\n\nSimulation Ready!" << G4endl;

	Ready = true;*/
}

void Simulation::RunSimulation()
{
	/*if (Ready == true)
	{
		//Start the simulation timer
		G4Timer FullTime;
		FullTime.Start();

		G4cout << "Starting simulation \n";

		//Prints the time and date of the local time that the simulation started
		time_t now = time(0);
		//Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);
		G4cout << "\n" << asctime(localtm) << "\n";

		//Find the total number of images
		G4int TotalImages = input -> GetNoImages();

		//Find the total number of particles and convert to a number
		unsigned long long int TotalParticles = std::stoull(input->GetNoPhotons());

		for (G4int Image = 0; Image < TotalImages; Image++)
		{
			//Start internal looptimer to update the estimated time for completion
			G4Timer LoopTimer;
			LoopTimer.Start();

			//Creates the arrays for the data, wipes them after each image
			data -> SetUpData(DC -> GetNoDetectorsY(), DC -> GetNoDetectorsZ());
		
			//Beam on to start the simulation
			BeamOn(TotalParticles);
		
			//Prepare for next run that geometry has changed
			G4RunManager::GetRunManager() -> ReinitializeGeometry();
		
			SaveDataToFile();

			//Stop loop timer and estimate the remaining time left
			LoopTimer.Stop();
			CompletionTime(LoopTimer.GetRealElapsed(), Image, TotalImages);
		}
	
		//Stop the full simulation time and save to data class
		FullTime.Stop();
		input -> SetSimulationTime(FullTime.GetRealElapsed());
		input -> WriteToTextFile();

		G4cout << "\n================================================================================"
	               << "\n                        The simulation is finihsed! "
	               << "\n             Total simulation run time : "<< FullTime
	               << "\n================================================================================" << G4endl;
	}
	else if (Ready == false)
	{
		G4cout << "\nSIMULATION IS NOT READY! Check the macro files and initialize the simulation first! \n";
	}*/
}

//Functions to be wrapped by Cython
std::vector<int> Simulation::GetLastImage()
{
	return data -> GetHitData();
}

std::vector<std::vector<int> > Simulation::GetLastEnergyData()
{
	return data -> GetEnergyData();
}

std::vector<int> Simulation::GetEnergyFreq()
{
	return data -> GetEnergyFreq();
}

std::vector<double> Simulation::GetEnergyBins()
{
	return data -> GetEnergyBins();
}

std::vector<int> Simulation::GetBeamEnergyFreq()
{
	return PGA -> GetBeamEnergyFreq();
}

//Private functions
void Simulation::BeamOn(unsigned long long int nParticles)
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

void Simulation::CompletionTime(double LoopTimer, int Image, int NoImages)
{
	//Calculates the estimated time
	G4double ETSeconds = (LoopTimer * NoImages) - (LoopTimer * Image);

	if (NoImages > 1 && Image < NoImages)
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
			{G4cout << "Less than a minute left to go... \n";}
	}
}
