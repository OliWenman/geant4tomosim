//Own classes
#include "Simulation.hh"
#include "SimulationMessenger.hh"
#include "DetectorConstruction.hh"
#include "TargetConstruction.hh"
#include "FluorescenceSD.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"
#include "Data.hh"
#include "DefineMaterials.hh"
//Output to console/write to file
#include "SettingsLog.hh"

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

#include <list>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <xraylib.h>

Simulation::Simulation()
{	
	simMessenger = new SimulationMessenger(this);

	Reset = false;
	Ready = false;

	verboseLevel = 2;

	G4cout << "\nWelcome to the tomography data simulation!"; 
              "\nSetting up... \n";

	//Create an instance of the classes
	runManager = new G4RunManager();
	
	data = new Data();
	DC = new DetectorConstruction(data); 
	PL = new PhysicsList();
	materials = new DefineMaterials(); 
	visManager = 0;
	SaveLogPath = "./../Output/HDF5/";

	//Setup the Geant4 user and action intialization	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);

	PGA = new PrimaryGeneratorAction(data);
	runManager -> SetUserAction(PGA);
	
	particleManager = new StackingAction();
	runManager -> SetUserAction(particleManager);
	particleManager -> SetKillElectrons(true);
	
	stepManager = new SteppingAction();
	runManager -> SetUserAction(stepManager);

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

    //Set the seed engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	
	seedCmd = 0;	
}

Simulation::~Simulation()
{
	G4cout << "\nClosing simulation..." << G4endl;

	delete simMessenger;

	if (runManager){delete runManager;}
	if (visManager){delete visManager;}

	if (data){delete data;}
	if (materials) {delete materials;}
	
	delete CLHEP::HepRandom::getTheEngine();

	G4cout << "Simulation closed! \n" << G4endl;
}

void Simulation::pyOutputOptions(bool FFF, bool FFM)
{
	//Tell the PrimaryGeneratorAction class what data to record
	//PGA -> SetFluoreFM(FFM);

	//Tell the Data class what data to record
	//data -> SetFFF(FFF);
	//data -> SetFFM(FFM);
}

void Simulation::pySetupDetectors(int nDetectorsY, int nDetectorsZ, std::vector<double> DetDimensions, int nBins)
{
    G4cout << "\nAdding detector variables...";

	G4ThreeVector halfDimensions = G4ThreeVector(DetDimensions[0], DetDimensions[1], DetDimensions[2]);

	DC -> SetNoDetectorsY(nDetectorsY);
	DC -> SetNoDetectorsZ(nDetectorsZ);
	DC -> SetDetectorSize(halfDimensions); 

	if (nBins > 0)
		{DC -> SetFluorescenceDet(true);}
	else
		{DC -> SetFluorescenceDet(false);}

	data -> SetNoBins(nBins);
	data -> SetHalfDetectorDimensions(halfDimensions);	

	Ready = true;
	
	G4cout << "\nSuccess!" << G4endl;
}

void Simulation::pyAddMacros(std::vector<std::string> macroFiles)
{
    int nFiles = macroFiles.size();
    
    for (int n = 0; n < nFiles ; n++)
    {
        G4cout << "\nReading macro file " << n + 1 << ") " << macroFiles[n] << G4endl
               << "Adding commands..." << G4endl;
               
        G4int Success = UImanager -> ApplyCommand("/control/execute " + macroFiles[n]);
        
        G4cout << "\nSuccess!" << G4endl; 
    }
    
    macrofiles = macroFiles;
}

//std::vector<int> Simulation::pyRun(unsigned long long int TotalParticles, int NumberOfImages, double rotation_angle, int Image, int nDarkFlatFields)
std::vector<int> Simulation::pyRun(unsigned long long int TotalParticles, std::vector<int> ImageInfo, double rotation_angle, std::vector<double> gunEnergy, G4String gunType)
{   
    //Get the info from the vectors
    int Image = ImageInfo[0];
    int nDarkFlatFields = ImageInfo[1];
    int NumberOfImages = ImageInfo[2];
    
    double monoEnergy = gunEnergy[0];
    double sigmaEnergy = gunEnergy[1];

    //Seed settings
    if(Image == 0)
    {		
        if (seedCmd != 0){
            //Set the seed sequence using inputted seed
            srand(seedCmd);	 
        }
        else {
            //Set random seed sequence with system time
            seedCmd = time(0);
            srand(seedCmd);	 
        }
    }  	    
    
    //Set the seed for this image by calling the rand function (next number in the sequence)
	CLHEP::HepRandom::setTheSeed(rand());

    G4String Mode;
    if (Image < NumberOfImages - nDarkFlatFields){
        Mode = "Simulating";
    }
    else if (Image >= NumberOfImages - nDarkFlatFields){
        Mode = "Calibrating";
    }
 
    TargetConstruction* TC = DC -> GetTargetConstruction();
    TC -> SetSimMode(Mode);
    TC -> SetRotationAngle(rotation_angle);
       
    runManager -> Initialize(); 
       
    if(Image == 0)
    {		  	    
	    int verbose;    
	    if (verboseLevel < 2){verbose = 0;}
	    else {verbose = verboseLevel - 2;}
	    runManager -> SetVerboseLevel(verbose);
		
		//Let the PrimaryGeneratorAction class know where to position the start of the beam
	    PGA-> SetValues(data -> GetNoBins(), DC -> GetWorldSize().x());
		
	    DC -> RelayToTC(NumberOfImages, rotation_angle);
		PGA -> SetNumberOfEvents(TotalParticles, NumberOfImages);
		PGA -> SetFluoreFM(data -> GetFullMapping_Option());
	     
        //Prints the time and date of the local time that the simulation started
		time_t now = time(0);
		//Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);

        Visualisation();
                
        G4cout << "\n--------------------------------------------------------------------"
	              "\nStarting simulation... \n";
			          
	    G4cout << "\n" << asctime(localtm);
		    
		if (TotalParticles > 0){
	        G4cout << "\n================================================================================"
	                  "\n                                 Geant4 info"
                      "\n================================================================================" << G4endl;
        }
	}

    //Prepare for the run that geometry has changed
	runManager -> ReinitializeGeometry();
    
    //Prepare for next run. Check if energy or gun has changed 
    PGA -> ResetEvents(Image + 1);   
    PGA -> SetupGun(gunType, monoEnergy, sigmaEnergy);

	//Creates the arrays for the data, wipes them after each image
	data -> SetUpData(DC -> GetNoDetectorsY(), DC -> GetNoDetectorsZ(), Image);
		
    //G4cout << "\n3.448*m = " << 3.448*m << G4endl;exit(0);
		
    //Beam on to start the simulation
    BeamOn(TotalParticles);

	if (Image + 1 == NumberOfImages)
	{
       G4cout << "\n================================================================================"
                 "\n                        The simulation is finished! "
                 "\n================================================================================" << G4endl;
	            
        TargetConstruction* TC = DC -> GetTargetConstruction();         
        TC -> SetCurrentImage(0);
        DC -> SetCurrentImage(0);
        PGA -> ResetEvents(0);
	}
	
	return data -> GetHitData();
}

void Simulation::Visualisation()
{
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
		visManager = new G4VisExecutive("quiet");

		visManager -> Initialize();

        //Make the name of the file the same as the logname, remove the "_log.txt" at the end and replace with _vis
        std::string visFileName = FileName.substr (0, FileName.length() - 8) + "_vis";    
        
        //Set the filepath to save it, if the defualt is used, save in the Visualization folder
        std::string filePath;
        std::string defaultPath = "./../Output/HDF5/";
        if (SaveLogPath == defaultPath){
            filePath = "./../Output/Visualization/";
        }
        else {
            //If a different path is used, create a folder to store the visualization data 
            filePath = SaveLogPath + "Visualization/";
            const char *filepath_char = filePath.c_str();
            
            //Check if the directory exists
            DIR* Directory_exists = opendir(filepath_char);
            
            //If it doesn't exist, create it
            if (!Directory_exists){
                std::string mkdir = "mkdir " + filePath;
                const char *mkdir_char = mkdir.c_str();
                system(mkdir_char);
                delete mkdir_char;
            }
            //Make sure to delete pointers once done with them
            if (Directory_exists){delete Directory_exists;}
        }
		
		//Setup the vis manager
		UImanager -> ApplyCommand("/vis/open HepRepFile");
		UImanager -> ApplyCommand("/vis/heprep/setFileDir " + filePath);
		UImanager -> ApplyCommand("/vis/heprep/setFileName " + visFileName);
		UImanager -> ApplyCommand("/vis/viewer/set/autoRefresh false");
		UImanager -> ApplyCommand("/vis/verbose errors");
		UImanager -> ApplyCommand("/vis/viewer/flush");
		UImanager -> ApplyCommand("/vis/drawVolume");
		
		//Set the draw options
		UImanager -> ApplyCommand("/vis/viewer/set/style wireframe");
		UImanager -> ApplyCommand("/vis/viewer/set/auxiliaryEdge true");
		UImanager -> ApplyCommand("/vis/viewer/set/lineSegmentsPerCircle 100");
		UImanager -> ApplyCommand("/vis/scene/add/trajectories smooth");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/create/drawByCharge");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2");
		
		UImanager -> ApplyCommand("/vis/scene/add/axes 0 0 0 1 mm");
		UImanager -> ApplyCommand("/vis/viewer/zoom 0.5 mm");
		
		UImanager -> ApplyCommand("/vis/scene/add/hits");
		UImanager -> ApplyCommand("/vis/scene/endOfEventAction accumulate");
		UImanager -> ApplyCommand("/vis/viewer/set/autoRefresh true");
		
		G4cout << "\nGRAPHICS HAVE BEEN ENABLED! \nSaving as " << filePath + visFileName + "N.heprep where N is an integer "<< G4endl;
			
		PGA -> SetProgressBar(false);
	}
}

//Returns the last image the simulate did
std::vector<int> Simulation::GetLastImage(){
	return data -> GetHitData();
}
//Returns the last fluorescence array
std::vector<int> Simulation::GetFluorescence(){
	return data -> GetFluorescence();
}
//Returns the x scale for the energy
std::vector<double> Simulation::GetEnergyBins(){
	return data -> GetEnergyBins();
}
//Gets the beam energy for the last image
std::vector<int> Simulation::GetBeamEnergy(){
	return PGA -> GetBeamEnergy();
}
//Gets the last full mapping fluorescence data
std::vector<std::vector<std::vector<int> > > Simulation::GetFullMapping(){
	return data -> GetFullMapping();
}
//Sets the saving time for the estimated time to take into account
void Simulation::SetSavingTime(double Time){
    PGA -> SetSavingTime(Time);
}

//Function to run the simulation, will repeat simulation if over max limit
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

//Function to limit the number of particles if graphics is turned on
unsigned long long int Simulation::LimitGraphics(unsigned long long int nParticles, int nImage, std::string Mode)
{
    if (DC -> GetVisualization() == true && nParticles > 5000)
    {   
        int limit = 100;
        nParticles = limit;     
            
        if (nImage == 0)
        {
            G4cout << "\n////////////////////////////////////////////////////////////////////////////////\n"
                      "\n         WARNING: " << nParticles << " PARTICLES IS TOO MANY TOO SIMULATE WITH GRAPHICS"
                      "\n                    Reducing the number of particles to " << limit << "\n"
                      "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;;         
        }
    }
    
    return nParticles;
}

//Function to log the inforimation about the simulation. Outputs to terminal depending on verbose level. Always outputs to _log.txt file
void Simulation::PrintInfo(unsigned long long int TotalParticles, int NumberOfImages, int nDarkFlatFields)
{   
    int verbose = 1;
    
    std::ofstream SaveToFile;
    
    //Try to open the file 
    SaveToFile.open(SaveLogPath + FileName); 
    if( !SaveToFile ) { 	
        std::cerr << "\nError: " << SaveLogPath + FileName << " file could not be opened from Simulation. " << "\n" << std::endl;
        exit(1);
    }
    
    //Create an instance of the log to output to terminal and file    
    SettingsLog log(SaveToFile);

    PrintInformation(verbose, log);
	
	log << "\n--------------------------------------------------------------------"
		   "\nMETA DATA: \n"

        << "\n- The seed used: " << seedCmd
        << "\n- Total number of projections being processed: " << NumberOfImages
	    << "\n  - Dark fields: " << nDarkFlatFields
	    << "\n  - Sample: " << NumberOfImages - nDarkFlatFields
        << "\n- Number of photons per image: " << TotalParticles
        << "\n- Number of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ()) << G4endl;
	 
	SaveToFile.close();
}

void Simulation::PrintInfo(int verbose)
{
    std::ofstream SaveToFile;
    
    //Try to open the file 
    SaveToFile.open(SaveLogPath + FileName); 
    if( !SaveToFile ) { 	
        std::cerr << "\nError: " << SaveLogPath + FileName << " file could not be opened from Simulation. " << "\n" << std::endl;
        exit(1);
    }
    
    //Create an instance of the log to output to terminal and file    
    SettingsLog log(SaveToFile);

    PrintInformation(verbose, log);
    SaveToFile.close();
}

void Simulation::PrintInformation(int verbose, SettingsLog log)
{       
    //If the verbose has been set >= 2 output info to terminal. Will always output to textfile though
    if (verbose >= 2) {log.terminalOn = true;}
    else              {log.terminalOn = false;}
    
    //Loop through the macro files
    int nFiles = macrofiles.size();
    for (int n = 0 ; n < nFiles ; n++)
    {      
        std::ifstream ReadFile;
        
        //Try to open the macro file      
        ReadFile.open(macrofiles[n]);
        if (!ReadFile) {
            G4cout << "\nERROR: Unable to open " << macrofiles[n] << " for log output. " << G4endl;
            exit(1); 
        }
      
        log << "\n--------------------------------------------------------------------"
               "\nMacro file " << n +1 << ": " << macrofiles[n] << "\n" << G4endl; 
      
        //Read the file
        std::string Line;    
        while ((std::getline(ReadFile, Line))) {
            //if (Line.find('#') == std::string::npos)
            if(Line[1] != '/'){
                log << Line << G4endl;}
        }
        ReadFile.close();
    }
    
    if (verbose >= 1) {log.terminalOn = true;}
    else              {log.terminalOn = false;}
    
    //Log the info from other classes
    DC -> ReadOutInfo(log);
	PGA -> ReadOutInfo(log);
	PL -> ReadOutInfo(log);
}
